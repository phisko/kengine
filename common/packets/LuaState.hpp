#pragma once

namespace putils { class BaseModule; }
namespace sol { class state; }

namespace kengine {
    namespace packets {
        namespace LuaState {
            struct Query {
                putils::BaseModule * sender = nullptr;
            };

            struct Response {
                sol::state * state = nullptr;
            };
        }
    }
}