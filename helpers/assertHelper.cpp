#ifndef KENGINE_NDEBUG

#include <unordered_set>

#include "AssertHelper.hpp"
#include "kengine.hpp"

#include "data/TimeModulatorComponent.hpp"
#include "functions/Execute.hpp"
#include "imgui.h"
#include "lengthof.hpp"
#include "string.hpp"
#include "get_call_stack.hpp"

struct AssertInfo {
	const char * file;
	int line;
	std::string expr;
	std::string stack;
};

using File = std::unordered_set<int>; // Set of ignored lines
using AssertMap = std::unordered_map<const char *, File>; // Map of files
static AssertMap g_assertMap;

namespace kengine::assertHelper {
	bool assertFailed(const char * file, int line, const std::string & expr) noexcept {
		struct impl {
			static bool assertFailed(const char * file, int line, const std::string & expr) noexcept {
				if (isIgnored(file, line))
					return false;

				entities += [file, line, expr](Entity & e) {
					auto & info = e.attach<AssertInfo>();
					info.file = file;
					info.line = line;
					info.expr = expr;
					info.stack = putils::getCallStack();

					e += TimeModulatorComponent{ 0.f };

					EntityID id = e.id;
					e += functions::Execute{ [id, &info](float deltaTime) {
						bool open = true;
						ImGui::SetNextWindowSize({ 300, 400 }, ImGuiCond_Appearing);
						if (ImGui::Begin(putils::string<64>("Assertion failed##%d", id), &open)) {

							ImGui::BeginChild("##Child", { 0, -ImGui::GetTextLineHeightWithSpacing() });
							{
								ImGui::Text("%s l.%d", info.file, info.line);
								ImGui::Separator();
								ImGui::Text(info.expr.c_str());
								ImGui::Separator();
								ImGui::Text(info.stack.c_str());
								if (ImGui::IsItemClicked(1))
									ImGui::SetClipboardText(info.stack.c_str());
								if (ImGui::IsItemHovered())
									ImGui::SetTooltip("Right-click to copy to clipboard");
							}
							ImGui::EndChild();

							if (ImGui::Button("Skip"))
								entities -= id;
							ImGui::SameLine();
							if (ImGui::Button("Ignore"))
								ignore(info.file, info.line);
							ImGui::SameLine();
							if (ImGui::Button("Exit"))
								exit(1);
						}
						ImGui::End();

						if (!open)
							entities -= id;
					} };
				};

				return true;
			}

			static void ignore(const char * file, int line) noexcept {
				std::vector<kengine::EntityID> toRemove;
				g_assertMap[file].insert(line);
				for (const auto & [e, info] : kengine::entities.with<AssertInfo>())
					if (info.file == file && info.line == line)
						toRemove.push_back(e.id);

				for (const auto id : toRemove)
					kengine::entities -= id;
			}

			static bool isIgnored(const char * file, int line) noexcept {
				const auto it = g_assertMap.find(file);
				if (it == g_assertMap.end())
					return false;

				return it->second.find(line) != it->second.end();
			}

		};

		return impl::assertFailed(file, line, expr);
	}

	bool isDebuggerPresent() noexcept {
#ifdef WIN32
		return IsDebuggerPresent();
#else
		return false;
#endif
	}
}

#endif
