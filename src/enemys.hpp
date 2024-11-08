#pragma once
#include "global.hpp"
#include <vector>
#include <cmath>
using namespace std;


// Enemys Container
struct EnemyGroup {
	// enemy types
	struct Enemy {
		static const int TSIZE = Scene::TSIZE;
		GFX::Scene* gfx = NULL;
		int spriteid = 0, alive = true;
		virtual ~Enemy() = default;
		virtual void init() = 0;
		virtual void free() = 0;
		virtual void update() = 0;
	};
	struct Sentry;
	struct SentryWobble;

	GFX::Scene& gfx;
	map<int, Enemy*> enemys;

	void spawn(Enemy* enemy) {
		enemy->gfx = &gfx;
		enemy->init();
		enemys[enemy->spriteid] = enemy;
	}

	void kill(int id) {
		if (!enemys.count(id))  return;
		enemys[id]->free();
		delete enemys[id];
		enemys.erase(id);
		// printf("killed %d\n", (int)id);
	}

	void update() {
		for (auto& [id, enemy] : enemys)
			enemy->update();
	}

	void cleardead() {
		vector<int> dead;
		for (auto& [id, enemy] : enemys)
			if (!enemy->alive)
				dead.push_back( enemy->spriteid );
		for (auto id : dead)
			kill( id );
	}
};


// Enemys
struct EnemyGroup::Sentry : Enemy {
	static const int ENEMY_SPEED = 1, ANIM_TT = 30;
	int animtt = 0, anim = 0;

	virtual void init() {
		if (!gfx) return;
		spriteid = gfx->makesprite( Scene::tilesetimage, TSIZE, TSIZE );
		auto& enemy = gfx->getsprite( spriteid );
		enemy.src.x = TSIZE * 4;
		enemy.pos.x = 2 + ( rand() % (Scene::SCENEW - TSIZE - 4) );
		enemy.pos.y = -TSIZE;
		alive = true;
	}

	virtual void free() {
		if (!gfx) return;
		gfx->freesprite( spriteid );
	}

	virtual void animate(GFX::Sprite& enemy) {
		animtt++;
		if (animtt >= ANIM_TT) {
			anim = (anim + 1) % 2;
			enemy.src.x = TSIZE * ( 4 + anim );
			animtt = 0;
		}
	}

	virtual void move(GFX::Sprite& enemy) {
		enemy.pos.y += ENEMY_SPEED;
	}

	virtual void update() {
		if (!gfx) return;
		auto& enemy = gfx->getsprite( spriteid );
		animate( enemy );
		move( enemy );
		if (enemy.pos.y > Scene::SCENEH)
			alive = false;
	}
};

struct EnemyGroup::SentryWobble : Sentry {
	double x = 0, y = 0, xspeed = 0, yspeed = 1, xacc = 0.07;

	void init() {
		Sentry::init();
		if (!gfx) return;
		auto& enemy = gfx->getsprite( spriteid );
		x = enemy.pos.x;
		y = enemy.pos.y;
	}

	void move(GFX::Sprite& enemy) {
		xspeed += xacc;
		if (abs(xspeed) >= 1.0)  xacc = -xacc;
		x += xspeed;
		y += yspeed;
		enemy.pos.x = round(x);
		enemy.pos.y = round(y);
	}

	void update() {
		Sentry::update();
	}
};