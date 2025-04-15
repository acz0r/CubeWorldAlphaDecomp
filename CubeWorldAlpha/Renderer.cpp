#include "CubeWorld.h"
#include "Renderer.h"
#include "D3D9Engine.h"
#include "CubeShader.h"

#include <string>

IDirect3D9* g_D3D9_INTERFACE = nullptr;
IDirect3DDevice9* g_D3D9_DEVICE_INTERFACE = nullptr;

bool init_renderer() {
	/* Create Direct3D interface */
	g_D3D9_INTERFACE = Direct3DCreate9(32);
	if (!g_D3D9_INTERFACE)
		return false;

	/* Create present parameters */
	D3DPRESENT_PARAMETERS d3d9_present_params = { 0 };
	d3d9_present_params.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3d9_present_params.MultiSampleQuality = 0;
	d3d9_present_params.BackBufferWidth = GetSystemMetrics(0);
	d3d9_present_params.BackBufferHeight = GetSystemMetrics(1);
	d3d9_present_params.Windowed = true;
	d3d9_present_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3d9_present_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3d9_present_params.EnableAutoDepthStencil = true;
	d3d9_present_params.AutoDepthStencilFormat = D3DFMT_D24S8;

	if (FAILED(g_D3D9_INTERFACE->CreateDevice(
		0u,
		D3DDEVTYPE_HAL,
		g_HWND,
		D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3d9_present_params,
		&g_D3D9_DEVICE_INTERFACE
	))) {
		g_D3D9_INTERFACE->Release();
		return false;
	}

	try {
		g_D3D9_ENGINE = std::make_unique<D3D9Engine>();
	}
	catch (const std::bad_alloc&) {
		abort("Failed to allocate memory for D3D9Engine");
	}

	std::wstring fonts_path = L"c:\\windows\\fonts";
	// Unknown method ((D3D9_ENGINE->ObjectManager_data).offset_0x30,&fonts_path)
	if (g_D3D9_ENGINE->unknown_method_1()) {
		try {
			g_CUBE_SHADER = std::make_unique<CubeShader>();
		}
		catch (const std::bad_alloc&) {
			abort("Failed to allocate memory for CubeShader");
		}

		// TODO
	}

	return true;
}
