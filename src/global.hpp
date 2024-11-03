#pragma once
#include "scenelib/sdl.hpp"
#include "scenelib/gfx.hpp"
#include "scenelib/dpad.hpp"


struct Scene {
	static const  int TSIZE = 8, SCENEW = 80, SCENEH = 160;
	static inline int tilesetimage = 0;
	static inline DPad dpad;

	virtual void init() {}
	virtual void update() {}
	virtual void drawscene() {}
};