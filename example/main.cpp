// stl
#include <filesystem>
#include <iostream>

// kengine
#include "kengine/core/data/transform.hpp"
#include "kengine/log/stdout/systems/log_stdout.hpp"
#include "kengine/main_loop/data/keep_alive.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/main_loop/helpers/main_loop.hpp"
#include "kengine/model_instance/data/graphics.hpp"
#include "kengine/scripting/lua/data/lua.hpp"
#include "kengine/scripting/lua/helpers/lua_helper.hpp"
#include "kengine/scripting/lua/systems/lua.cpp"

struct debug_system {
    debug_system(entt::handle e) noexcept
        : r(*e.registry())
    {
        e.emplace<kengine::functions::execute>([this](float delta_time) {
             execute(delta_time);
        });
    }

    void execute(float delta_time) noexcept {
        for (const auto & [e, transform, lua] : r.view<kengine::data::transform, kengine::data::lua>().each()) {
            std::cout << "Entity " << (int)e << std::endl;
            std::cout << "\tTransform: "
                << transform.bounding_box.position.x << ' '
                << transform.bounding_box.position.y << ' '
                << transform.bounding_box.position.z << std::endl;

            std::cout << "\tScripts:" << std::endl;
            for (const auto & script : lua.scripts)
                std::cout << "\t\t[" << script << "]" << std::endl;

            std::cout << std::endl;
        }
    }

    entt::registry & r;
};

void add_debug_system(entt::registry & r) noexcept {
    const entt::handle e(r, r.create());
    e.emplace<debug_system>(e);
}

int main(int, char **av) {
    // Go to executable directory to be near "resources"
    const auto bin_dir = std::filesystem::path(av[0]).parent_path();
    if (exists(bin_dir))
        std::filesystem::current_path(bin_dir);

    entt::registry r;

    kengine::systems::add_log_stdout(r);
    kengine::systems::add_lua(r);
    add_debug_system(r);

    const auto e = r.create();
    r.emplace<kengine::data::keep_alive>(e);
    r.emplace<kengine::data::transform>(e).bounding_box.position = { 42.f, 0.f, 42.f };
    r.emplace<kengine::data::lua>(e).scripts = { "unit.lua" };

	// Register types to be used in lua
	kengine::lua_helper::register_types<
		true, // components
		kengine::data::transform,
		kengine::data::lua
	>(r);

	kengine::lua_helper::register_types<
		false, // non-components
		putils::rect3f,
		putils::point3f
	>(r);

    // Start "game"
    kengine::main_loop::run(r);

	r.clear(); // Explicitly clear so that component dtors are called before pools are invalidated
    return 0;
}