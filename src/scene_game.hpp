#pragma once
#include "global.hpp"
#include <vector>
// #include <functional>
using namespace std;


struct EnemySentry {
	static const int TSIZE = Scene::TSIZE, ENEMY_SPEED = 1;
	GFX::Scene* gfx = NULL;
	int enemyid = 0;
	bool alive = true;

	EnemySentry(GFX::Scene& gfxref) : gfx(&gfxref) {
		enemyid = gfx->makesprite( Scene::tilesetimage, TSIZE, TSIZE );
		auto& enemy = gfx->getsprite( enemyid );
		enemy.src.x = TSIZE * 5;
		enemy.pos.x = rand() % Scene::SCENEW;
		enemy.pos.y = -TSIZE;
	}

	void update() {
		auto& enemy = gfx->getsprite( enemyid );
		enemy.pos.y += ENEMY_SPEED;
		if (enemy.pos.y > Scene::SCENEH)
			alive = false;
	}
};


struct SceneGame : Scene {
	// scene data
	GFX::Scene gfx;
	int shipspriteid = 0;
	// bullet data
	const int BULLET_CD = 7, BULLET_SPEED = 2;
	vector<int> bullets;
	int bulletcd = 0;
	// enemys
	vector<EnemySentry> sentrys;

	void init() {
		shipspriteid = gfx.makesprite( tilesetimage, TSIZE, TSIZE );
		auto& ship = gfx.getsprite( shipspriteid );
		ship.pos.x = (SCENEW - ship.pos.w) / 2;
		ship.pos.y = SCENEH - 16;
	}

	void update() {
		// move ship
		auto& ship = gfx.getsprite( shipspriteid );
		ship.pos.x = max( 1, min( SCENEW - ship.pos.w - 1, ship.pos.x + dpad.xaxis ) );

		// move bullets
		for (int i = bullets.size() - 1; i >= 0; i--) {
			auto& bullet = gfx.getsprite( bullets[i] );
			bullet.pos.y -= BULLET_SPEED;
			if (bullet.pos.y < -TSIZE) {
				gfx.freesprite( bullets[i] );
				bullets.erase( bullets.begin() + i );
			}
		}
		// spawn bullets
		bulletcd = max( bulletcd - 1, 0 );
		if (bulletcd == 0 && dpad.a > 0) {
			// printf("spawn  %d  (%d)\n", (int)bullets.size(), gfx.pcounter);
			bullets.push_back( gfx.makesprite( tilesetimage, 4, TSIZE ) );
			auto& bullet = gfx.getsprite( bullets.back() );
			bullet.src.x = TSIZE * 2;
			bullet.pos.x = ship.pos.x + 2;
			bullet.pos.y = ship.pos.y - TSIZE - 1;
			bullet.z = 100;
			bulletcd = BULLET_CD;
		}

		// move enemys
		for (int i = sentrys.size() - 1; i >= 0; i--) {
			sentrys[i].update();
			if ( !sentrys[i].alive )
				sentrys.erase( sentrys.begin() + i );
		}
		// spawn enemys
		if (sentrys.size() < 10 && rand() % 10 == 0)
			sentrys.push_back( EnemySentry( gfx ) );
	}

	void draw() {
		gfx.drawscene();
	}
};

extern SceneGame game;