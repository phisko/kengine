#include <windows.h>
#include <winuser.h>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"

#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#ifndef WIN32
static_assert(false, "Only implemented on Windows for now");
#endif

#ifdef WIN32
# include <windowsx.h>
# define GLFW_EXPOSE_NATIVE_WIN32
#  include <GLFW/glfw3native.h>
# undef GLFW_EXPOSE_NATIVE_WIN32
# include "imgui_internal.h"
#endif

#include "EntityManager.hpp"

#include "ImGuiOverlaySystem.hpp"
#include "components/ImGuiComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "packets/ImGuiScale.hpp"

#include "Point.hpp"
#include "to_string.hpp"

#ifndef KENGINE_IMGUI_OVERLAY_TOOLS_SAVE_FILE
# define KENGINE_IMGUI_OVERLAY_TOOLS_SAVE_FILE "tools.cnf"
#endif

static kengine::EntityManager * g_em = nullptr;

namespace kengine {
	static GLFWwindow * g_window = nullptr;
	static bool g_focused = true;
	static bool g_enabled = true;
	static float g_dpiScale = 1.f;

	static void hideFromTaskbar(GLFWwindow * window) {
		const auto win32Window = glfwGetWin32Window(window);
		const auto currStyle = GetWindowLong(win32Window, GWL_EXSTYLE);
		SetWindowLong(win32Window, GWL_EXSTYLE, currStyle & ~WS_EX_APPWINDOW | WS_EX_TOOLWINDOW);
	}

#define MY_SYSTEM_TRAY_MESSAGE (WM_APP + 1) // arbitrary value between WM_APP and 0xBFFF

	static HMENU g_hMenu;
	void showContextMenu() {
		DestroyMenu(g_hMenu);
		g_hMenu = CreatePopupMenu();

		size_t i = 0;
		for (const auto & [e, tool] : g_em->getEntities<ImGuiToolComponent>()) {
			AppendMenu(g_hMenu, MF_STRING, i, tool.name);
			++i;
		}
		AppendMenu(g_hMenu, MF_STRING, i, "Exit");

		POINT curPoint;
		GetCursorPos(&curPoint);

		const auto hWnd = glfwGetWin32Window(g_window);
		SetForegroundWindow(hWnd);
		TrackPopupMenu(g_hMenu, 0, curPoint.x, curPoint.y, 0, hWnd, nullptr);
	}

	static WNDPROC g_prevWndProc;
	static LRESULT wndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
		if (umsg == MY_SYSTEM_TRAY_MESSAGE) {
			if (lParam == WM_LBUTTONDBLCLK)
				glfwFocusWindow(g_window);
			else if (lParam == WM_RBUTTONUP)
				showContextMenu();
		}
		else if (umsg == WM_COMMAND) { // In context menu
			const auto id = LOWORD(wParam);
			size_t i = 0;
			for (const auto & [e, tool] : g_em->getEntities<ImGuiToolComponent>()) {
				if (id == i)
					tool.enabled = !tool.enabled;
				++i;
			}
			if (id >= i) // Exit
				g_em->running = false;
		}

		return CallWindowProc(g_prevWndProc, hwnd, umsg, wParam, lParam);
	}

	static void addToSystemTray() {
		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(nid);
		nid.uID = std::hash<const char *>()("koala overlay");
		nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
		nid.hIcon = (HICON)LoadImage(nullptr, "resources/koala.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		nid.hWnd = glfwGetWin32Window(g_window);
		nid.uCallbackMessage = MY_SYSTEM_TRAY_MESSAGE;
		strcpy_s(nid.szTip, "Koverlay");

		Shell_NotifyIconA(NIM_ADD, &nid);

#ifdef GWL_WNDPROC
		g_prevWndProc = (WNDPROC)SetWindowLongPtr(nid.hWnd, GWL_WNDPROC, (LONG_PTR)&wndProc);
#else
		g_prevWndProc = (WNDPROC)SetWindowLongPtr(nid.hWnd, GWLP_WNDPROC, (LONG_PTR)&wndProc);
#endif
	}

	static HHOOK g_hook;
	static LRESULT __stdcall hookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
		static bool altPressed = false;
		static bool focusPressed = false;
		static bool disablePressed = false;

		if (nCode == HC_ACTION) { // valid action
			const auto kbd = (KBDLLHOOKSTRUCT *)lParam;

			const bool keyDown = wParam == WM_SYSKEYDOWN;

			switch (kbd->vkCode) {
			case VK_LMENU:
				altPressed = keyDown;
				break;
			case 'W':
				focusPressed = keyDown;
				break;
			case 'Q':
				disablePressed = keyDown;
				break;
			}

			if (altPressed && focusPressed && g_enabled)
				g_focused = !g_focused;
			else if (altPressed && disablePressed)
				g_enabled = !g_enabled;
		}
		return CallNextHookEx(g_hook, nCode, wParam, lParam);
	}

	static void addKeyboardHook() {
		g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, hookCallback, nullptr, 0);
	}

	ImGuiOverlaySystem::ImGuiOverlaySystem(kengine::EntityManager & em)
		: System(em),
		_em(em)
	{
		g_em = &em;
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Overlay] Scale", &g_dpiScale); };
		g_confFile.parse();
	}

	void ImGuiOverlaySystem::init() const { // Separate so that adjustables have already been loaded
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

#ifndef KENGINE_NDEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

		const auto videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		const putils::Point2i screenSize = { videoMode->width - 1, videoMode->height - 1 };
		g_window = glfwCreateWindow((int)screenSize.x, (int)screenSize.y, "Koverlay", nullptr, nullptr);

		glfwMakeContextCurrent(g_window);
		glfwSetWindowAspectRatio(g_window, (int)screenSize.x, (int)screenSize.y);

		ImGui::CreateContext();
		auto & io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigViewportsNoTaskBarIcon = true;

		{ // Stolen from ImGui_ImplOpenGL3_CreateFontsTexture
			ImFontConfig config;
			config.SizePixels = 13.f * g_dpiScale;
			io.Fonts->AddFontDefault(&config);
			unsigned char * pixels;
			int width, height;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

			GLint last_texture;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
			glBindTexture(GL_TEXTURE_2D, (GLuint)io.Fonts->TexID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			ImGui::GetStyle().ScaleAllSizes(g_dpiScale);
			glBindTexture(GL_TEXTURE_2D, last_texture);
		}

		ImGui_ImplGlfw_InitForOpenGL(g_window, true);
		ImGui_ImplOpenGL3_Init();

		glewExperimental = true;
		const bool ret = glewInit();
		assert(ret == GLEW_OK);

		glfwSetWindowFocusCallback(g_window, [](GLFWwindow *, int focused) {
			g_focused = focused == GLFW_TRUE;
		});

#ifndef KENGINE_NDEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam) {
			if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
				fprintf(stderr, "GL: severity = 0x%x, message = %s\n",
					severity, message);
			// assert(type != GL_DEBUG_TYPE_ERROR);
		}, nullptr);
#endif

		hideFromTaskbar(g_window);
		addToSystemTray();
		addKeyboardHook();

		_em += ToolsController();
	}

	ImGuiOverlaySystem::~ImGuiOverlaySystem() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(g_window);
		glfwTerminate();
	}

	static void updateWindowRegion() {
		bool first = true;
		HRGN totalRegion = CreateRectRgn(0, 0, 0, 0);
		if (g_enabled) {
			for (const ImGuiWindow * window : GImGui->Windows) {
				if (!window->Active)
					continue;

				const auto rgn = CreateRectRgn(
					(int)window->Pos.x, (int)window->Pos.y,
					(int)(window->Pos.x + window->Size.x), (int)(window->Pos.y + window->Size.y)
				);
				CombineRgn(totalRegion, totalRegion, rgn, RGN_OR);
				DeleteRgn(rgn);
			}
		}
		SetWindowRgn(glfwGetWin32Window(g_window), totalRegion, true);
		DeleteRgn(totalRegion);
	}

	void ImGuiOverlaySystem::execute() noexcept {
		static bool first = true;
		if (first) {
			init();
			send(packets::ImGuiScale{ g_dpiScale });
			first = false;
		}

		updateWindowRegion();

		glfwPollEvents();
		if (glfwGetWindowAttrib(g_window, GLFW_ICONIFIED)) {
			glfwSwapBuffers(g_window);
			return;
		}
		if (glfwWindowShouldClose(g_window)) {
			_em.running = false;
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (g_enabled) {
			for (const auto &[e, comp] : _em.getEntities<kengine::ImGuiComponent>())
				comp.display(GImGui);

			if (!g_focused)
				for (ImGuiWindow * window : GImGui->Windows)
					if (strcmp(window->Name, "##MainMenuBar") == 0)
						window->Active = false;
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(g_window);
	}
}