// stl
#include <filesystem>
#include <iostream>

// kengine
#include "kengine/core/data/transform.hpp"
#include "kengine/core/log/standard_output/systems/system.hpp"
#include "kengine/main_loop/data/keep_alive.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/main_loop/helpers/run.hpp"
#include "kengine/scripting/lua/data/scripts.hpp"
#include "kengine/scripting/lua/helpers/register_types.hpp"
#include "kengine/scripting/lua/systems/system.cpp"

struct debug_system {
    debug_system(entt::handle e) noexcept
        : r(*e.registry())
    {
        e.emplace<kengine::main_loop::execute>([this](float delta_time) {
             execute(delta_time);
        });
    }

    void execute(float delta_time) noexcept {
        for (const auto & [e, transform, lua_scripts] : r.view<kengine::core::transform, kengine::scripting::lua::scripts>().each()) {
            std::cout << "Entity " << (int)e << std::endl;
            std::cout << "\tTransform: "
                << transform.bounding_box.position.x << ' '
                << transform.bounding_box.position.y << ' '
                << transform.bounding_box.position.z << std::endl;

            std::cout << "\tScripts:" << std::endl;
            for (const auto & script : lua_scripts.files)
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

    kengine::core::log::standard_output::add_system(r);
    kengine::scripting::lua::add_system(r);
    add_debug_system(r);

    const auto e = r.create();
    r.emplace<kengine::main_loop::keep_alive>(e);
    r.emplace<kengine::core::transform>(e).bounding_box.position = { 42.f, 0.f, 42.f };
    r.emplace<kengine::scripting::lua::scripts>(e).files = { "unit.lua" };

	// Register types to be used in lua
	kengine::scripting::lua::register_types<
		true, // components
		kengine::core::transform,
		kengine::scripting::lua::scripts
	>(r);

	kengine::scripting::lua::register_types<
		false, // non-components
		putils::rect3f,
		putils::point3f
	>(r);

    // Start "game"
    kengine::main_loop::run(r);

	r.clear(); // Explicitly clear so that component dtors are called before pools are invalidated
    return 0;
}