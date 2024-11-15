#pragma once
#include "global.hpp"
#include <cmath>

struct Wave {
	enum ENEMY_T {
		ENEMY_ORB = 200,
		ENEMY_WOBBLEORB = 201,
	};
	struct EnemyData {
		int anim, frameoffset;
		double xspeed, yspeed, xacc;
	};

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
		enemy.userdata.resize( sizeof(EnemyData) );
		auto& data = (EnemyData&)enemy.userdata[0];
		data.yspeed = 1;
		enemys.push_back( enemyid );
		return enemyid;
	}

	int makewobbleorb(int x) {
		int enemyid = makeorb( x );
		auto& enemy = gfx.getsprite( enemyid );
		// enemy data
		enemy.usertype = ENEMY_WOBBLEORB;
		auto& data = (EnemyData&)enemy.userdata[0];
		data.xacc = 0.07;
		return enemyid;
	}

	bool updateenemy(int enemyid) {
		auto& enemy = gfx.getsprite( enemyid );
		// animate orb
		auto& data = (EnemyData&)enemy.userdata[0];
		data.anim++;
		if (data.anim >= 30) {
			data.frameoffset = (data.frameoffset + 1) % 2;
			data.anim = 0;
			enemy.src.x = TSIZE * (4 + data.frameoffset);
		}
		// straight down orb
		if (enemy.usertype == ENEMY_ORB) {
			enemy.pos.y += data.yspeed;
		}
		// wobble orb
		else if (enemy.usertype == ENEMY_WOBBLEORB) {
			data.xspeed += data.xacc;
			if (abs(data.xspeed) >= 1.0)  data.xacc = -data.xacc;
			enemy.pos.x += data.xspeed;
			enemy.pos.y += data.yspeed;
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