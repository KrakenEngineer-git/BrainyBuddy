#include "OpenAIClient.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <future>

OpenAIClient::OpenAIClient(const std::string &api_key) : api_key_(api_key)
{
    thread_pool_ = make_unique<ThreadPool>(4);
    curl_handler_ = make_unique<CurlHandler>();
    curl_handler_->AddHeader("Authorization: Bearer " + api_key_);
    curl_handler_->AddHeader("Content-Type: application/json");

    for (const std::string &example : question_examples)
    {
        example_question_embeddings_.push_back(get_text_embedding(example));
    }
}

std::string OpenAIClient::generate_response(const std::string &input, const std::string &author_username)
{
    std::cout << "Generating response for input: " << input << std::endl;
    std::string url = "https://api.openai.com/v1/chat/completions";
    
    const int max_tokens = 200;
    const double temperature = 0.8;

    nlohmann::json payload = {
        {"model", "gpt-3.5-turbo-0301"},
        {
            "messages",
            {
                {
                    {"role", "system"},
                    {"content", "You are an AI assistant designed to answer questions from users on a Discord server related to League of legends community application named Team Advisor. Your current conversation is with " + author_username + "."}},
                {
                    {"role", "user"},
                    {"content", input}
                }
            }
        },
        {"max_tokens", max_tokens},
        {"n", 1},
        {"stop", nullptr},
        {"temperature", temperature},
    };

    std::string data = payload.dump();

    std::string generated_text = "";
    const int max_retries = 5;

    for (int retry = 0; retry < max_retries; ++retry)
    {
        std::string response;
        std::promise<std::string> response_promise;
        std::future<std::string> response_future = response_promise.get_future();
        thread_pool_->enqueue_task([this, &url, &data, &response, &response_promise]() {
            std::string response = curl_handler_->post(url, data, true);
            response_promise.set_value(response);
        });

        response = response_future.get();

        nlohmann::json response_json = nlohmann::json::parse(response);

        if (response_json.contains("choices") && !response_json["choices"].empty()) {
            auto message = response_json["choices"][0].value("message", nlohmann::json::object());
            if (!message.empty()) {
                generated_text += message.value("content", "");
            }
        }   

        if (generated_text.back() == '.' || generated_text.back() == '?' || generated_text.back() == '!')
        {
            break;
        }

        payload["messages"].push_back({{"role", "user"}, {"content", generated_text}});
        data = payload.dump();
    }


    return generated_text;
}


bool OpenAIClient::is_question(const std::string &input)
{
    std::cout<<"Checking if question"<<std::endl;
    int count = 0;
    float weight_rule_based = 0.6f;
    float weight_embeddings = 0.4f;

    if (is_question_based_on_punctuation(input))
    {
        count++;
    }
    else if (is_question_based_on_keywords(input))
    {
        count += weight_rule_based;
    }

    if (is_question_based_on_embeddings(input))
    {
        count += weight_embeddings;
    }

    return count >= (1.0f + weight_rule_based * 0.5f);
}

bool OpenAIClient::is_question_based_on_punctuation(const std::string &input)
{
    return !input.empty() && input.back() == '?';
}

bool OpenAIClient::is_question_based_on_keywords(const std::string &input)
{
    std::cout << "Checking if question based on keywords" << std::endl;
    std::string lower_input = input;
    std::transform(lower_input.begin(), lower_input.end(), lower_input.begin(), ::tolower);
    const std::vector<std::string> question_words = {"what", "how", "where", "when", "why", "who", "which", "is", "are", "do", "does", "did", "can", "could", "would", "will", "shall", "explain", "define", "describe"};

    std::stringstream input_stream(lower_input);
    std::string first_word;
    input_stream >> first_word;

    for (const auto &word : question_words)
    {
        if (first_word == word)
        {
            return true;
        }
    }

    return false;
}

bool OpenAIClient::is_question_based_on_embeddings(const std::string &input)
{
    std::cout << "Checking if question based on embeddings" << std::endl;
    std::vector<float> input_embedding = get_text_embedding(input);

    float average_similarity = 0.0f;
    for (const std::vector<float> &example_embedding : example_question_embeddings_)
    {
        float similarity = cosine_similarity(input_embedding, example_embedding);
        average_similarity += similarity;
    }
    average_similarity /= example_question_embeddings_.size();

    const float similarity_threshold = 0.65f; 
    return average_similarity >= similarity_threshold;
}


std::vector<float> OpenAIClient::get_text_embedding(const std::string &text)
{
    std::cout << "Getting text embedding for text: " << text << std::endl;
    std::string url = "https://api.openai.com/v1/embeddings";
    nlohmann::json payload = {
        {"input", text},
        {"model", "text-embedding-ada-002"}
    };

    std::string data = payload.dump();
    std::string response;
    std::promise<std::string> response_promise;
    std::future<std::string> response_future = response_promise.get_future();

    thread_pool_->enqueue_task([this, &url, &data, &response, &response_promise]() {
        try {
            response = curl_handler_->post(url, data, true);
            response_promise.set_value(response);
        } catch (const std::exception& e) {
            response_promise.set_exception(std::current_exception());
        }
    });

    try {
        response = response_future.get();
    } catch (const std::exception& e) {
        std::cerr << "Error getting text embedding: " << e.what() << std::endl;
        return std::vector<float>();
    }

    try {
        nlohmann::json response_json = nlohmann::json::parse(response);
        if (response_json.contains("data") && !response_json["data"].empty()) {
            auto embedding_json = response_json["data"][0].value("embedding", nlohmann::json::object());
            if (!embedding_json.empty()) {
                return embedding_json.get<std::vector<float>>();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing text embedding response: " << e.what() << std::endl;
    }

    return std::vector<float>();
}



float OpenAIClient::cosine_similarity(const std::vector<float> &a, const std::vector<float> &b)
{
    float dot_product = 0.0f;
    float a_norm = 0.0f;
    float b_norm = 0.0f;

    for (size_t i = 0; i < a.size(); ++i)
    {
        dot_product += a[i] * b[i];
        a_norm += a[i] * a[i];
        b_norm += b[i] * b[i];
    }

    return dot_product / (std::sqrt(a_norm) * std::sqrt(b_norm));
}