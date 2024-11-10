#pragma once
#include "global.hpp"

struct Wave1 {
	static const int TSIZE = Scene::TSIZE;
	struct SentryData { char alive; };
	struct SentryWobbleData : SentryData { double x, y, xspeed, yspeed, xacc; };

	GFX::Scene& gfx;
	vector<int> enemys;
	int delta;

	void update() {
		delta++;
		// move enemys
		for (int i = (int)enemys.size()-1; i >= 0; i--)
			if (enemyupdate( enemys[i] ))
				enemys.erase( enemys.begin() + i );
		// spawn next enemy
		switch (delta) {
		case 1:
		case 20:
		case 40:
		case 60:
		case 80:
			enemymake( 30 );
			break;
		case 200:
			delta = 0;
			break;
		}
	}

	void kill(int enemyid) {
		auto it = find( enemys.begin(), enemys.end(), enemyid );
		if (it != enemys.end()) {
			gfx.freesprite( enemyid );
			enemys.erase( it );
		}
	}

	int enemymake(int x) {
		int enemyid = gfx.makesprite( Scene::tilesetimage, TSIZE, TSIZE );
		auto& enemy = gfx.getsprite( enemyid );
		enemy.src.x = TSIZE * 4;
		// enemy.pos.x = 2 + ( rand() % (Scene::SCENEW - TSIZE - 4) );
		enemy.pos.x = x;
		enemy.pos.y = -TSIZE;
		// enemy data
		enemy.usertype = 1;
		enemy.userdata.resize( sizeof(SentryData) );
		// auto& data = (SentryData&)enemy.userdata[0];
		// data.alive = true;
		enemys.push_back( enemyid );
		return enemyid;
	}

	// int enemymake_wobble() {
	// 	int enemyid = enemymake();
	// 	auto& enemy = gfx.getsprite( enemyid );
	// 	// enemy data
	// 	enemy.usertype = 2;
	// 	enemy.userdata.resize( sizeof(SentryWobbleData) );
	// 	auto& data = (SentryWobbleData&)enemy.userdata[0];
	// 	data.x = enemy.pos.x;
	// 	data.y = enemy.pos.y;
	// 	data.yspeed = 1;
	// 	data.xacc = 0.07;
	// 	return enemyid;
	// }

	bool enemyupdate(int enemyid) {
		auto& enemy = gfx.getsprite( enemyid );
		// auto& data = (SentryData&)enemy.userdata[0];
		// straight down enemy
		if (enemy.usertype == 1) {
			enemy.pos.y += 1;
		}
		// wobble enemy
		// if (enemy.usertype == 2) {
		// 	auto& data = (SentryWobbleData&)enemy.userdata[0];
		// 	data.xspeed += data.xacc;
		// 	if (abs(data.xspeed) >= 1.0)  data.xacc = -data.xacc;
		// 	data.x += data.xspeed;
		// 	data.y += data.yspeed;
		// 	enemy.pos.x = round(data.x);
		// 	enemy.pos.y = round(data.y);
		// }
		// check offscreen, return true if dead
		if (enemy.pos.y > Scene::SCENEH)
			return true;
		return false;
	}
};