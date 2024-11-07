#pragma once
#include "global.hpp"
#include <vector>
#include <cmath>
using namespace std;


// User Interface
struct UIF : Scene {
	GFX::Scene gfx;
	int uifsprite = 0;

	void init() {
		uifsprite = gfx.makespriteimage( SCENEW, SCENEH );
		auto& spr = gfx.getsprite( uifsprite );
		spr.pos.x = SCENEW;
		spr.z = 1000;
		// paint ui
		gfx.fill( gfx.getimage(spr.image), 0xff000022 );
	}

	void drawscene() {
		gfx.drawscene();
	}
};


// Stars
struct StarField : Scene {
	const float STAR_SMALL_SPEED = 0.3, STAR_LARGE_SPEED = 0.6;
	GFX::Scene gfx;
	int starimageid = 0;
	vector<int> stars_small, stars_large;
	float starsmalldy = 0, starlargedy = 0;
	
	void init() {
		srand(100);
		// make star background image
		starimageid = gfx.makeimage( 8, 8 );
		auto& starimg = gfx.getimage( starimageid );
		// make small stars
		gfx.px( starimg, 0xff666666, 0, 0 );
		for (int i = 0; i < 100; i++) {
			int starid = gfx.makesprite( starimageid, 1, 1 );
			stars_small.push_back( starid );
			auto& spr = gfx.getsprite( starid );
			spr.src.x = 0;
			spr.pos.y = rand() % SCENEH;
			spr.pos.x = 2 + ( rand() % (SCENEW - 4) );
		}
		// make large stars
		gfx.px( starimg, 0xffffffff, 1, 0 );
		for (int i = 0; i < 15; i++) {
			int starid = gfx.makesprite( starimageid, 1, 1 );
			stars_large.push_back( starid );
			auto& spr = gfx.getsprite( starid );
			spr.src.x = 1;
			spr.pos.y = rand() % SCENEH;
			spr.pos.x = 2 + ( rand() % (SCENEW - 4) );
		}
	}

	void update() {
		starsmalldy += STAR_SMALL_SPEED;
		starlargedy += STAR_LARGE_SPEED;
		for (int starid : stars_small) {
			auto& starspr = gfx.getsprite( starid );
			starspr.pos.y = ( starspr.pos.y + int(starsmalldy) ) % SCENEH;
		}
		for (int starid : stars_large) {
			auto& starspr = gfx.getsprite( starid );
			starspr.pos.y = ( starspr.pos.y + int(starlargedy) ) % SCENEH;
		}
		starsmalldy -= int(starsmalldy);
		starlargedy -= int(starlargedy);
	}

	void drawscene() {
		gfx.drawscene();
	}
};


// Enemys
struct Enemy {
	static const int TSIZE = Scene::TSIZE;
	GFX::Scene* gfx;
	int spriteid, alive;
};

struct EnemySentry : Enemy {
	static const int ENEMY_SPEED = 1, ANIM_TT = 30;
	int animtt = 0, anim = 0;

	void init() {
		if (!gfx) return;
		spriteid = gfx->makesprite( Scene::tilesetimage, TSIZE, TSIZE );
		auto& enemy = gfx->getsprite( spriteid );
		enemy.src.x = TSIZE * 4;
		enemy.pos.x = 2 + ( rand() % (Scene::SCENEW - TSIZE - 4) );
		enemy.pos.y = -TSIZE;
		alive = true;
	}

	void free() {
		if (!gfx) return;
		gfx->freesprite( spriteid );
	}

	void animate(GFX::Sprite& enemy) {
		animtt++;
		if (animtt >= ANIM_TT) {
			anim = (anim + 1) % 2;
			enemy.src.x = TSIZE * ( 4 + anim );
			animtt = 0;
		}
	}

	void move(GFX::Sprite& enemy) {
		enemy.pos.y += ENEMY_SPEED;
	}

	void update() {
		if (!gfx) return;
		auto& enemy = gfx->getsprite( spriteid );
		animate( enemy );
		move( enemy );
		if (enemy.pos.y > Scene::SCENEH)
			alive = false;
	}
};

struct EnemySentryWobble : EnemySentry {
	double x = 0, y = 0, xspeed = 0, yspeed = 1, xacc = 0.1;

	void init() {
		EnemySentry::init();
		if (!gfx) return;
		auto& enemy = gfx->getsprite( spriteid );
		x = enemy.pos.x;
		y = enemy.pos.y;
	}

	void move(GFX::Sprite& enemy) {
		xspeed += xacc;
		if (abs(xspeed) >= 1.0)  xspeed = -xspeed;
		x += xspeed;
		y += yspeed;
		enemy.pos.x = round(x);
		enemy.pos.y = round(y);
	}

	void update() {
		EnemySentry::update();
	}
};


// Main Game scene 
struct SceneGame : Scene {
	// scene data
	GFX::Scene gfx;
	int shipspriteid = 0;
	// bullet data
	const int BULLET_CD = 7, BULLET_SPEED = 2;
	vector<int> bullets;
	int bulletcd = 0;
	// enemys
	map<int, EnemySentryWobble> sentrys;
	// interface
	UIF uif;
	StarField starfield;

	void init() {
		shipspriteid = gfx.makesprite( tilesetimage, TSIZE, TSIZE );
		auto& ship = gfx.getsprite( shipspriteid );
		ship.pos.x = (SCENEW - ship.pos.w) / 2;
		ship.pos.y = SCENEH - 16;
		uif.init();
		starfield.init();
	}

	void update() {
		starfield.update();

		// move ship
		auto& ship = gfx.getsprite( shipspriteid );
		ship.z = 101;
		ship.pos.x = max( 1, min( SCENEW - ship.pos.w - 1, ship.pos.x + dpad.xaxis ) );

		// move bullets
		for (int i = bullets.size() - 1; i >= 0; i--) {
			auto& bullet = gfx.getsprite( bullets[i] );
			bullet.pos.y -= BULLET_SPEED;
			// offscreen collision
			int collide = bullet.pos.y < -TSIZE;
			// enemy collision
			if (gfx.collidesprite( bullet )) {
				collide++;
				for (int c : gfx.collisions_sprite)
					if (sentrys.count(c)) {
						sentrys[c].alive = 0;
						break;
					}
			}
			// erase collided bullets
			if (collide) {
				gfx.freesprite( bullets[i] );
				bullets.erase( bullets.begin() + i );
			}
		}

		// spawn bullets
		bulletcd = max( bulletcd - 1, 0 );
		if (bulletcd == 0 && dpad.a > 0) {
			bullets.push_back( gfx.makesprite( tilesetimage, 4, TSIZE ) );
			auto& bullet = gfx.getsprite( bullets.back() );
			bullet.src.x = TSIZE * 2;
			bullet.pos.x = ship.pos.x + 2;
			bullet.pos.y = ship.pos.y - TSIZE - 1;
			bullet.z = 100;
			bulletcd = BULLET_CD;
		}

		// move enemys
		vector<int> dead;
		for (auto& [id, sentry] : sentrys) {
			sentry.update();
			if ( !sentry.alive )
				dead.push_back( id );
		}
		for (int id : dead) {
			sentrys[id].free();
			sentrys.erase( id );
		}

		// spawn enemys
		if (sentrys.size() < 10 && rand() % 10 == 0) {
			EnemySentryWobble enemy = { &gfx };
			enemy.init();
			sentrys[enemy.spriteid] = enemy;
		}
	}

	void drawscene() {
		starfield.drawscene();
		gfx.drawscene();
		uif.drawscene();
	}
};

extern SceneGame game;