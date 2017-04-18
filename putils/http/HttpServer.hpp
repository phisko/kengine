#pragma once

#include "mediator/Mediator.hpp"

#include "modules/LoggerModule.hpp"
#include "modules/ProtocolModule.hpp"
#include "modules/RoutesModule.hpp"
#include "modules/SecureConnectionModule.hpp"

#include "Process.hpp"
#include "to_string.hpp"

namespace putils
{
    class HttpServer
    {
    public:
        HttpServer(short normalPort = 4242, short securePort = 4243)
        {
            _modules.push_back(std::make_unique<LoggerModule>());
            _modules.push_back(std::make_unique<ProtocolModule>());
            _modules.push_back(std::make_unique<SecureConnectionModule>(_mediator, normalPort, securePort));

            for (const auto &m : _modules)
                _mediator.addModule(m.get());

            _mediator.addModule(&_routes);
        }

    public:
        void get(const std::string &uri, const RoutesModule::Route &route)
        {
            _routes.addRoute(uri, RoutesModule::Method::Get, route);
        }

    public:
        void post(const std::string &uri, const RoutesModule::Route &route)
        {
            _routes.addRoute(uri, RoutesModule::Method::Post, route);
        }

    private:
        RoutesModule _routes;
        std::vector<std::unique_ptr<putils::BaseModule>> _modules;
        putils::Mediator _mediator;
    };

    namespace test
    {
        inline void httpServer()
        {
            putils::HttpServer server(4242, -1);

            // Home page
            server.get("/", [](const auto &)
            {
                return "saucisseuh";
            });

            // Run an executable
            server.get("/run/{first}", [](const auto &params)
            {
                putils::Process::Options options;
                options.stdout.redirected = true;

                putils::Process p(params.at("first"), options);

                return putils::to_string(p.getStdout());
            });

            // Post
            server.post("/{first}", [](const auto &params)
            {
                return params.at("first") + " " + params.at("str");
            });

            // Let modules do their thing in their threads
            std::string buff;
            while (std::getline(std::cin, buff))
                if (buff == "exit")
                    return;
        }
    }
}