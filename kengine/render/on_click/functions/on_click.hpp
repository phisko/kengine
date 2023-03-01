#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::render::on_click {
	using on_click_signature = void(int button);
	//! putils reflect all
	//! parents: [refltype::base]
	struct on_click : base_function<on_click_signature> {};
}

#include "on_click.rpp"