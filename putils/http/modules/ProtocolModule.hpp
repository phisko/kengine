#pragma once

#include "mediator/Module.hpp"
#include "Packets.hpp"

class ProtocolModule final : public putils::Module<ProtocolModule,
    kia::packets::IncomingMessage, kia::packets::HttpResponse>
{
public:
    void handle(const kia::packets::HttpResponse &packet) const noexcept;
    void handle(const kia::packets::IncomingMessage &packet) const noexcept;

private:
    void getParams(kia::packets::HttpRequest &request, const std::string &line) const noexcept;
    void getRequestLine(kia::packets::HttpRequest &request, const std::string &line) const noexcept;
    void addHeader(kia::packets::HttpRequest &request, const std::string &line) const noexcept;
};
