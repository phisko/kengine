#pragma once

#include <cstddef>
#include <unordered_map>
#include "BaseModule.hpp"
#include "for_each.hpp"
#include "static_if.hpp"

namespace putils
{
    namespace
    {
        struct ExampleQuery
        {
            BaseModule *sender { nullptr };
        };
    }

    template<typename CRTP, typename ...DataPackets>
    class Module : public virtual BaseModule
    {
    public:
        Module(Mediator *m = nullptr) : BaseModule(m)
        {
            static_assert(std::is_base_of<Module, CRTP>::value, "Module's first template parameter should be its inheriting class");

            pmeta::tuple_for_each(std::tuple<pmeta::type<DataPackets>...>(),
                    [this](auto &&type)
                    {
                        this->setHandler<typename std::decay_t<decltype(type)>::wrapped>([this](const auto &p)
                                { static_cast<CRTP*>(this)->handle(p); }
                        );
                    });
        }

    private:
        template<typename Data>
        void setHandler(const std::function<void(const Data &)> &handler)
        {
            _handlers[pmeta::type<Data>::index] = [this, handler](const ADataPacket &packet)
            {
                handler(static_cast<const putils::DataPacket<Data> &>(packet).data);
            };
        }

    protected:
        template<typename Response>
        Response query(auto &&q) // q: DataPacket containing a "putils::BaseModule *sender" field
        {
            return Handler<Response>(getMediator(), FWD(q)).res;
        }

    private:
        template<typename Response>
        struct Handler : Module<Handler<Response>, Response>
        {
            template<typename Q>
            Handler(Mediator *m, Q &&query) : BaseModule(m)
            {
                query.sender = this;
                this->send(FWD(query));
            }

            void handle(const Response &response) { res = response; }

            Response res;
        };

    private:
        const std::vector<pmeta::type_index> &getSubscriptions() const noexcept final { return _subscriptions; }
        void receive(const putils::ADataPacket &packet) noexcept final
        {
            const auto it = _handlers.find(packet.type);
            if (it != _handlers.end())
                it->second(packet);
        }

    private:
        std::unordered_map<pmeta::type_index, std::function<void(const putils::ADataPacket &)>> _handlers;

    private:
        static const std::vector<pmeta::type_index> _subscriptions;
    };
}

template<typename CRTP, typename ...DataPackets>
const std::vector<pmeta::type_index> putils::Module<CRTP, DataPackets...>::_subscriptions = { pmeta::type<DataPackets>::index... };
