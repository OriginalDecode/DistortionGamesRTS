#pragma once

enum class eMessageType
{
	GAME_STATE,
	RESIZE,
	ON_CLICK,
	MOVE_CAMERA,
	MOVE_UNITS,
	LUA_MOVE_CAMERA,
	SPAWN_UNIT,
	CINEMATIC,
	FADE,
	TOGGLE_GUI,
	LUA_RUN_SCRIPT,
	TOGGLE_LINES,
	TRIGGER,
	RESOURCE,
	TIME_MULTIPLIER,
	VICTORY,
	PARTICLE,
	TOGGLE_BUILD_TIME,
	COUNT,
};