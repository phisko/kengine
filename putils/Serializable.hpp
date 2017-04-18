#pragma once

#include <iostream>
#include <memory>
#include "meta/for_each.hpp"
#include "chop.hpp"

namespace putils
{
    namespace OutputPolicies
    {
        struct Json
        {
            static void startSerialize(std::ostream &s) { s << '{'; }
            static void endSerialize(std::ostream &s) { s << '}'; }
            static void serializeNewField(std::ostream &s) { s << ", "; }

            static void startUnserialize(std::istream &s) { s.get(); }
            static void endUnserialize(std::istream &s) { while (std::isspace(s.get())); s.get(); }
            static void unserializeNewField(std::istream &s) { while (std::isspace(s.get())); s.get(); }

            template<typename T>
            static void serialize(std::ostream &s, const std::string &name, const std::unique_ptr<T> &)
            { s << name << ": \"\""; }

            template<typename T>
            static void unserialize(std::istream &s, const std::string &name, std::unique_ptr<T> &)
            {
                while (std::isspace(s.peek()))
                    s.get();
                while (s.peek() != ',')
                    if (s.get() == '\\')
                        s.get();
            }

            template<typename T>
            static void serialize(std::ostream &s, const std::string &name, const std::shared_ptr<T> &)
            { s << name << ": \"\""; }

            template<typename T>
            static void unserialize(std::istream &s, const std::string &name, std::shared_ptr<T> &)
            {
                while (std::isspace(s.peek()))
                    s.get();
                while (s.peek() != ',')
                    if (s.get() == '\\')
                        s.get();
            }

            template<typename T, typename = std::enable_if_t<!std::is_enum<T>::value>>
            static void serialize(std::ostream &s, const std::string &name, const T &attr)
            {
                s << name << ": " << attr;
            }

            template<typename T, typename = std::enable_if_t<!std::is_enum<T>::value>>
            static void unserialize(std::istream &s, const std::string &name, T &attr)
            {
                while (std::isspace(s.peek()))
                    s.get();
                while (s.get() != ':');
                while (std::isspace(s.peek()))
                    s.get();

                std::string value;
                while (s.peek() != ',' && s.peek() != '}')
                {
                    const char c = s.get();
                    if (c == '\\')
                        value.append(1, s.get());
                    else
                        value.append(1, c);
                }

                std::stringstream(putils::chop(value)) >> attr;
            }

            template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
            static void serialize(std::ostream &s, const std::string &name, T attr)
            { s << name << ": " << (int)attr; };

            template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
            static void unserialize(std::istream &s, const std::string &name, const T &attr)
            {
                while (std::isspace(s.peek()))
                    s.get();
                while (s.get() != ':');

                std::string value;
                while (s.peek() != ',')
                {
                    const char c = s.get();
                    if (c == '\\')
                        value.append(1, s.get());
                    else
                        value.append(1, c);
                }

                std::stringstream(putils::chop(value)) >> (int&)attr;
            }
        };

        struct Default
        {
            static void startSerialize(std::ostream &s) {}
            static void endSerialize(std::ostream &s) {}
            static void serializeNewField(std::ostream &s) {}

            static void startUnserialize(std::istream &s) {}
            static void endUnserialize(std::istream &s) {}
            static void unserializeNewField(std::istream &s) {}

            template<typename T>
            static void serialize(std::ostream &, const std::string &, const std::unique_ptr<T> &) {}

            template<typename T>
            static void unserialize(std::istream &, const std::string &, std::unique_ptr<T> &) {}

            template<typename T>
            static void serialize(std::ostream &, const std::string &, const std::shared_ptr<T> &) {}

            template<typename T>
            static void unserialize(std::istream &, const std::string &, std::shared_ptr<T> &) {}

            template<typename T, typename = std::enable_if_t<!std::is_enum<T>::value>>
            static void serialize(std::ostream &s, const std::string &name, const T &attr) { s << attr << " "; }

            template<typename T, typename = std::enable_if_t<!std::is_enum<T>::value>>
            static void unserialize(std::istream &s, const std::string &name, T &attr) { s >> attr; }

            template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
            static void serialize(std::ostream &s, const std::string &name, T attr) { s << (int)attr << " "; };

            template<typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
            static void unserialize(std::istream &s, const std::string &name, const T &attr) { s >> (int&)attr; }
        };

        struct Binary
        {
            static void startSerialize(std::ostream &s) {}
            static void endSerialize(std::ostream &s) {}
            static void serializeNewField(std::ostream &s) {}

            static void startUnserialize(std::istream &s) {}
            static void endUnserialize(std::istream &s) {}
            static void unserializeNewField(std::istream &s) {}

            template<typename T>
            static void serialize(std::ostream &, const std::string &, const std::unique_ptr<T> &) {}

            template<typename T>
            static void unserialize(std::istream &, const std::string &, std::unique_ptr<T> &) {}

            template<typename T>
            static void serialize(std::ostream &, const std::string &, const std::shared_ptr<T> &) {}

            template<typename T>
            static void unserialize(std::istream &, const std::string &, std::shared_ptr<T> &) {}

            template<typename T, typename = std::enable_if_t<!std::is_enum<T>::value>>
            static void serialize(std::ostream &s, const std::string &name, const T &attr) { s.write(&attr, sizeof(attr)); }

            template<typename T, typename = std::enable_if_t<!std::is_enum<T>::value>>
            static void unserialize(std::istream &s, const std::string &name, T &attr) { s.read(&attr, sizeof(attr)); }
        };

        struct None
        {
            static void startSerialize(std::ostream &s) {}
            static void endSerialize(std::ostream &s) {}
            static void serializeNewField(std::ostream &s) {}
            static void startUnserialize(std::istream &s) {}
            static void endUnserialize(std::istream &s) {}
            static void unserializeNewField(std::istream &s) {}
            static void serialize(std::ostream &, const std::string &, const auto &) {}
            static void unserialize(std::istream &, const std::string &, auto &) {}
        };
    }

    class SerializableBase
    {
        // Destructor
    public:
        virtual ~SerializableBase() {}

        // Serialize this object
    public:
        virtual void serialize(std::ostream &s) const noexcept = 0;
        virtual void unserialize(std::istream &s) noexcept = 0;
    };


    // OutputPolicy: type ressembling DefaultOutputPolicy (above), with a
    // template<T> static int serialize(std::ostream &s, const T &attr) function that will be called to
    // serialize each attribute
    template<typename Derived, typename OutputPolicy = OutputPolicies::Json>
    class Serializable : public SerializableBase
    {
        // Serialization implementation detail: pointer to generic tuple which will be serialized
        // Tuple should be given all the attributes to serialize
    private:
        // Base interface for Serializer template
        struct SerializerBase
        {
            virtual ~SerializerBase() {}

            virtual void serialize(const Derived *, std::ostream &s) const noexcept = 0;
            virtual void unserialize(Derived *, std::istream &s) noexcept = 0;
        };

        template<typename ...Attrs>
        struct Serializer : SerializerBase
        {
            Serializer(Attrs &&...attrs)
            {
                if (_attrs == nullptr)
                    _attrs = std::make_unique<std::tuple<Attrs...>>(attrs...);
            }

            // For each member pointer in _attrs, serialize it by calling serializeOne
            void serialize(const Derived *obj, std::ostream &s) const noexcept override
            {
                OutputPolicy::startSerialize(s);

                bool first = true;
                pmeta::tuple_for_each(*_attrs, [&s, obj, &first](const auto &attr)
                {
                    if (!first)
                        OutputPolicy::serializeNewField(s);
                    OutputPolicy::serialize(s, attr.first, obj->*(attr.second));
                    first = false;
                });

                OutputPolicy::endSerialize(s);
            }

            void unserialize(Derived *obj, std::istream &s) noexcept override
            {
                pmeta::tuple_for_each(*_attrs, [&s, obj](const auto &attr)
                {
                    OutputPolicy::unserialize(s, attr.first, obj->*(attr.second));
                });
            }

            // Static tuple containing the member pointers to be serialized for this class (Derived)
        private:
            static std::unique_ptr<std::tuple<Attrs ...>> _attrs;
        };

        // Constructor
    public:
        template<typename ...Fields> // MemberPointers: std::pair<std::string, Derived::*attr>
        Serializable(Fields &&...attrs)
                : SerializableBase(), _serializer(new Serializer<Fields...>(FWD(attrs)...)) {}

    public:
        void serialize(std::ostream &s) const noexcept override
        {
            _serializer->serialize(static_cast<const Derived *>(this), s);
        }

        void unserialize(std::istream &s) noexcept override
        {
            _serializer->unserialize(static_cast<Derived *>(this), s);
        }

        std::shared_ptr<SerializerBase> _serializer;
    };
}

template<typename Derived, typename OutputPolicy>
template<typename ...Attrs>
std::unique_ptr<std::tuple<Attrs...>>    putils::Serializable<Derived, OutputPolicy>::Serializer<Attrs...>::_attrs = nullptr;

template<typename Derived, typename OutputPolicy>
std::ostream &operator<<(std::ostream &s, const putils::Serializable<Derived, OutputPolicy> &obj)
{
    obj.serialize(s);
    return s;
}

template<typename Derived, typename OutputPolicy>
std::istream &operator>>(std::istream &s, putils::Serializable<Derived, OutputPolicy> &obj)
{
    obj.unserialize(s);
    return s;
}

namespace putils
{
    namespace test
    {
        void serializable()
        {
            struct Tmp : public Serializable<Tmp>
            {
                Tmp(int x, int y)
                        :
                        Serializable(
                                std::make_pair("x", &Tmp::_x),
                                std::make_pair("y", &Tmp::_y)
                        ), _x(x), _y(y)
                {}
                int _x, _y;
            };

            Tmp test(24, 42);
            std::stringstream s;

            test.serialize(std::cout); std::cout << std::endl;
            test.serialize(s);

            test._x = 5; test._y = 5;

            test.serialize(std::cout); std::cout << std::endl;
            test.unserialize(s);
            test.serialize(std::cout); std::cout << std::endl;
        }
    }
}
