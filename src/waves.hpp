#pragma once
#include "global.hpp"
#include <cmath>

struct Wave {
	enum ENEMY_T {
		ENEMY_ORB = 200,
		ENEMY_WOBBLEORB = 201,
	};
	struct SentryData { int anim, frameoffset; };
	struct SentryWobbleData : SentryData { double x, y, xspeed, yspeed, xacc; };

	static const int TSIZE = Scene::TSIZE;
	GFX::Scene& gfx;
	vector<int> enemys;
	int delta = 0, wave = 0;

	void update() {
		delta++;
		// move enemys
		for (int i = (int)enemys.size()-1; i >= 0; i--)
			if (updateenemy( enemys[i] ))
				enemys.erase( enemys.begin() + i );
	}

	void kill(int enemyid) {
		auto it = find( enemys.begin(), enemys.end(), enemyid );
		if (it != enemys.end()) {
			gfx.freesprite( enemyid );
			enemys.erase( it );
		}
	}

	int makeorb(int x) {
		int enemyid = gfx.makesprite( Scene::tilesetimage, TSIZE, TSIZE );
		auto& enemy = gfx.getsprite( enemyid );
		enemy.src.x = TSIZE * 4;
		// enemy.pos.x = 2 + ( rand() % (Scene::SCENEW - TSIZE - 4) );
		enemy.pos.x = x;
		enemy.pos.y = -TSIZE;
		// enemy data
		enemy.usertype = ENEMY_ORB;
		enemy.userdata.resize( sizeof(SentryData) );
		// auto& data = (SentryData&)enemy.userdata[0];
		// data.alive = true;
		enemys.push_back( enemyid );
		return enemyid;
	}

	int makewobbleorb(int x) {
		int enemyid = makeorb( x );
		auto& enemy = gfx.getsprite( enemyid );
		// enemy data
		enemy.usertype = ENEMY_WOBBLEORB;
		enemy.userdata.resize( sizeof(SentryWobbleData) );
		auto& data = (SentryWobbleData&)enemy.userdata[0];
		data.x = enemy.pos.x;
		data.y = enemy.pos.y;
		data.yspeed = 1;
		data.xacc = 0.07;
		return enemyid;
	}

	bool updateenemy(int enemyid) {
		auto& enemy = gfx.getsprite( enemyid );
		// animate orb
		auto& data = (SentryData&)enemy.userdata[0];
		data.anim++;
		if (data.anim >= 30) {
			data.frameoffset = (data.frameoffset + 1) % 2;
			data.anim = 0;
			enemy.src.x = TSIZE * (4 + data.frameoffset);
		}
		// straight down orb
		if (enemy.usertype == ENEMY_ORB) {
			enemy.pos.y += 1;
		}
		// wobble orb
		if (enemy.usertype == ENEMY_WOBBLEORB) {
			auto& data = (SentryWobbleData&)enemy.userdata[0];
			data.xspeed += data.xacc;
			if (abs(data.xspeed) >= 1.0)  data.xacc = -data.xacc;
			data.x += data.xspeed;
			data.y += data.yspeed;
			enemy.pos.x = round(data.x);
			enemy.pos.y = round(data.y);
		}
		// check offscreen, return true if dead
		if (enemy.pos.y > Scene::SCENEH)
			return true;
		return false;
	}
};


struct Wave1 : Wave {
	void update() {
		Wave::update();
		const int 
			w1 = 0, w2 = 51, w3 = 200, wreset = 400,
			interval1 = 15, interval3 = 20;

		// spawn next enemy
		switch (delta) {
		// first wave
		case w1 + interval1 * 1:
		case w1 + interval1 * 2:
		case w1 + interval1 * 3:
		case w1 + interval1 * 4:
		case w1 + interval1 * 5:
			makeorb( 15 );
			break;
		// second wave
		case w2 + interval1 * 1:
		case w2 + interval1 * 2:
		case w2 + interval1 * 3:
		case w2 + interval1 * 4:
		case w2 + interval1 * 5:
			makeorb( Scene::SCENEW - TSIZE - 15 );
			break;
		// wobble wave
		case w3 + interval3 * 1:
		case w3 + interval3 * 2:
		case w3 + interval3 * 3:
		case w3 + interval3 * 4:
		case w3 + interval3 * 5:
		case w3 + interval3 * 6:
		case w3 + interval3 * 7:
			makewobbleorb( (Scene::SCENEW - TSIZE) / 2 - 5 );
			break;
		case wreset:
			delta = 0;
			break;
		}
	}
};