#include "OpenAIClient.hpp"
#include <algorithm>
#include <cmath>
#include <future>
#include <iostream>
#include <sstream>

constexpr int MAX_TOKENS = 200;
constexpr double TEMPERATURE = 0.8;
constexpr int MAX_RETRIES = 5;
constexpr float SIMILARITY_THRESHOLD = 0.65f;
constexpr int CURL_THREADS_NUMBER = 2;

OpenAIClient::OpenAIClient(const std::string &api_key) : api_key_(api_key)
{
    setupCurlHandler();
    setupThreadPool();
    generateExampleEmbeddings();
}

OpenAIClient::~OpenAIClient()
{
    std::cout << "OpenAIClient destructor called" << std::endl;
}

void OpenAIClient::setupCurlHandler()
{
    curl_handler_ = std::make_unique<CurlHandler>();
    curl_handler_->AddHeader("Authorization: Bearer " + api_key_);
    curl_handler_->AddHeader("Content-Type: application/json");
}

void OpenAIClient::setupThreadPool()
{
    thread_pool_ = std::make_unique<ThreadPool>(4);
}

void OpenAIClient::generateExampleEmbeddings()
{
    for (const auto &example : question_examples_)
    {
        try
        {
            example_question_embeddings_.push_back(getTextEmbedding(example));
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error generating example embeddings: " << e.what() << std::endl;
        }
    }
}

std::string OpenAIClient::generate_response(const std::string &input,
                                            const std::string &author_username) noexcept(false)
{
    std::cout << "OpenAIClient::generate_response" << std::endl;
    if (input.empty())
    {
        throw std::invalid_argument("Input cannot be empty.");
    }

    std::string url = "https://api.openai.com/v1/chat/completions";

    nlohmann::json payload = {
        {"model", "gpt-3.5-turbo-0301"},
        {"messages",
         {{{"role", "system"},
           {"content",
            "You are an AI assistant designed to answer questions from users on a Discord server related to League of "
            "legends community application named Team Advisor. Your current conversation is with " +
                author_username + "."}},
          {{"role", "user"}, {"content", input}}}},
        {"max_tokens", MAX_TOKENS},
        {"n", 1},
        {"stop", nullptr},
        {"temperature", TEMPERATURE},
    };

    std::string data = payload.dump();

    std::string generated_text;
    for (int retry = 0; retry < MAX_RETRIES; ++retry)
    {
        auto response = enqueueTask(url, data);

        nlohmann::json response_json = nlohmann::json::parse(response.body);

        if (response_json.contains("error"))
        {
            throw std::runtime_error("Error from the API: " + response_json["error"].get<std::string>());
        }

        if (!response_json.contains("choices") || response_json["choices"].empty())
        {
            throw std::runtime_error("Unexpected response from the API");
        }

        generated_text += response_json["choices"][0]["message"]["content"].get<std::string>();

        if (generated_text.empty() || generated_text.back() == '.' || generated_text.back() == '?' ||
            generated_text.back() == '!')
        {
            break;
        }

        payload["messages"].push_back({{"role", "user"}, {"content", generated_text}});
        data = payload.dump();
    }

    return generated_text;
}

CurlHandler::Response OpenAIClient::enqueueTask(const std::string &url, const std::string &data)
{
    std::lock_guard<std::mutex> lock(curl_handler_mutex_);
    std::shared_ptr<std::promise<CurlHandler::Response>> response_promise =
        std::make_shared<std::promise<CurlHandler::Response>>();
    std::future<CurlHandler::Response> response_future = response_promise->get_future();

    curl_handler_->enqueue_request([this, url, data, response_promise]() {
        try
        {
            CurlHandler::Response response = this->curl_handler_->post(url, data, true);
            response_promise->set_value(response);
        }
        catch (const std::exception &e)
        {
            response_promise->set_exception(std::current_exception());
        }
    });
    return response_future.get();
}

bool OpenAIClient::is_question(const std::string &input)
{
    int count = 0;

    if (isQuestionBasedOnPunctuation(input))
    {
        count++;
    }
    else if (isQuestionBasedOnKeywords(input))
    {
        count++;
    }

    if (isQuestionBasedOnEmbeddings(input))
    {
        count++;
    }

    return count >= 2;
}

bool OpenAIClient::isQuestionBasedOnPunctuation(const std::string &input)
{
    return !input.empty() && input.back() == '?';
}

bool OpenAIClient::isQuestionBasedOnKeywords(const std::string &input)
{
    std::string lower_input = input;
    std::transform(lower_input.begin(), lower_input.end(), lower_input.begin(), ::tolower);
    const std::vector<std::string> question_words = {"what",  "how",  "where", "when",    "why",    "who",     "which",
                                                     "is",    "are",  "do",    "does",    "did",    "can",     "could",
                                                     "would", "will", "shall", "explain", "define", "describe"};

    lower_input = lower_input.substr(lower_input.find_first_not_of(' '));
    std::stringstream input_stream(lower_input);
    std::string first_word;
    input_stream >> first_word;

    if (first_word.empty())
    {
        return false;
    }

    for (const auto &word : question_words)
    {
        if (first_word == word)
        {
            return true;
        }
    }

    return false;
}

bool OpenAIClient::isQuestionBasedOnEmbeddings(const std::string &input)
{
    std::vector<float> input_embedding = getTextEmbedding(input);

    float average_similarity = 0.0f;
    for (const auto &example_embedding : example_question_embeddings_)
    {
        float similarity = cosineSimilarity(input_embedding, example_embedding);
        average_similarity += similarity;
    }
    average_similarity /= example_question_embeddings_.size();

    return average_similarity >= SIMILARITY_THRESHOLD;
}

std::vector<float> OpenAIClient::getTextEmbedding(const std::string &text)
{
    std::string url = "https://api.openai.com/v1/embeddings";
    nlohmann::json payload = {{"input", text}, {"model", "text-embedding-ada-002"}};

    std::string data = payload.dump();
    auto response = enqueueTask(url, data);

    try
    {
        nlohmann::json response_json = nlohmann::json::parse(response.body);
        if (response_json.contains("data") && !response_json["data"].empty())
        {
            auto embedding_json = response_json["data"][0].value("embedding", nlohmann::json::object());
            if (!embedding_json.empty())
            {
                return embedding_json.get<std::vector<float>>();
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error parsing text embedding response: " << e.what() << std::endl;
    }

    return std::vector<float>();
}

float OpenAIClient::cosineSimilarity(const std::vector<float> &a, const std::vector<float> &b)
{
    if (a.size() != b.size())
    {
        throw std::invalid_argument("Vectors must be the same size to calculate cosine similarity");
    }

    float dot_product = 0.0f;
    float a_norm = 0.0f;
    float b_norm = 0.0f;

    for (size_t i = 0; i < a.size(); ++i)
    {
        dot_product += a[i] * b[i];
        a_norm += a[i] * a[i];
        b_norm += b[i] * b[i];
    }

    float denominator = std::sqrt(a_norm) * std::sqrt(b_norm);
    if (denominator == 0)
    {
        throw std::invalid_argument("Cannot calculate cosine similarity: division by zero");
    }

    return dot_product / denominator;
}
