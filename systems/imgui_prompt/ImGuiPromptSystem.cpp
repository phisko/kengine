#include "ImGuiPromptSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "forward_to.hpp"
#include "reflection_helpers/imgui_helper.hpp"

// kengine data
#include "data/NameComponent.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "data/LuaStateComponent.hpp"
#include "data/PythonStateComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct ImGuiPromptSystem {
		entt::registry & r;
		bool * enabled;

		enum class Language {
			Lua,
			Python
		};

		Language selectedLanguage = Language::Lua;
		int maxLines = 128;
		char buff[1024];

		ImGuiPromptSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "ImGuiPromptSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));

			e.emplace<NameComponent>("Prompt");
			auto & tool = e.emplace<ImGuiToolComponent>();
			enabled = &tool.enabled;
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*enabled)
				return;

			kengine_log(r, Verbose, "Execute", "ImGuiPromptSystem");

			if (ImGui::Begin("Prompt", enabled)) {
				ImGui::Columns(2);
				drawHistory();
				ImGui::NextColumn();
				if (drawPrompt()) {
					history.addLine(buff, false, putils::NormalizedColor{ 0.f }); // cyan
					eval();
					buff[0] = 0;
					while (history.lines.size() > maxLines && !history.lines.empty())
						history.lines.pop_front();
				}
				ImGui::Columns();
			}
			ImGui::End();
		}

		bool shouldScrollDown = false;
		void drawHistory() noexcept {
			KENGINE_PROFILING_SCOPE;

			int tmp = maxLines;
			if (ImGui::InputInt("Max history", &tmp, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
				maxLines = tmp;

			ImGui::BeginChild("History");
			for (const auto & line : history.lines) {
				if (line.separator)
					ImGui::Separator();

				ImGui::PushTextWrapPos();
				ImGui::TextColored({ line.color.r, line.color.g, line.color.b, line.color.a }, "%s", line.text.c_str());
				ImGui::PopTextWrapPos();
				if (ImGui::IsItemClicked(1))
					ImGui::SetClipboardText(line.text.c_str());
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Right-click to copy to clipboard");
			}

			if (shouldScrollDown) {
				ImGui::SetScrollHereY();
				shouldScrollDown = false;
			}

			ImGui::EndChild();
		}

		bool firstDrawPrompt = true;
		bool drawPrompt() noexcept {
			KENGINE_PROFILING_SCOPE;

			if (putils::reflection::imguiEnumCombo("##Language", selectedLanguage) || firstDrawPrompt) {
				history.addLine(
					std::string(magic_enum::enum_names<Language>()[(int)selectedLanguage]),
					true,
					putils::NormalizedColor{ 1.f, 1.f, 0.f }
				);
				shouldScrollDown = true;
				firstDrawPrompt = false;
			}

			const bool ret = ImGui::InputTextMultiline("##Prompt", buff, putils::lengthof(buff), { -1.f, -1.f }, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AllowTabInput);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Ctrl+Enter to execute");

			if (ret) {
				shouldScrollDown = true;
				ImGui::SetKeyboardFocusHere(-1);
			}
			return ret;
		}

		void eval() noexcept {
			KENGINE_PROFILING_SCOPE;

			switch (selectedLanguage) {
			case Language::Lua:
				evalLua();
				break;
			case Language::Python:
				evalPython();
				break;
			default:
				static_assert(magic_enum::enum_count<Language>() == 2);
			}
		}

		bool active = false;
		bool firstEvalLua = true;
		void evalLua() noexcept {
			KENGINE_PROFILING_SCOPE;

#ifndef KENGINE_LUA
			kengine_log(Error, "Execute/ImGuiPromptSystem", "Attempt to evaluate Lua script but KENGINE_LUA is not defined");
			addLineToHistory(
				"Please compile with KENGINE_LUA",
				false,
				putils::NormalizedColor{ 1.f, 0.f, 0.f }
			);
#else
			kengine_logf(r, Log, "Execute/ImGuiPromptSystem", "Evaluating Lua script: '%s'", buff);

			for (const auto & [e, state] : r.view<kengine::LuaStateComponent>().each()) {
				if (firstEvalLua) {
					setupOutputRedirect(*state.state);
					firstEvalLua = false;
				}

				active = true;
				try {
					state.state->script(buff);
				}
				catch (const std::exception & e) {
					history.addError(e.what());
				}
				active = false;
			}
#endif
		}

#ifdef KENGINE_LUA
		void setupOutputRedirect(sol::state & state) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init/ImGuiPromptSystem", "Setting up output redirection for Lua");

			static ImGuiPromptSystem * g_this = nullptr;
			kengine_assert_with_message(r, !g_this, "ImGuiPromptSystem doesn't support existing in multiple registries currently. Fix this!");
			g_this = this;

			const luaL_Reg printlib[3] = {
				{ "print", [](lua_State * L) { return g_this->addToHistoryOrPrint(L, putils::NormalizedColor{}); } },
				{ "error", [](lua_State * L) { return g_this->addToHistoryOrPrint(L, putils::NormalizedColor{ 1.f, 0.f, 0.f }); } },
				{ nullptr, nullptr }
			};

			lua_getglobal(state, "_G");
			luaL_setfuncs(state, printlib, 0);
			lua_pop(state, 1);
		}

		int addToHistoryOrPrint(lua_State * L, const putils::NormalizedColor & color) noexcept {
			KENGINE_PROFILING_SCOPE;

			std::string line;

			// Stolen from luaB_print
			const int nargs = lua_gettop(L);
			lua_getglobal(L, "tostring");
			for (int i = 1; i <= nargs; i++) {
				lua_pushvalue(L, -1);  /* function to be called */
				lua_pushvalue(L, i);   /* value to print */
				lua_call(L, 1, 1);
				const char * s = lua_tolstring(L, -1, nullptr);  /* get result */
				if (s == nullptr)
					return luaL_error(L, "'tostring' must return a string to 'print'");
				if (i > 1)
					line += '\t';
				line += s;
				lua_pop(L, 1);  /* pop result */
			}

			if (active)
				history.addLine(std::move(line), false, color);
			else
				std::cout << line << std::endl;
			return 0;
		}
#endif

		void evalPython() {
			KENGINE_PROFILING_SCOPE;

#ifndef KENGINE_PYTHON
			kengine_log(Error, "Execute/ImGuiPromptSystem", "Attempt to evaluate Python script but KENGINE_PYTHON is not defined");
			addLineToHistory(
				"Please compile with KENGINE_PYTHON",
				false,
				putils::NormalizedColor{ 1.f, 0.f, 0.f }
			);
#else
			kengine_logf(r, Log, "Execute/ImGuiPromptSystem", "Evaluating Python script: '%s'", buff);

#ifdef __GNUC__
// Ignore "declared with greater visibility than the type of its field" warnings
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wattributes"
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
				_stdout_buffer = stringio();  // Other filelike object can be used here as well, such as objects created by pybind11
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
#	pragma GCC diagnostic pop
#endif

			PyStdErrOutStreamRedirect redirect;
			try {
				py::exec(buff);
			}
			catch (const std::exception & e) {
				history.addError(e.what());
			}

			auto output = redirect.stdoutString();
			if (!output.empty())
				history.addLine(std::move(output));

			auto err = redirect.stderrString();
			if (!err.empty())
				history.addError(std::move(err));
#endif
		}

		struct {
			struct Line {
				std::string text;
				bool separator = false;
				putils::NormalizedColor color;
			};

			std::list<Line> lines;

			template<typename S>
			void addLine(S && s, bool separator = false, const putils::NormalizedColor & color = putils::NormalizedColor{}) noexcept {
				lines.push_back({ FWD(s), separator, color });
			}

			template<typename S>
			void addError(S && s, bool separator = false) noexcept {
				addLine(FWD(s), separator, putils::NormalizedColor{ 1.f, 0.f, 0.f });
			}
		} history;
	};

	void addImGuiPromptSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<ImGuiPromptSystem>(e);
	}
}
