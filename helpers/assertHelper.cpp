#include <unordered_set>

#include "AssertHelper.hpp"
#include "EntityManager.hpp"

#include "data/TimeModulatorComponent.hpp"
#include "functions/Execute.hpp"
#include "imgui.h"
#include "lengthof.hpp"
#include "string.hpp"

#ifdef WIN32
# include "dbghelp.h"
#endif

struct AssertInfo {
	const char * file;
	int line;
	std::string expr;
	std::string stack;
};

using File = std::unordered_set<int>; // Set of ignored lines
using AssertMap = std::unordered_map<const char *, File>; // Map of files
static AssertMap g_assertMap;

// declarations
static std::string getCallStack();
static bool isIgnored(kengine::EntityManager & em, const char * file, int line);
static void ignore(kengine::EntityManager & em, const char * file, int line);
//

namespace kengine {
	namespace assertHelper {
		void assertFailed(EntityManager & em, const char * file, int line, const std::string & expr) {
			if (isIgnored(em, file, line))
				return;

			em += [&em, file, line, expr](Entity & e) {
				auto & info = e.attach<AssertInfo>();
				info.file = file;
				info.line = line;
				info.expr = expr;
				info.stack = getCallStack();

				e += TimeModulatorComponent{ 0.f };

				Entity::ID id = e.id;
				e += functions::Execute{[&em, id, &info](float deltaTime) {
					bool open = true;
					ImGui::SetNextWindowPosCenter(ImGuiCond_Appearing);
					ImGui::SetNextWindowSize({ 300, 400 }, ImGuiCond_Appearing);
					if (ImGui::Begin(putils::string<64>("Assertion failed##%d", id), &open)) {

						ImGui::BeginChild("##Child", { 0, -ImGui::GetItemsLineHeightWithSpacing() });
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
							em.removeEntity(id);
						ImGui::SameLine();
						if (ImGui::Button("Ignore"))
							ignore(em, info.file, info.line);
						ImGui::SameLine();
						if (ImGui::Button("Exit"))
							exit(1);
					}
					ImGui::End();

					if (!open)
						em.removeEntity(id);
				}};
			};
		}

		bool isDebuggerPresent() {
#ifdef WIN32
			return IsDebuggerPresent();
#else
			return false;
#endif
		}
	}
}

static std::string getCallStack() {
	std::string ret;
#ifdef WIN32
	const auto process = GetCurrentProcess();
	SymInitialize(process, nullptr, true);

	void * stack[128];
	const auto frames = CaptureStackBackTrace(0, lengthof(stack), stack, nullptr);

	char symbolBuffer[sizeof(SYMBOL_INFO) + 256];
	auto symbol = (SYMBOL_INFO *)symbolBuffer;
	symbol->MaxNameLen = 256;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (int i = 0; i < frames; i++) {
		SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

		DWORD  displacement;
		IMAGEHLP_LINE64 line;
		SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &displacement, &line);

		static constexpr auto stackFramesToIgnore = 5;
		if (i >= stackFramesToIgnore) {
			const putils::string<256> s("\t %i: %s - (l.%i)", frames - i - 1, symbol->Name, line.LineNumber);
			if (!ret.empty())
				ret += '\n';
			ret += s;
		}
	}
#endif
	return ret;
}

static void ignore(kengine::EntityManager & em, const char * file, int line) {
	std::vector<kengine::Entity::ID> toRemove;
	g_assertMap[file].insert(line);
	for (const auto & [e, info] : em.getEntities<AssertInfo>())
		if (info.file == file && info.line == line)
			toRemove.push_back(e.id);

	for (const auto id : toRemove)
		em.removeEntity(id);
}

static bool isIgnored(kengine::EntityManager & em, const char * file, int line) {
	const auto it = g_assertMap.find(file);
	if (it == g_assertMap.end())
		return false;

	return it->second.find(line) != it->second.end();
}

