#pragma once

#include "Controller.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <memory>

class GameController : public Controller {
public:
	bool m_unknown_flag_1 = false; // TODO
	DWORD m_target_frame_time = 1000 / 60;

	GameController();
	bool unknown_method_1();
};

extern std::unique_ptr<GameController> g_GAME_CONTROLLER;
