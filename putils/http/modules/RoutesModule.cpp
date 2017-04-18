#include <regex>
#include <fstream>
#include <iostream>
#include "RoutesModule.hpp"

#include "concat.hpp"

void RoutesModule::addRoute(const std::string &uri, RoutesModule::Method method, const RoutesModule::Route &route)
{
    RoutesModule::Uri toAdd;
    std::string reg = "^";

    for (std::size_t i = 0; i < uri.length(); ++i)
    {
        if (uri[i] == '{')
        {
            std::string param = "";

            ++i;
            while (i < uri.length() && uri[i] != '}')
            {
                param.append(1, uri[i]);
                ++i;
            }
            ++i;

            toAdd.params.push_back(param);

            if (i > uri.length())
                throw std::runtime_error("Unmatched '{' in route");

            if (i == uri.length())
                reg += "(.*)";
            else
                reg += putils::concat("([^", uri[i], "]*)", uri[i]);
        }
        else
            reg.append(1, uri[i]);
    }

    reg += "$";

    toAdd.reg = reg;
    toAdd.route = route;

    if (method == Get)
        _get.push_back(std::move(toAdd));
    else if (method == Post)
        _post.push_back(std::move(toAdd));
}

void RoutesModule::handle(const kia::packets::HttpRequest &p) const noexcept
{
    const auto &vec = (p.method == "GET") ? _get : _post;

    for (const auto &uri : vec)
    {
        std::smatch m;

        if (std::regex_match(p.uri, m, uri.reg))
        {
            HttpParams params = p.params;

            for (std::size_t i = 0; i < uri.params.size(); ++i)
                params[uri.params[i]] = m[i + 1];

            // Normal behavior: just transfer the request with the right route
            sendResponse(p, uri.route(params));
            return;
        }
    }

    // No route for that uri
    auto err = std::string("[Routes] Unknown route ") + p.uri;
    send(kia::packets::Log{ err });
    send404(p);
}

void RoutesModule::sendResponse(const kia::packets::HttpRequest &p, const std::string &response) const noexcept
{
    auto packet = kia::success(p);
    packet.body = response;
    send(std::move(packet));
}

void RoutesModule::send404(const kia::packets::HttpRequest &request) const noexcept
{
    // TODO: Load an error page specified in routes.txt
    auto response = kia::error(request, "404");
    response.httpVersion = "HTTP/1.1";
    response.body = "404 Not found\r\n";
    send(std::move(response));
}
