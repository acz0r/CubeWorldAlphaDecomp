#include "CubeWorld.h"
#include "Renderer.h"
#include "GameController.h"

#include <winsock.h>
#pragma comment(lib, "ws2_32")
#include <cstdlib>
#include <vector>
#include <dinput.h>
#pragma comment(lib, "dinput8")
#pragma comment(lib, "dxguid")
#include <shellapi.h>
#include <direct.h>
#include <memory>
#include <timeapi.h>
#pragma comment(lib, "winmm")

HWND g_HWND = nullptr;
HINSTANCE g_HINSTANCE = nullptr;

LPCWSTR g_APP_NAME = L"CubeWorld (Alpha)";

static LRESULT CALLBACK WndProc(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param);
struct Resolution {
	uint32_t m_width;
	uint32_t m_height;

	Resolution(uint32_t width, uint32_t height);
};

int WINAPI wWinMain(_In_ HINSTANCE h_instance, _In_opt_ HINSTANCE h_prev_instance, _In_ LPWSTR lp_cmd_line, _In_ int n_show_cmd) {
	g_HINSTANCE = h_instance;
	
	WSADATA WSA_data;
	WSAStartup(0x202, &WSA_data);

	/* Create window class */
	WNDCLASSW wc = { 0 };
	wc.style = 0u;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_HINSTANCE;
	// FIXME: wc.hIcon = LoadIconW(H_INSTANCE, L"CubeWorldAlphaIcon");
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH) COLOR_WINDOWFRAME;
	wc.lpszMenuName = g_APP_NAME;
	wc.lpszClassName = g_APP_NAME;

	/* Register window class */
	if (!RegisterClassW(&wc))
		return 1;

	/* Create main window */
	g_HWND = CreateWindowExW(WS_OVERLAPPED, g_APP_NAME, g_APP_NAME, WS_OVERLAPPEDWINDOW, 200, 200, 800, 600, nullptr, nullptr, g_HINSTANCE, nullptr);
	if (g_HWND == nullptr)
		return 1;
	
	/* Initialize renderer */
	if (!init_renderer())
		abort("Failed to initialize Direct3D.                                     \
			   Please make sure the latest DirectX End-User Runtime is installed: \
			   http://www.microsoft.com/en-us/download/details.aspx?id=35");

	/* Gather available resolutions */
	UINT adapter_mode_count = g_D3D9_INTERFACE->GetAdapterCount();
	std::vector<Resolution> resolutions;
	for (UINT adapter_mode_index = 0u; adapter_mode_index < adapter_mode_count; adapter_mode_index++) {
		D3DDISPLAYMODE display_mode;
		if (FAILED(g_D3D9_INTERFACE->EnumAdapterModes(0u, D3DFMT_X8R8G8B8, adapter_mode_index, &display_mode)))
			continue;

		/* Check if this is a unique resolution */
		bool is_duplicate = false;
		for (const auto& resolution : resolutions)
			if (display_mode.Width == resolution.m_width && display_mode.Height == resolution.m_height) {
				is_duplicate = true;
				break;
			}

		/* Only add new resolution if it is unique */
		if (!is_duplicate)
			resolutions.emplace_back(display_mode.Width, display_mode.Height);
	}

	// FIXME: PRIMARY_DISPLAY_WIDTH = GetSystemMetrics(0);
	// FIXME: PRIMARY_DISPLAY_WIDTH = GetSystemMetrics(1);

	// TODO: Intialize audio

	/* Create DirectInput8 interface */
	LPDIRECTINPUT8 lp_dinput_interface = nullptr;
	if (FAILED(DirectInput8Create(g_HINSTANCE, 0x800, IID_IDirectInput8W, (LPVOID*)&lp_dinput_interface, nullptr)))
		abort("Could not initialize DirectInput8.                                  \
			   Please make sure the latest DirectX End-User Run time is installed: \
			   http://www.microsoft.com/en-us/download/details.aspx?id=35");

	/* Create keyboard interface */
	LPDIRECTINPUTDEVICE8 lp_keyboard_interface = nullptr;
	if (FAILED(lp_dinput_interface->CreateDevice(GUID_SysKeyboard, &lp_keyboard_interface, nullptr)))
		abort("Failed to create DirectInput8 keyboard interface");
	
	if (FAILED(lp_keyboard_interface->SetDataFormat(&c_dfDIKeyboard)))
		abort("Failed to set DirectInput8 keyboard data format");

	/* Create mouse interface */
	LPDIRECTINPUTDEVICE8 lp_mouse_interface = nullptr;
	if (FAILED(lp_dinput_interface->CreateDevice(GUID_SysMouse, &lp_mouse_interface, nullptr)))
		abort("Failed to create DirectInput8 mouse interface");

	if (FAILED(lp_mouse_interface->SetDataFormat(&c_dfDIMouse)))
		abort("Failed to set DirectInput8 mouse data format");

	/* Parse command line arguments */
	int num_cmd_line_args = 0;
	LPWSTR* cmd_line_args = CommandLineToArgvW(lp_cmd_line, &num_cmd_line_args);
	if (cmd_line_args != nullptr) {
		/* Check if the 'server' flag was passed */
		if (num_cmd_line_args > 1 && cmd_line_args[1] == L"server") {
			// TODO: Server stuff?
		}
		else {
			// TODO: Non-server stuff?
		}

		LocalFree(cmd_line_args);
	}

	/* Create save directory */
	if (_mkdir("Save") != 0 && errno != EEXIST)
		abort("Failed to make Save directory");

	/* Initialize GameController */
	try {
		g_GAME_CONTROLLER = std::make_unique<GameController>();
	}
	catch (const std::bad_alloc&) {
		abort("Failed to allocate memory for GameController");
	}

	ShowWindow(g_HWND, SW_SHOWMAXIMIZED);
	// FIXME: SetCursor(nullptr);
	// FIXME: ShowCursor(false);
	UpdateWindow(g_HWND);
	SetFocus(g_HWND);

	/* Begin game loop */
	timeBeginPeriod(1u);
	DWORD prev_time;
	while (true) {
		prev_time = timeGetTime();
		MSG msg;
		if (PeekMessageW(&msg, nullptr, WM_NULL, WM_NULL, PM_REMOVE)) {
			if (msg.message != WM_QUIT) {
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
			else
				break;

			/* TODO: desc (potentially checking for a 'quitting' flag) */
			if (g_GAME_CONTROLLER->m_unknown_flag_1)
				PostQuitMessage(0);

			/* TODO: desc */
			if (false /* TODO: g_SOME_GLOBAL_FLAG */) {
				RECT client_rect;
				GetClientRect(g_HWND, &client_rect);
				POINT cursor_pos;
				GetCursorPos(&cursor_pos);
				ScreenToClient(g_HWND, &cursor_pos);

				HWND current_window = GetFocus();

				/* TODO: desc */
				if (!g_GAME_CONTROLLER->unknown_method_1()) {
					if (current_window = g_HWND) {
						cursor_pos.x = client_rect.right / 2l;
						cursor_pos.y = client_rect.bottom / 2l;
						// TODO: g_UNKNOWN_FLAG = false;
						ClientToScreen(g_HWND, &cursor_pos);
						SetCursorPos(cursor_pos.x, cursor_pos.y);
					}
				}
				else if (false /* TODO: !g_UNKNOWN_FLAG */) {
					// TODO
				}
				// TODO: g_UNKNOWN_FLAG = true;

				/* Update key and mouse press flags */
				if (false /* TODO: g_SOME_GLOBAL_FLAG */ && current_window == g_HWND) {
					/* Update key press flags */
					BYTE keyboard_state[256] = {};
					lp_keyboard_interface->GetDeviceState(256l, &keyboard_state);
					g_GAME_CONTROLLER->m_W_pressed = (keyboard_state[17] != 0);
					g_GAME_CONTROLLER->m_A_pressed = (keyboard_state[30] != 0);
					g_GAME_CONTROLLER->m_S_pressed = (keyboard_state[31] != 0);
					g_GAME_CONTROLLER->m_D_pressed = (keyboard_state[32] != 0);

					g_GAME_CONTROLLER->m_Q_pressed = (keyboard_state[16] != 0);
					g_GAME_CONTROLLER->m_E_pressed = (keyboard_state[18] != 0);
					g_GAME_CONTROLLER->m_R_pressed = (keyboard_state[19] != 0);
					g_GAME_CONTROLLER->m_T_pressed = (keyboard_state[20] != 0);

					g_GAME_CONTROLLER->m_SPACE_pressed = (keyboard_state[57] != 0);
					g_GAME_CONTROLLER->m_LSHIFT_pressed = (keyboard_state[42] != 0);
					g_GAME_CONTROLLER->m_LCTRL_pressed = (keyboard_state[29] != 0);

					g_GAME_CONTROLLER->m_1_pressed = (keyboard_state[2] != 0);
					g_GAME_CONTROLLER->m_2_pressed = (keyboard_state[3] != 0);
					g_GAME_CONTROLLER->m_3_pressed = (keyboard_state[4] != 0);
					g_GAME_CONTROLLER->m_4_pressed = (keyboard_state[5] != 0);

					/* Update mouse press flags */
					DIMOUSESTATE mouse_state = {};
					lp_mouse_interface->GetDeviceState(16, &mouse_state);

					// TODO: Some mouse stuff (includes scroll wheel handling)

					g_GAME_CONTROLLER->m_LMB_pressed = (mouse_state.rgbButtons[0] != 0);
					g_GAME_CONTROLLER->m_RMB_pressed = (mouse_state.rgbButtons[1] != 0);
					g_GAME_CONTROLLER->m_MMB_pressed = (mouse_state.rgbButtons[2] != 0);

					// TODO: Unknown extra logic
				}
			}
		}

		// TODO: Other update logic

		DWORD curr_time = timeGetTime();
		DWORD time_elapsed = curr_time - prev_time;
		if (time_elapsed < g_GAME_CONTROLLER->m_target_frame_time)
			Sleep(g_GAME_CONTROLLER->m_target_frame_time - time_elapsed);
	}

	lp_keyboard_interface->Release();
	lp_mouse_interface->Release();
	lp_dinput_interface->Release();

	WSACleanup();

	return 0;
}

static LRESULT CALLBACK WndProc(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
	switch (u_msg) {
	case WM_CLOSE:
		DestroyWindow(g_HWND);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
	case WM_SETFOCUS:
		// TODO: Some func
		return 0;
	case WM_KILLFOCUS:
		return 0;
	case WM_KEYDOWN:
		// TODO: Some func
		return 0;
	case WM_KEYUP:
		// TODO: Some func
		return 0;
	case WM_CHAR:
		// TODO: Some func
		return 0;
	case WM_LBUTTONDOWN:
		// TODO: Some func
		return 0;
	case WM_LBUTTONUP:
		// TODO: Some func
		return 0;
	case WM_RBUTTONDOWN:
		// TODO: Some func
		return 0;
	case WM_RBUTTONUP:
		// TODO: Some func
		return 0;
	case WM_MBUTTONDOWN:
		// TODO: Some func
		return 0;
	case WM_MBUTTONUP:
		// TODO: Some func
		return 0;
	case WM_MOUSEWHEEL:
		// TODO: Some func
		return 0;
	}

	return DefWindowProc(h_wnd, u_msg, w_param, l_param);
}

Resolution::Resolution(uint32_t width, uint32_t height) : m_width(width), m_height(height) {}

[[noreturn]] inline void abort(const char* err_msg) {
	MessageBoxA(g_HWND, err_msg, "CubeWorld Error", 0u);
	std::exit(1);
}
