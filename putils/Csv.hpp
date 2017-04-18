#pragma once

#include <iostream>
#include <unordered_map>
#include <tuple>
#include <string>
#include <sstream>
#include "meta/for_each.hpp"

namespace putils
{
    // Implementation details
    namespace
    {
        template<typename T>
        void extract(T &attr, const std::string &val)
        {
            std::stringstream s(val);
            s >> attr;
        }

        template<>
        void extract(std::string &attr, const std::string &val)
        {
            attr = val;
        }
    }

    namespace
    {
        template<typename ...Fields>
        void parseFieldsImplem(const std::tuple<Fields...> &,
                               const std::tuple<std::function<void(Fields)>...> &,
                               std::index_sequence<>)
        {}

        template<typename ...Fields, size_t I, size_t ...Is>
        void parseFieldsImplem(const std::tuple<Fields...> &fields,
                               const std::tuple<std::function<void(Fields)>...> &actions,
                               std::index_sequence<I, Is...>)
        {
            std::get<I>(actions)(std::get<I>(fields));
            parseFieldsImplem(fields, actions, std::index_sequence<Is...>());
        };

        template<typename ...Fields>
        void parseFields(const std::tuple<Fields...> &fields,
                         const std::tuple<std::function<void(Fields)>...> &actions)
        {
            parseFieldsImplem(fields, actions, std::index_sequence_for<Fields...>());
        }
    }

    //
    // CSV Lexer
    // Template arguments are:
    //      the separator used in the CSV file
    //      the "key" type (first element of each row)
    //      the list of types following the key on each row
    //
    // For instance, for a file like the following:
    //      "1;this is a test;42.0"
    // You want to declare putils::Csv<';', int, std::string, double>
    //
    template<char Separator, typename Key, typename ...Fields>
    class Csv
    {
        // Constructors
    public:
        // Initialize from stream
        Csv(std::istream &s, bool ignoreFirstLine = false)
        {
            fillRows(s, ignoreFirstLine);
        }

        // Initialize from file
        Csv(const std::string &fileName, bool ignoreFirstLine = false)
        {
            std::ifstream s(fileName);
            fillRows(s, ignoreFirstLine);
        }

        Csv() {}

    public:
        void dump(std::ostream &s) const
        {
            for (auto &p : _orderedRows)
            {
                s << p->first;
                pmeta::tuple_for_each(p->second, [&s](auto &attr)
                {
                    s << Separator << attr;
                });
                s << std::endl;
            }
        }

        // Get the row for specified key
    public:
        const std::tuple<Fields...> &get(const Key &key) const { return _rows.at(key); }

        // Get all rows
    public:
        const std::vector<std::pair<const Key, std::tuple<Fields...>> *> &getRows() const { return _orderedRows; }

    public:
        void add(const Key &key, const std::tuple<Fields...> &t)
        {
            const auto &p = _rows.emplace(key, t);
            _orderedRows.emplace_back(&(*p.first));
        }

        void add(const Key &key, const Fields &...fields) { add(key, std::make_tuple(fields...)); }

    public:
        void addRow(std::string line)
        {
            const auto pos = line.find(Separator);
            std::string skey = line.substr(0, pos);
            Key key;
            extract(key, skey);

            line = line.substr(pos + 1);

            std::tuple<Fields...> row;
            pmeta::tuple_for_each(row, [&line](auto &attr)
            {
                const auto pos = line.find(Separator);
                std::string val = line.substr(0, pos);
                line = line.substr(pos + 1);

                extract(attr, val);
            });

            const auto &p = _rows.emplace(std::move(key), std::move(row));
            _orderedRows.emplace_back(&(*p.first));
        }

    public:
        void parse(
                const std::function<void(Key)> &init,
                const std::tuple<std::function<void(Fields)>...> &actions,
                const std::function<void()> &end) const
        {
            for (const auto &p : _orderedRows)
            {
                init(p->first);
                parseFields(p->second, actions);
                end();
            }
        }

        // Constructor implementation detail
    private:
        void fillRows(std::istream &s, bool ignoreFirstLine)
        {
            if (ignoreFirstLine)
            {
                std::string dummy;
                std::getline(s, dummy);
            }

            for (std::string line; std::getline(s, line);)
                addRow(putils::chop(line));
        }

    private:
        std::unordered_map<Key, std::tuple<Fields...>> _rows;
        std::vector<std::pair<const Key, std::tuple<Fields...>> *> _orderedRows;
    };

    namespace test
    {
        void csv()
        {
            putils::Csv<';', int, int, std::string, char> csv("test.csv");
            // putils::Csv<',', std::string, std::string> csv("B-PAV-242 PAR.csv");

            // Iterate through rows and output them
            for (auto &p : csv.getRows())
            {
                const auto &key = p->first;
                const auto &t = p->second;

                std::cout << "[" << key << "]" << "\t";
                // Iterate through fields in row
                pmeta::tuple_for_each(t, [](auto &attr)
                {
                    std::cout << "'" << attr << "'" << "\t";
                });
                std::cout << std::endl;
            }

            // Copy all rows to a new csv
            putils::Csv<',', int, int, std::string, char> output;
            for (auto &p : csv.getRows())
                output.add(p->first, p->second);

            output.add(21, 21, "BOB LE BRICOLEUR", 'x');
            output.dump(std::cout);

            // Parse the CSV with the following actions
            output.parse(
                    // Key action
                    [](auto &&key){ std::cout << "KEY: " << key << std::endl; },
                    // Fields actions
                    {
                            [](auto &&first) { std::cout << "FIRST: " << first << std::endl; },
                            [](auto &&second) { std::cout << "SECOND: " << second << std::endl; },
                            [](auto &&third) { std::cout << "THIRD: " << third << std::endl; }
                    },
                    // End of row action
                    []() { std::cout << "END" << std::endl; }
            );
        }
    }
}
