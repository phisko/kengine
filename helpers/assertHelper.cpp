#ifndef KENGINE_NDEBUG

#include "assertHelper.hpp"
#include "kengine.hpp"

// stl
#include <unordered_set>

// imgui
#include <imgui.h>

// putils
#include "string.hpp"
#include "get_call_stack.hpp"

// kengine data
#include "data/TimeModulatorComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::assertHelper {
#ifdef KENGINE_IMGUI_ASSERT_HANDLER
    static bool imguiAssertHandler(const char * file, int line, const std::string & expr) noexcept;
#endif

    std::function<bool(const char * file, int line, const std::string & expr)> assertHandler =
#ifdef KENGINE_IMGUI_ASSERT_HANDLER
        imguiAssertHandler;
#else
        nullptr;
#endif

    bool assertFailed(const char * file, int line, const std::string & expr) noexcept {
		KENGINE_PROFILING_SCOPE;
        kengine_logf(Error, "Assert", "%s:%d %s", file, line, expr.c_str());

        if (assertHandler)
            return assertHandler(file, line, expr);
        return true;
    }

    bool isDebuggerPresent() noexcept {
		KENGINE_PROFILING_SCOPE;
#ifdef WIN32
		return IsDebuggerPresent();
#else
		return false;
#endif
	}

#ifdef KENGINE_IMGUI_ASSERT_HANDLER
    static bool imguiAssertHandler(const char * file, int line, const std::string & expr) noexcept {
		KENGINE_PROFILING_SCOPE;

        using File = std::unordered_set<int>; // Set of ignored lines
        using AssertMap = std::unordered_map<const char *, File>; // Map of files
        static AssertMap g_assertMap;

        struct impl {
            struct AssertInfo {
                const char *file;
                int line;
                std::string expr;
                std::string stack;
            };

            static bool assertFailed(const char *file, int line, const std::string &expr) noexcept {
				KENGINE_PROFILING_SCOPE;

                if (isIgnored(file, line))
                    return false;

                entities += [file, line, expr](Entity &e) {
                    auto &info = e.attach<AssertInfo>();
                    info.file = file;
                    info.line = line;
                    info.expr = expr;
                    info.stack = putils::getCallStack();

                    e += TimeModulatorComponent{0.f};

                    EntityID id = e.id;
                    e += functions::Execute{[id, &info](float deltaTime) {
                        bool open = true;
                        ImGui::SetNextWindowSize({300, 400}, ImGuiCond_Appearing);
                        if (ImGui::Begin(putils::string<64>("Assertion failed##%d", id), &open)) {

                            ImGui::BeginChild("##Child", {0, -ImGui::GetTextLineHeightWithSpacing()});
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
                    }};
                };

                return true;
            }

            static void ignore(const char *file, int line) noexcept {
				KENGINE_PROFILING_SCOPE;
                kengine_logf(Verbose, "Assert", "Ignoring asserts for %s:%d", file, line);

                std::vector<kengine::EntityID> toRemove;
                g_assertMap[file].insert(line);
                for (const auto &[e, info]: kengine::entities.with<AssertInfo>())
                    if (info.file == file && info.line == line)
                        toRemove.push_back(e.id);

                for (const auto id: toRemove)
                    kengine::entities -= id;
            }

            static bool isIgnored(const char *file, int line) noexcept {
				KENGINE_PROFILING_SCOPE;

                const auto it = g_assertMap.find(file);
                if (it == g_assertMap.end())
                    return false;

                return it->second.find(line) != it->second.end();
            }
        };

        return impl::assertFailed(file, line, expr);
    }
#endif
}

#endif
