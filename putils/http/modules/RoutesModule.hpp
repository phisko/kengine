#pragma once

#include <regex>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include "mediator/Module.hpp"

#include "Packets.hpp"

class RoutesModule : public putils::Module<RoutesModule, kia::packets::HttpRequest>
{
public:
    using HttpParams = std::unordered_map<std::string, std::string>;
    using Route = std::function<std::string(const HttpParams &)>;

    struct Uri
    {
        std::regex reg;
        std::vector<std::string> params;
        Route route;
    };

public:
    void handle(const kia::packets::HttpRequest &p) const noexcept;

public:
    enum Method
    {
        Get,
        Post
    };
    void addRoute(const std::string &uri, Method method, const Route &route);

private:
    void sendResponse(const kia::packets::HttpRequest &p, const std::string &response) const noexcept;

private:
    void send404(const kia::packets::HttpRequest &request) const noexcept;

private:
    std::vector<Uri> _get;
    std::vector<Uri> _post;
};
