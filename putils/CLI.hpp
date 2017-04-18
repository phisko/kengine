#pragma once

#include <atomic>
#include <vector>
#include <sstream>
#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>
#include "fwd.hpp"

#ifdef __unix__
#include <signal.h>
#endif

namespace putils
{
    class CLI
    {
    public:
        struct Param
        {
            std::string value;
            std::pair<char, char> delimiters;
        };
        using Command = std::function<void(const std::string &cmd, const std::vector<Param> &params)>;
        using CommandMap = std::unordered_map<std::string, Command>;

    public:
        CLI(const CommandMap &funcs,
            const Command &default_ = [](const std::string &cmd, const std::vector<Param> &) { std::cout << "Unknown command: " << cmd << std::endl; },
            const std::function<std::string()> &prompt = []{ return "> "; },
            std::vector<std::pair<char, char>> &&delimiters =
                    {
                            { '\"', '\"'},
                            { '\'', '\''},
                            {'(', ')'}
                    })
                : _funcs(funcs), _default(default_), _prompt(prompt), _delimiters(std::move(delimiters))
        {
#ifdef __unix__
            signal(SIGINT, [](int){ std::cout << std::endl; });
#endif
            std::cout << _prompt() << std::flush;
        }

    public:
        void run() const
        {
            std::string buff;
            while (running && std::getline(std::cin, buff))
            {
                if (!buff.length())
                {
                    std::cout << _prompt() << std::flush;
                    continue;
                }

                executeCommand(buff);
                std::cout << _prompt() << std::flush;
            }
        }

    public:
        void step()
        {
            std::string buff;
            if (!std::getline(std::cin, buff))
            {
                running = false;
                return;
            }

            if (!buff.length())
            {
                std::cout << _prompt() << std::flush;
                return;
            }

            executeCommand(std::move(buff));
            std::cout << _prompt() << std::flush;
        }

    public:
        void displayPrompt() const
        {
            std::cout << _prompt();
        }

    public:
        void executeCommand(std::string buff) const
        {
            try
            {
                std::stringstream s(buff);

                auto cmd = getCommand(s);
                auto p = getParams(s);

                const auto it = _funcs.find(cmd);
                if (it != _funcs.end())
                    it->second(cmd, p);
                else
                    _default(cmd, p);
            }
            catch (const std::logic_error &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

    private:
        std::string getCommand(std::stringstream &s) const
        {
            return extract(s).value;
        }

        std::vector<Param> getParams(std::stringstream &s) const
        {
            std::vector<Param> ret;

            while (s && s.peek() != -1)
                ret.push_back(extract(s));

            return ret;
        }

        Param extract(std::stringstream &s) const
        {
            while (std::isspace(s.peek()))
                s.get();

            {
                char c = s.peek();
                for (const auto &p : _delimiters)
                    if (c == p.first)
                        return extractUntilDelimiter(s, p);
            }

            Param ret{"", {0, 0}};
            while (s && !std::isspace(s.peek()) && s.peek() != -1)
                ret.value.append(1, s.get());

            return ret;
        }

        Param extractUntilDelimiter(std::stringstream &s, const std::pair<char, char> &p) const
        {
            Param ret{"", p};

            s.get(); // Skip first delimiter

            char c;
            while (s && (c = s.get()) != p.second)
            {
                if (c == '\\' && s)
                    ret.value.append(1, s.get());
                else
                    ret.value.append(1, c);
            }
            if (c != p.second)
                throw std::logic_error(std::string("No matching delimiter (").append(1, p.second).append(") found for ").append(1, p.first));

            return ret;
        }

    public:
        std::atomic<bool> running { true };

    private:
        CommandMap _funcs;
        Command _default;
        std::function<std::string()> _prompt;
        std::vector<std::pair<char, char>> _delimiters;
    };

    namespace test
    {
        inline void cli()
        {
            putils::CLI(
                    // Commands
                    {
                            {
                                    "test",
                                    [](const std::string &, const std::vector<putils::CLI::Param> &params)
                                    {
                                        std::cout << "Test successful" << std::endl;
                                        for (const auto &p : params)
                                            std::cout << "\t[" << p.value << "]" << std::endl;
                                    }
                            }
                    },
                    // Default
                    [](const std::string &cmd, const std::vector<putils::CLI::Param> &params)
                    {
                        std::string line = cmd;

                        for (const auto &p : params)
                        {
                            line += " ";
                            if (p.delimiters.first != '\0')
                            {
                                line.append(1, p.delimiters.first);
                                line += p.value;
                                line.append(1, p.delimiters.second);
                            }
                            else
                                line += p.value;
                        }

                        std::cout << "[" << line << "]" << std::endl;
                        system(line.c_str());
                    }

            ).run();
        }
    }
}