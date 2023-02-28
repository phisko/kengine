#pragma once

// entt
#include <entt/entity/fwd.hpp>

struct ImGuiContext;
namespace ImGui {
	ImGuiContext * GetCurrentContext();
	void SetCurrentContext(ImGuiContext* ctx);
}

namespace kengine::imgui {
	// Use this callback pattern so that the kengine_imgui DLL can set the context for the calling DLL
	using get_context_callback_type = decltype(ImGui::GetCurrentContext);
	using set_context_callback_type = decltype(ImGui::SetCurrentContext);
	KENGINE_IMGUI_EXPORT bool set_context(const entt::registry & r, get_context_callback_type * get_context_callback = ImGui::GetCurrentContext, set_context_callback_type * set_context_callback = ImGui::SetCurrentContext) noexcept;
}