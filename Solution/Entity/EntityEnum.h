#pragma once

enum eOwnerType
{
	NOT_USED = -1,
	PLAYER = 1,
	ENEMY = 2,
	TRIGGER = 4,
	NEUTRAL = 8
};

enum eEntityType
{
	DRAGON,
	DRAGON_STATIC,
	PINE_TREE,
	_COUNT,
};

enum class eEntityState : int
{
	IDLE,
	WALKING,
	ATTACKING,
	DYING,
	_COUNT,
};

enum class eComponentType
{
	NOT_USED = -1,
	CONTROLLER, //Controller HAS to be above Actor in this list!!!
	ACTOR,
	ANIMATION,
	COLLISION,
	GRAPHICS,
	MOVEMENT,
	HEALTH,
	_COUNT,
};