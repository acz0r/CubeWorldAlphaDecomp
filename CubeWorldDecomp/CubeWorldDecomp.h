#pragma once

#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxguid.lib")

#include <windows.h>
#include <timeapi.h>
#include <D3d9.h>

const wchar_t* APP_NAME = L"Cube";

static HWND H_WND = nullptr;
static IDirect3DDevice9* DIRECT3D_DEVICE = nullptr;
static IDirect3D9* DIRECT3D_OBJ = nullptr;
static int PRIMARY_DISPLAY_WIDTH = 0;
static int PRIMARY_DISPLAY_HEIGHT = 0;

struct OptionsConfig {
	int fullscreen;
	int resolution_x;
	int resulution_y;
	int anti_aliasing;
	int render_distance;
	int sound_volume;
	int music_volume;
	int camera_speed;
	int camera_smoothness;
	int invert_y;
	int language;
	int min_time_step;
};

static OptionsConfig OPTIONS_CONFIG;

struct DisplayMode {
	unsigned int width;
	unsigned int height;
};
