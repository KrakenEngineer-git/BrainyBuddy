#ifndef OPENAI_CLIENT_HPP
#define OPENAI_CLIENT_HPP


#include <string>
#include "CurlHandler/CurlHandler.hpp"
#include "utilities/utilities.hpp"

class OpenAIClient
{
public:
    OpenAIClient(const std::string &api_key);
    std::string generate_response(const std::string &input);

private:
    std::unique_ptr<CurlHandler> curl_handler_;
    std::string api_key_;
};


#endif // OPENAI_CLIENT_HPP