#pragma once

#include <d3d9.h>
#pragma comment(lib, "d3d9")

extern IDirect3D9* g_D3D9_INTERFACE;
extern IDirect3DDevice9* g_D3D9_DEVICE_INTERFACE;

bool init_renderer();
