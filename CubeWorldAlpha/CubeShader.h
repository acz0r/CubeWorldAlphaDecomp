#pragma once

#include <memory>

class CubeShader {
public:
	CubeShader();
};

extern std::unique_ptr<CubeShader> g_CUBE_SHADER;
