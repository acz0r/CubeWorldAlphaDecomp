#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

extern HWND g_HWND;
extern HINSTANCE g_HINSTANCE;

[[noreturn]] inline void abort(const char* err_msg);
