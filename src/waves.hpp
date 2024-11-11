#pragma once
#include "global.hpp"
#include <cmath>

struct Wave {
	enum ENEMY_T {
		ENEMY_ORB = 200,
		ENEMY_WOBBLEORB = 201,
	};
	struct SentryData { char alive; };
	struct SentryWobbleData : SentryData { double x, y, xspeed, yspeed, xacc; };

	static const int TSIZE = Scene::TSIZE;
	GFX::Scene& gfx;
	vector<int> enemys;
	int delta = 0;

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
		// straight down enemy
		if (enemy.usertype == ENEMY_ORB) {
			// auto& data = (SentryData&)enemy.userdata[0];
			enemy.pos.y += 1;
		}
		// wobble enemy
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
		// spawn next enemy
		switch (delta) {
		case 1:
		case 20:
		case 40:
		case 60:
		case 80:
			makeorb( 15 );
			break;
		case 200:
		case 215:
		case 230:
		case 245:
		case 260:
		case 275:
		case 290:
			makewobbleorb( (Scene::SCENEW - TSIZE) / 2 );
			break;
		case 400:
			delta = 0;
			break;
		}
	}
};