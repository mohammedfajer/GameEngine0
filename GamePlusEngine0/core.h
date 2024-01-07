#pragma once

#include "config.h"

START_NAMESPACE

enum class App_Mode {
	Game,
	Editor
};

struct Time_Info {
	float dt;
};

struct Core {
	static Time_Info time_info;
	static App_Mode app_mode;
};

END_NAMESPACE
