#include "system.hpp"

// stl
#include <list>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"
#include "putils/reflection_helpers/imgui_helper.hpp"

// kengine
#include "kengine/core/data/name.hpp"
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/imgui/helpers/set_context.hpp"
#include "kengine/imgui/tool/data/tool.hpp"
#include "kengine/main_loop/functions/execute.hpp"

#ifdef KENGINE_SCRIPTING_LUA
#include "kengine/scripting/lua/data/state.hpp"
#endif

#ifdef KENGINE_SCRIPTING_PYTHON
#include "kengine/scripting/python/data/state.hpp"
#endif

namespace kengine::scripting::imgui_prompt {
	static constexpr auto log_category = "scripting_imgui_prompt";

	struct system {
		entt::registry & r;
		bool * enabled;

		enum class language {
			lua,
			python
		};

		language selected_language = language::lua;
		int max_lines = 128;
		char buff[1024] = "";

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			e.emplace<core::name>("Prompt");
			auto & tool = e.emplace<imgui::tool::tool>();
			enabled = &tool.enabled;
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			if (!*enabled) {
				kengine_log(r, very_verbose, log_category, "Disabled");
				return;
			}

			if (!kengine::imgui::set_context(r))
				return;

			if (ImGui::Begin("Prompt", enabled)) {
				ImGui::Columns(2);
				draw_history();
				ImGui::NextColumn();
				if (draw_prompt()) {
					history.add_line(buff, false, putils::normalized_color{ 0.f }); // cyan
					eval();
					buff[0] = 0;
					while (history.lines.size() > max_lines && !history.lines.empty())
						history.lines.pop_front();
				}
				ImGui::Columns();
			}
			ImGui::End();
		}

		bool should_scroll_down = false;
		void draw_history() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Drawing history");

			int tmp = max_lines;
			if (ImGui::InputInt("Max history", &tmp, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
				kengine_logf(r, verbose, log_category, "Max history changed to {}", tmp);
				max_lines = tmp;
			}

			ImGui::BeginChild("History");
			for (const auto & line : history.lines) {
				if (line.separator)
					ImGui::Separator();

				ImGui::PushTextWrapPos();
				ImGui::TextColored({ line.color.r, line.color.g, line.color.b, line.color.a }, "%s", line.text.c_str());
				ImGui::PopTextWrapPos();
				if (ImGui::IsItemClicked(1)) {
					kengine_logf(r, log, log_category, "Setting clipboard text to '{}'", line.text);
					ImGui::SetClipboardText(line.text.c_str());
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Right-click to copy to clipboard");
			}

			if (should_scroll_down) {
				kengine_log(r, log, log_category, "Scrolling down");
				ImGui::SetScrollHereY();
				should_scroll_down = false;
			}

			ImGui::EndChild();
		}

		bool first_draw_prompt = true;
		bool draw_prompt() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Drawing prompt");

			if (putils::reflection::imgui_enum_combo("##language", selected_language) || first_draw_prompt) {
				std::string new_language(magic_enum::enum_names<language>()[(int)selected_language]);

				kengine_logf(r, log, log_category, "Changed language to {}", new_language);
				history.add_line(
					std::move(new_language),
					true,
					putils::normalized_color{ 1.f, 1.f, 0.f }
				);
				should_scroll_down = true;
				first_draw_prompt = false;
			}

			const bool ret = ImGui::InputTextMultiline("##Prompt", buff, putils::lengthof(buff), { -1.f, -1.f }, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AllowTabInput);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Ctrl+Enter to execute");

			if (ret) {
				kengine_log(r, log, log_category, "User confirmed prompt");
				should_scroll_down = true;
				ImGui::SetKeyboardFocusHere(-1);
			}
			return ret;
		}

		void eval() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, log, log_category, "Evaluating '{}", buff);

			switch (selected_language) {
				case language::lua:
					eval_lua();
					break;
				case language::python:
					eval_python();
					break;
				default:
					static_assert(magic_enum::enum_count<language>() == 2);
			}
		}

		bool active = false;
		bool first_eval_lua = true;
		void eval_lua() noexcept {
			KENGINE_PROFILING_SCOPE;

#ifndef KENGINE_SCRIPTING_LUA
			kengine_log(r, error, log_category, "Attempt to evaluate Lua script but KENGINE_SCRIPTING_LUA is not defined");
			history.add_line(
				"Please compile with KENGINE_SCRIPTING_LUA",
				false,
				putils::normalized_color{ 1.f, 0.f, 0.f }
			);
#else
			kengine_logf(r, log, log_category, "Evaluating Lua script: '{}'", buff);

			for (const auto & [e, state] : r.view<scripting::lua::state>().each()) {
				if (first_eval_lua) {
					setup_output_redirect(*state.ptr);
					first_eval_lua = false;
				}

				active = true;
				try {
					kengine_logf(r, verbose, log_category, "Evaluating with state {}", e);
					state.ptr->script(buff);
				}
				catch (const std::exception & e) {
					kengine_logf(r, error, log_category, "Error: {}", e.what());
					history.add_error(e.what());
				}
				active = false;
			}
#endif
		}

#ifdef KENGINE_SCRIPTING_LUA
		void setup_output_redirect(sol::state & state) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Setting up output redirection for Lua");

			static system * g_this = nullptr;
			kengine_assert_with_message(r, !g_this, "imgui_prompt system doesn't support existing in multiple registries currently. Fix this!");
			g_this = this;

			const luaL_Reg printlib[3] = {
				{ "print", [](lua_State * L) { return g_this->add_to_history_or_print(L, putils::normalized_color{}); } },
				{ "error", [](lua_State * L) { return g_this->add_to_history_or_print(L, putils::normalized_color{ 1.f, 0.f, 0.f }); } },
				{ nullptr, nullptr }
			};

			lua_getglobal(state, "_G");
			luaL_setfuncs(state, printlib, 0);
			lua_pop(state, 1);
		}

		int add_to_history_or_print(lua_State * L, const putils::normalized_color & color) noexcept {
			KENGINE_PROFILING_SCOPE;

			std::string line;

			// Stolen from luaB_print
			int n = lua_gettop(L);  /* number of arguments */
			int i;
			for (i = 1; i <= n; i++) {  /* for each argument */
				size_t l;
				const char * s = luaL_tolstring(L, i, &l);  /* convert it to string */
				if (i > 1)  /* not the first element? */
					line += '\t';  /* add a tab before it */
				line += s;  /* print it */
				lua_pop(L, 1);  /* pop result */
			}

			if (active)
				history.add_line(std::move(line), false, color);
			else
				std::cout << line << std::endl;
			return 0;
		}
#endif

		void eval_python() {
			KENGINE_PROFILING_SCOPE;

#ifndef KENGINE_SCRIPTING_PYTHON
			kengine_log(r, error, log_category, "Attempt to evaluate python script but KENGINE_SCRIPTING_PYTHON is not defined");
			history.add_line(
				"Please compile with KENGINE_SCRIPTING_PYTHON",
				false,
				putils::normalized_color{ 1.f, 0.f, 0.f }
			);
#else
			kengine_logf(r, log, log_category, "Evaluating python script: '{}'", buff);

#ifdef __GNUC__
// Ignore "declared with greater visibility than the type of its field" warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif
			// Stolen from https://github.com/pybind/pybind11/issues/1622
			class PyStdErrOutStreamRedirect {
				py::object _stdout;
				py::object _stderr;
				py::object _stdout_buffer;
				py::object _stderr_buffer;

			public:
				PyStdErrOutStreamRedirect() {
					auto sysm = py::module_::import("sys");
					_stdout = sysm.attr("stdout");
					_stderr = sysm.attr("stderr");
					auto stringio = py::module_::import("io").attr("StringIO");
					_stdout_buffer = stringio(); // Other filelike object can be used here as well, such as objects created by pybind11
					_stderr_buffer = stringio();
					sysm.attr("stdout") = _stdout_buffer;
					sysm.attr("stderr") = _stderr_buffer;
				}
				std::string stdoutString() {
					_stdout_buffer.attr("seek")(0);
					return py::str(_stdout_buffer.attr("read")());
				}
				std::string stderrString() {
					_stderr_buffer.attr("seek")(0);
					return py::str(_stderr_buffer.attr("read")());
				}
				~PyStdErrOutStreamRedirect() {
					auto sysm = py::module_::import("sys");
					sysm.attr("stdout") = _stdout;
					sysm.attr("stderr") = _stderr;
				}
			};
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

			PyStdErrOutStreamRedirect redirect;
			try {
				py::exec(buff);
			}
			catch (const std::exception & e) {
				kengine_logf(r, error, log_category, "Error: {}", e.what());
				history.add_error(e.what());
			}

			auto output = redirect.stdoutString();
			if (!output.empty())
				history.add_line(std::move(output));

			auto err = redirect.stderrString();
			if (!err.empty())
				history.add_error(std::move(err));
#endif
		}

		struct {
			struct line {
				std::string text;
				bool separator = false;
				putils::normalized_color color;
			};

			std::list<line> lines;

			template<typename S>
			void add_line(S && s, bool separator = false, const putils::normalized_color & color = putils::normalized_color{}) noexcept {
				lines.push_back({ FWD(s), separator, color });
			}

			template<typename S>
			void add_error(S && s, bool separator = false) noexcept {
				add_line(FWD(s), separator, putils::normalized_color{ 1.f, 0.f, 0.f });
			}
		} history;
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}
