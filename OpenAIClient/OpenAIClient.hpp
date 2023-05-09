#ifndef OPENAI_CLIENT_HPP
#define OPENAI_CLIENT_HPP


#include <string>
#include "CurlHandler/CurlHandler.hpp"
#include "utilities/utilities.hpp"

class OpenAIClient
{
public:
    OpenAIClient(const std::string &api_key);
    std::string generate_response(const std::string &input,const std::string &author_username);
    bool is_question(const std::string &input);

private:
    bool is_question_based_on_punctuation(const std::string &input);
    bool is_question_based_on_keywords(const std::string &input);
    bool is_question_based_on_embeddings(const std::string &input);
    std::vector<float> get_text_embedding(const std::string &text);
    float cosine_similarity(const std::vector<float> &a, const std::vector<float> &b);
    std::unique_ptr<CurlHandler> curl_handler_;
    std::string api_key_;
    std::vector<std::vector<float>> example_question_embeddings_;
    
    std::vector<std::string> question_examples = {
        "What is the capital of France?",
        "How does photosynthesis work?",
        "What are the main components of a computer?",
        "What's the weather like today?",
        "What is the square root of 144?",
        "How do I cook spaghetti?",
        "What strategies can I use to improve my League of Legends gameplay?",
        "What's the best way to learn a new champion in League of Legends?",
        "How can I find the optimal build for my champion?",
        "What are the key differences between low and high elo playstyles?",
        "What's the best way to communicate with teammates during a match?",
        "How do I climb the ranked ladder in League of Legends?",
        "What are some common mistakes to avoid while playing?",
        "How important is vision control in the game?",
        "What are some tips for playing against a specific champion?",
        "How can I efficiently manage my in-game resources?",
        "Which champions are currently strong in the meta?",
        "What are the benefits of a well-coordinated team?",
        "How do I adapt my playstyle when facing different team compositions?",
        "What are some good strategies for split pushing?",
        "How do I deal with a fed enemy champion?",
        "What are the key objectives to focus on during a match?",
        "How do I choose the right summoner spells for my champion?",
        "How can I effectively play from behind when my team is losing?",
        "What's the best way to practice and improve my mechanics?"
    };

};


#endif // OPENAI_CLIENT_HPP