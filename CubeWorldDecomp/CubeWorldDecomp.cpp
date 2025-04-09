#define _CRT_SECURE_NO_DEPRECATE

#include "CubeWorldDecomp.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool init_renderer();
void parse_options_config(OptionsConfig* options_config);

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	HINSTANCE v6 = hInstance;
	HINSTANCE previous_time = hInstance;

	WNDCLASSW wnd_class = {};
	wnd_class.style = 0;
	wnd_class.lpfnWndProc = WndProc;
	wnd_class.cbClsExtra = 0;
	wnd_class.cbWndExtra = 0;
	wnd_class.hInstance = hInstance;
	/*wnd_class.hIcon = LoadIconW(hInstance, APP_NAME);*/
	wnd_class.hCursor = 0;
	wnd_class.hbrBackground = (HBRUSH) 6;
	wnd_class.lpszMenuName = APP_NAME;
	wnd_class.lpszClassName = APP_NAME;

	if (!RegisterClassW(&wnd_class))
		return 0;

	H_WND = CreateWindowExW(0, APP_NAME, APP_NAME, 0x6CF0000u, 200, 200, 800, 600, 0, 0, hInstance, 0);
	if (!H_WND)
		return 0;

	if (!init_renderer()) { // TODO: Init direct3d
		MessageBoxA(
			H_WND,
			"Could not initialize Direct3D. Please make sure the latest DirectX End-User Runtime is installed: http://www.microsoft.com/en-us/download/details.aspx?id=35",
			"Cube World",
			0
		);

		return 0; 
	}

	UINT adapter_mode_count = DIRECT3D_OBJ->GetAdapterModeCount(0, D3DFMT_X8R8G8B8);
	std::vector<DisplayMode> unique_display_modes;
	D3DDISPLAYMODE current_display_mode;
	for (UINT adapter_mode_index = 0; adapter_mode_index < adapter_mode_count; adapter_mode_index++) {
		DIRECT3D_OBJ->EnumAdapterModes(0, D3DFMT_X8R8G8B8, adapter_mode_index, &current_display_mode);

		// Skip duplicates
		bool is_duplicate = false;
		for (const auto& mode : unique_display_modes) {
			if (mode.width == current_display_mode.Width && mode.height == current_display_mode.Height) {
				is_duplicate = true;
				break;
			}
		}

		if (!is_duplicate) {
			DisplayMode* new_display_mode = new DisplayMode;
			new_display_mode->width = current_display_mode.Width;
			new_display_mode->height = current_display_mode.Height;

			unique_display_modes.push_back(*new_display_mode);
		}
	}

	PRIMARY_DISPLAY_WIDTH = GetSystemMetrics(0);
	PRIMARY_DISPLAY_HEIGHT = GetSystemMetrics(1);

	parse_options_config(&OPTIONS_CONFIG);

	// Custom stdout behavior
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	//if (!true) { // TODO: Init xaudio2
	//	MessageBoxA(
	//		H_WND,
	//		"Could not initialize XAudio2. Please make sure the latest DirectX End-User Runtime is installed: http://www.microsoft.com/en-us/download/details.aspx?id=35",
	//		"Cube World",
	//		0
	//	);

	//	return 0;
	//}

	//// TODO: Initialize DirectInput8

	//// TODO

	ShowWindow(H_WND, SW_SHOWMAXIMIZED);
	/*SetCursor(nullptr);
	ShowCursor(false);*/
	UpdateWindow(H_WND);
	SetFocus(H_WND);

	MSG msg;
	while (true) {
		previous_time = (HINSTANCE) timeGetTime();
		if (PeekMessageW(&msg, nullptr, 0u, 0u, 0u)) {
			if (!GetMessageW(&msg, nullptr, 0u, 0u))
				break;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result;
	
	// Check for mouse events
	if (uMsg > 0x200) {
		switch (uMsg) {
			case WM_LBUTTONDOWN:
				// TODO
				result = 1;
				break;
			case WM_LBUTTONUP:
				// TODO
				result = 1;
				break;
			case WM_RBUTTONDOWN:
				// TODO
				result = 1;
				break;
			case WM_RBUTTONUP:
				// TODO
				result = 1;
				break;
			case WM_MBUTTONDOWN:
				// TODO
				result = 1;
				break;
			case WM_MBUTTONUP:
				// TODO
				result = 1;
				break;
			case WM_MOUSEWHEEL:
				//WPARAM v5 = HIWORD(wParam);
				//if ((wParam & 0x80000000) == 0) {
				//	/*if (SHIWORD(wParam) > 0)
				//		LOWORD(v5) = 1;*/
				//}
				//else {
				//	/*LOWORD(v5) = -1;*/
				//}

				return 1; 
			default:
				return DefWindowProcW(hwnd, uMsg, wParam, lParam);
		}
	}
	else if (uMsg == WM_MOUSEMOVE) {
		return 1;
	}
	else if (uMsg > WM_CLOSE) {
		switch (uMsg) {
			case WM_KEYDOWN:
				// TODO
				return 0;
			case WM_KEYUP:
				// TODO
				return 0;
			case WM_CHAR:
				// TODO
				return 0;
			default:
				return DefWindowProcW(hwnd, uMsg, wParam, lParam);
		}
	}
	else if (uMsg == WM_CLOSE) {
		DestroyWindow(hwnd);
		return 1;
	}
	else {
		switch (uMsg) {
			case 2u:
				PostQuitMessage(0);
				result = 1;
				break;
			case 5u:
			case 7u:
				// TODO
				result = 1;
				break;
			case 8u:
				return 1;
			default:
				return DefWindowProcW(hwnd, uMsg, wParam, lParam);
		}
	}

	return result;
}

bool init_renderer() {
	DIRECT3D_OBJ = Direct3DCreate9(D3D_SDK_VERSION);
	if (!DIRECT3D_OBJ)
		return false;

	D3DPRESENT_PARAMETERS direct3d_params = { 0 };
	direct3d_params.MultiSampleType = D3DMULTISAMPLE_NONE;
	direct3d_params.MultiSampleQuality = 0;
	direct3d_params.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
	direct3d_params.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
	direct3d_params.Windowed = true;
	direct3d_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	direct3d_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	direct3d_params.EnableAutoDepthStencil = true;
	direct3d_params.AutoDepthStencilFormat = D3DFMT_D24S8;

	if (DIRECT3D_OBJ->CreateDevice(0, D3DDEVTYPE_HAL, H_WND, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &direct3d_params, &DIRECT3D_DEVICE) != S_OK) {
		DIRECT3D_OBJ->Release();
		DIRECT3D_OBJ = nullptr;
		return false;
	}

	
	
	return true;
}

void parse_options_config(OptionsConfig* options_config) {
	std::string text;
	std::ifstream options_file("options.cfg");
	if (options_file.fail()) {
		return;
	}

	while (std::getline(options_file, text)) {
		std::cout << text << '\n';
	}
}

