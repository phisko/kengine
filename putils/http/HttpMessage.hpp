#pragma once

#include <unordered_map>
#include <string>

namespace Http
{
    struct Request
    {
        std::string method;
        std::string uri;
        std::string httpVersion;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
    };

    struct Response
    {
        std::string httpVersion;
        std::string statusCode;
        std::string reasonPhrase;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
    };
}
