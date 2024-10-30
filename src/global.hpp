#pragma once
#include "scenelib/sdl.hpp"
#include "scenelib/gfx.hpp"
#include "scenelib/dpad.hpp"


struct Scene {
	// enum GAMESCENE {
	// 	SCENE_TITLE,
	// 	SCENE_GAME,
	// 	SCENE_MENU,
	// };

	static const  int TSIZE = 16;
	static inline int tilesetimage = 0, playerimage = 0;
	// static inline GAMESCENE currentscene = SCENE_TITLE, toscene = SCENE_TITLE;
	static inline DPad dpad;

	// static void switchscene(GAMESCENE scene, int levelno=0);
	
	virtual void init() {}
	virtual void update() {}
	virtual void paint() {}
};