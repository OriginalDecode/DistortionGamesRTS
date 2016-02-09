#pragma once

#define AMOUNT_OF_CONTROL_GROUPS 9
#define AMOUNT_OF_UPGRADES 3
#define PLAYER_COLOR { 0.f, 0.35f, 0.75f, 1.f }

#define LOAD_PROPS
#define USE_WEATHER
//#define CLICK_EXPLOSION
#define USE_PARTICLES
#ifdef _DEBUG

#undef LOAD_PROPS
#undef USE_WEATHER

#endif

#define USE_DIFFICULTY

const bool globalUseParticles = true;
const bool globalUseWeather = true;