#include "OpenAIClient.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

OpenAIClient::OpenAIClient(const std::string &api_key) : api_key_(api_key)
{
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

    int retry_count = 0;
    int max_retries = 5;
    std::string generated_text = "";

    while (retry_count < max_retries)
    {
        std::string response = curl_handler_->post(url, data, true);

        nlohmann::json response_json = nlohmann::json::parse(response);
        generated_text += response_json["choices"][0]["message"]["content"];

        if (generated_text.back() == '.' || generated_text.back() == '?' || generated_text.back() == '!')
        {
            break;
        }

        payload["messages"].push_back({{"role", "user"}, {"content", generated_text}});
        data = payload.dump();

        retry_count++;
    }

    return generated_text;
}


bool OpenAIClient::is_question(const std::string &input)
{
    int count = 0;

    if (is_question_based_on_punctuation(input))
    {
        count++;
    }
    else if (is_question_based_on_keywords(input))
    {
        count++;
    }

    if (is_question_based_on_embeddings(input))
    {
        count++;
    }

    return count >= 2;
}

bool OpenAIClient::is_question_based_on_punctuation(const std::string &input)
{
    return !input.empty() && input.back() == '?';
}

bool OpenAIClient::is_question_based_on_keywords(const std::string &input)
{
    std::string lower_input = input;
    std::transform(lower_input.begin(), lower_input.end(), lower_input.begin(), ::tolower);
    const std::vector<std::string> question_words = {"what", "how", "where", "when", "why", "who", "which", "is", "are", "do", "does", "did", "can", "could", "would", "will", "shall"};

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
    std::vector<float> input_embedding = get_text_embedding(input);

    float average_similarity = 0.0f;
    for (const std::vector<float> &example_embedding : example_question_embeddings_)
    {
        float similarity = cosine_similarity(input_embedding, example_embedding);
        average_similarity += similarity;
    }
    average_similarity /= example_question_embeddings_.size();

    const float similarity_threshold = 0.7f;
    return average_similarity >= similarity_threshold;
}


std::vector<float> OpenAIClient::get_text_embedding(const std::string &text)
{
    std::string url = "https://api.openai.com/v1/embeddings";

    nlohmann::json payload = {
        {"input", text},
        {"model", "text-embedding-ada-002"}
    };

    std::string data = payload.dump();
    std::string response = curl_handler_->post(url, data, true);

    nlohmann::json response_json = nlohmann::json::parse(response);

    std::vector<float> embedding = response_json["data"][0]["embedding"].get<std::vector<float>>();

    return embedding;
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