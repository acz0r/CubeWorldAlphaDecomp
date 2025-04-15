#pragma once

#include <memory>

class D3D9Engine {
public:
	D3D9Engine();
	bool unknown_method_1();
};

extern std::unique_ptr<D3D9Engine> g_D3D9_ENGINE;
