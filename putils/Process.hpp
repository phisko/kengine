#pragma once

#include <cstdlib>
#include <fstream>

#include "concat.hpp"

#include "read_stream.hpp"

namespace putils
{
    class Process
    {
    public:
        struct Options
        {
            struct Stream
            {
                bool redirected;
                std::string replacement;
            };

            Stream stdin { false };
            Stream stdout { false, ".stdout" };
            Stream stderr { false, ".stderr" };
        };

    public:
        Process(const std::string &command, const Options &options = Options{ { false, "" }, { false, "" }, { false, "" }})
                : _options(options)
        {
            std::string run(command);

            if (options.stdin.redirected)
                run = concat("echo ", options.stdin.replacement, " | ", run);
            if (options.stdout.redirected)
                run = putils::concat(run, " 1> ", options.stdout.replacement);
            if (options.stderr.redirected)
                run = putils::concat(run, " 2> ", options.stderr.replacement);

            system(run.c_str());

            if (options.stdout.redirected)
                _stdout.open(options.stdout.replacement.c_str());
            if (options.stderr.redirected)
                _stderr.open(options.stderr.replacement.c_str());
        }

        ~Process()
        {
            if (_stdout.is_open())
            {
                _stdout.close();
                system(concat("rm ", _options.stdout.replacement).c_str());
            }

            if (_stderr.is_open())
            {
                _stderr.close();
                system(concat("rm ", _options.stderr.replacement).c_str());
            }
        }

        std::istream &getStdout() { return _stdout; }
        std::istream &getStderr() { return _stderr; }

    private:
        Options _options;
        std::ifstream _stdout;
        std::ifstream _stderr;
    };

    namespace test
    {
        inline void process()
        {
            putils::Process::Options options;
            options.stdout.redirected = true;
            options.stderr.redirected = true;

            {
                putils::Process p("ls", options);

                std::cout << "GOOD:" << std::endl;
                std::cout << "stdout: [" << putils::read_stream(p.getStdout()) << "]" << std::endl;
                std::cout << "stderr: [" << putils::read_stream(p.getStderr()) << "]" << std::endl;
            }

            std::cout << std::endl;

            {
                putils::Process p("ls \"BAD FILE\"", options);

                std::cout << "BAD:" << std::endl;
                std::cout << "stdout: [" << putils::read_stream(p.getStdout()) << "]" << std::endl;
                std::cout << "stderr: [" << putils::read_stream(p.getStderr()) << "]" << std::endl;
            }

            std::cout << std::endl;

            {
                options.stdin.redirected = true;
                options.stdin.replacement = "this should be printed by cat";
                putils::Process p("cat", options);

                std::cout << "INPUT:" << std::endl;
                std::cout << "stdout: [" << putils::read_stream(p.getStdout()) << "]" << std::endl;
                std::cout << "stderr: [" << putils::read_stream(p.getStderr()) << "]" << std::endl;
            }
        }
    }
}