#pragma once

#include <sstream>
#include <iostream>
#include <map>

#include "chop.hpp"

namespace putils
{
    namespace json
    {
        std::string prettyprint(auto &&str);

        struct Object
        {
            enum class Type
            {
                Value,
                Object,
                Array
            };
            std::string value;
            std::vector<Object> items;
            std::map<std::string, Object> fields;
            Type type;
            // if items.size() > 0, then I'm an array
            // if fields.size() == 0, then I'm a simple value
        };

        Object lex(auto &&str);
    }
}

namespace putils
{
    namespace json
    {
        namespace detail
        {
            void printLineAndSkipSpace(std::istringstream &s, auto &ret, std::size_t indent)
            {
                ret.append(1, '\n');
                ret.append(indent, '\t');
                while (s && std::isspace(s.peek()))
                    s.get();
            }
        }

        std::string prettyPrint(auto &&str)
        {
            static std::string startBlock = "{[";
            static std::string endBlock = "}]";
            static std::string newLine = ",";
            static std::string needSpace = ":";

            std::istringstream s(FWD(str));

            std::string ret;
            std::size_t indent = 0;

            while (s)
            {
                const char c = s.get();
                if (c == -1)
                    break;

                if (c == '\\')
                {
                    ret.append(1, s.get());
                    continue;
                }

                if (endBlock.find(c) != std::string::npos)
                {
                    --indent;
                    detail::printLineAndSkipSpace(s, ret, indent);
                }

                if (c != '\n')
                    ret.append(1, c);
                if (needSpace.find(c) != std::string::npos)
                {
                    ret.append(1, ' ');
                    while (s && std::isspace(s.peek()))
                        s.get();
                }

                if (newLine.find(c) != std::string::npos)
                    detail::printLineAndSkipSpace(s, ret, indent);

                if (startBlock.find(c) != std::string::npos)
                {
                    ++indent;
                    detail::printLineAndSkipSpace(s, ret, indent);
                }
            }

            return ret;
        }

        namespace
        {
            std::string lexValue(std::istringstream &s);
            Object lexArray(std::istringstream &s);
            Object lexObject(std::istringstream &s);

            void readKeyValue(std::istringstream &s, Object &ret)
            {
                while (std::isspace(s.peek()))
                    s.get();

                std::string key = lexValue(s);

                s.get(); // Skip ':'

                while (std::isspace(s.peek()))
                    s.get();

                if (s.peek() == '{')
                    ret.fields[key] = lexObject(s);
                else if (s.peek() == '[')
                    ret.fields[key] = lexArray(s);
                else
                    ret.fields[key] = Object{ lexValue(s), {}, {}, Object::Type::Value };
            }

            std::string lexValue(std::istringstream &s)
            {
                std::string ret;

                while (s)
                {
                    const char c = s.peek();
                    if (c == -1)
                        throw std::runtime_error("Unexpected EOF");

                    if (c == '\\')
                    {
                        s.get();
                        ret.append(1, s.get());
                        continue;
                    }

                    if (c == ':' || c == ',' || c == '}' || c == ']')
                        break;
                    ret.append(1, s.get());
                }

                return putils::chop(ret);
            }

            Object lexObject(std::istringstream &s)
            {
                Object ret;

                while (std::isspace(s.peek()))
                    s.get();

                if (s.peek() != '{')
                {
                    ret.type = Object::Type::Value;
                    s.get();
                    ret.value = lexValue(s);
                    return ret;
                }

                ret.type = Object::Type::Object;
                s.get(); // Skip '{'
                while (s)
                {
                    while (std::isspace(s.peek()))
                        s.get();

                    const char c = s.peek();
                    if (c == -1)
                        throw std::runtime_error("Unexpected EOF");

                    if (c == '}')
                    {
                        s.get();
                        break;
                    }

                    readKeyValue(s, ret);

                    while (std::isspace(s.peek()))
                        s.get();

                    if (s.peek() == ',')
                        s.get();
                }

                return ret;
            }

            Object lexArray(std::istringstream &s)
            {
                Object ret;
                ret.type = Object::Type::Array;

                s.get(); // Skip '['

                while (s)
                {
                    while (std::isspace(s.peek()))
                        s.get();

                    const char c = s.peek();
                    if (c == -1)
                        throw std::runtime_error("Unexpected EOF");
                    if (c == ']')
                    {
                        s.get();
                        break;
                    }

                    if (c == '{')
                        ret.items.push_back(lexObject(s));
                    else if (c == '[')
                        ret.items.push_back(lexArray(s));
                    else
                        ret.items.push_back(Object{ lexValue(s), {}, {}, Object::Type::Value });

                    while (std::isspace(s.peek()))
                        s.get();

                    if (s.peek() == ',')
                        s.get();
                }

                return ret;
            }
        }

        Object lex(auto &&str)
        {
            std::istringstream s(FWD(str));

            while (std::isspace(s.peek()))
                s.get();

            if (s.peek() == '{')
                return lexObject(s);
            if (s.peek() == '[')
                return lexArray(s);
            throw std::runtime_error(concat("Unexpected character " + s.peek()));
        }
    }

#include <assert.h>

    namespace test
    {
        namespace json
        {
            inline void prettyPrint()
            {
                std::cout <<
                          putils::json::prettyPrint("{ obj: { key: value }, list: [ first: { key: value }, second: { key: value } ] }")
                          << std::endl;
            }

            inline void lex()
            {
                putils::json::Object o = putils::json::lex(
                        putils::concat(
                                "{ obj: { key: value }, ",
                                "objectList: ",
                                "[ ",
                                "{ first: { key: value } }"
                                "{ second: { key: value } }",
                                " ], ",
                                "valueList: [ first, second ]",
                                " }"
                        )
                );
                assert(o.type == putils::json::Object::Type::Object);
                assert(o.fields.size() == 3);

                auto &obj = o.fields["obj"];
                assert(obj.type == putils::json::Object::Type::Object);
                assert(obj.fields.size() == 1);
                assert(obj.fields["key"].type == putils::json::Object::Type::Value);
                assert(obj.fields["key"].value == "value");

                auto &objectList = o.fields["objectList"];
                assert(objectList.type == putils::json::Object::Type::Array);
                assert(objectList.items.size() == 2);
                auto &object0 = objectList.items[0];
                assert(object0.type == putils::json::Object::Type::Object);
                assert(object0.fields.size() == 1);
                auto &first = object0.fields["first"];
                assert(first.type == putils::json::Object::Type::Object);
                assert(first.fields.size() == 1);
                assert(first.fields["key"].type == putils::json::Object::Type::Value);
                assert(first.fields["key"].value == "value");
                auto &object1 = objectList.items[1];
                assert(object1.type == putils::json::Object::Type::Object);
                assert(object1.fields.size() == 1);
                auto &second = object1.fields["second"];
                assert(second.type == putils::json::Object::Type::Object);
                assert(second.fields["key"].type == putils::json::Object::Type::Value);
                assert(second.fields["key"].value == "value");

                auto &valueList = o.fields["valueList"];
                assert(valueList.type == putils::json::Object::Type::Array);
                assert(valueList.items.size() == 2);
                auto &firstValue = valueList.items[0];
                assert(firstValue.type == putils::json::Object::Type::Value);
                assert(firstValue.value == "first");
                auto &secondValue = valueList.items[1];
                assert(secondValue.type == putils::json::Object::Type::Value);
                assert(secondValue.value == "second");
            }
        }
    }
}