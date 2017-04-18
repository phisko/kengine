#pragma once

#include <string>
#include <unordered_map>

namespace kia
{
    namespace packets
    {
        struct Log
        {
            std::string msg;
        };

        struct HttpRequest
        {
            int clientFd;
            std::string method;
            std::string uri;
            std::string httpVersion;
            std::unordered_map<std::string, std::string> headers;
            std::unordered_map<std::string, std::string> params;
            std::string body;
        };

        struct HttpResponse
        {
            int clientFd;
            std::string httpVersion;
            std::string statusCode;
            std::string reasonPhrase;
            std::unordered_map<std::string, std::string> headers;
            std::string body;
        };

        struct IncomingMessage
        {
            int clientFd;
            std::string msg;
        };

        struct OutgoingMessage
        {
            int clientFd;
            std::string msg;
        };
    }

    inline packets::HttpResponse success(const packets::HttpRequest &origin, const std::string &statusCode = "200", const std::string &reasonPhrase = "")
    {
        return packets::HttpResponse{ origin.clientFd, origin.httpVersion, statusCode, reasonPhrase, {}, ""};
    }

    inline packets::HttpResponse error(const packets::HttpRequest &origin, const std::string &statusCode = "400", const std::string &reasonPhrase = "")
    {
        return packets::HttpResponse{ origin.clientFd, origin.httpVersion, statusCode, reasonPhrase, {}, ""};
    }

}
