#pragma once
#include "global.hpp"
#include <cmath>

struct Wave {
	enum ENEMY_T {
		ENEMY_ORB = 200,
		ENEMY_ORB_WOBBLE,
		ENEMY_ORB_YELLOW,
		ENEMY_ALIEN_GREEN = 210,
		ENEMY_ALIEN_RED,
		ENEMY_EXPLOSION = 300
	};
	enum ENEMY_SPRITE {
		SPRITE_ORB_BLUE = 4,
		SPRITE_ORB_YELLOW = 6,
		SPRITE_ALIEN_GREEN = 8,
		SPRITE_ALIEN_RED = 10,
		SPRITE_EXPLOSION = 12,
	};
	struct EnemyData {
		int framebase, frameoffset, delta, health;
		double xspeed, yspeed, xacc;
	};

	static const int TSIZE = Scene::TSIZE;
	GFX::Scene& gfx;
	vector<int> enemys;
	int delta = 0, wave = 1;

	void reset() {
		// clear enemys
		for (int e : enemys)
			gfx.freesprite( e );
		enemys = {};
		// reset counters
		delta = 0, wave = 1;
	}

	void update() {
		delta++;
		// move enemys
		for (int i = (int)enemys.size()-1; i >= 0; i--)
			if (updateenemy( enemys[i] ))
				enemys.erase( enemys.begin() + i );
	}

	int kill(int enemyid) {
		auto it = find( enemys.begin(), enemys.end(), enemyid );
		if (it == enemys.end())  return 0;
		// update enemy health
		auto& enemy = gfx.getsprite( enemyid );
		auto& data = (EnemyData&)enemy.userdata[0];
		data.health--;
		if (data.health > 0)  return 0;
		// erase
		int type = enemy.usertype, x = enemy.pos.x, y = enemy.pos.y;
		gfx.freesprite( enemyid );
		enemys.erase( it );
		// spawn explosion
		if (type != ENEMY_EXPLOSION)
			makeexplosion( x, y );
		return type;
	}

	int makeorb(int x) {
		int enemyid = gfx.makesprite( Scene::tilesetimage, TSIZE, TSIZE );
		auto& enemy = gfx.getsprite( enemyid );
		enemy.pos.x = x;
		enemy.pos.y = -TSIZE;
		enemy.z = 10;
		// enemy data
		enemy.usertype = ENEMY_ORB;
		enemy.userdata.resize( sizeof(EnemyData) );
		auto& data = (EnemyData&)enemy.userdata[0];
		data.health = wave;
		data.yspeed = 1;
		data.framebase = SPRITE_ORB_BLUE;
		enemy.src.x = TSIZE * data.framebase;  // first frame
		enemys.push_back( enemyid );
		return enemyid;
	}

	int makewobbleorb(int x) {
		int enemyid = makeorb( x );
		auto& enemy = gfx.getsprite( enemyid );
		// enemy data
		enemy.usertype = ENEMY_ORB_WOBBLE;
		auto& data = (EnemyData&)enemy.userdata[0];
		data.xacc = 0.07;
		return enemyid;
	}

	int makegreenalien(int x) {
		int enemyid = makeorb( x );
		auto& enemy = gfx.getsprite( enemyid );
		auto& data = (EnemyData&)enemy.userdata[0];
		// enemy data
		enemy.usertype = ENEMY_ALIEN_GREEN;
		data.framebase = SPRITE_ALIEN_GREEN;
		return enemyid;
	}

	int makezigzag(int x) {
		int enemyid = makeorb( x );
		auto& enemy = gfx.getsprite( enemyid );
		auto& data = (EnemyData&)enemy.userdata[0];
		// set as yellow orb
		enemy.usertype = ENEMY_ORB_YELLOW;
		data.framebase = SPRITE_ORB_YELLOW;
		data.xspeed = 1;
		data.yspeed = 0.5;
		return enemyid;
	}

	int makepowerup(int x) {
		int enemyid = makeorb( x );
		auto& enemy = gfx.getsprite( enemyid );
		auto& data = (EnemyData&)enemy.userdata[0];
		// set as yellow orb
		enemy.usertype = ENEMY_ALIEN_RED;
		data.framebase = SPRITE_ALIEN_RED;
		data.xspeed = 2;
		data.yspeed = 0.5;
		return enemyid;
	}

	int makeexplosion(int x, int y) {
		int enemyid = gfx.makesprite( Scene::tilesetimage, TSIZE, TSIZE );
		auto& enemy = gfx.getsprite( enemyid );
		enemy.userdata.resize( sizeof(EnemyData) );
		auto& data = (EnemyData&)enemy.userdata[0];
		enemy.pos.x = x;
		enemy.pos.y = y;
		enemy.z = 1;
		enemy.hit = enemy.hurt = { 0 };
		enemy.usertype = ENEMY_EXPLOSION;
		data.framebase = SPRITE_EXPLOSION;
		data.health = 1;
		enemys.push_back( enemyid );
		return enemyid;
	}

	bool updateenemy(int enemyid) {
		auto& enemy = gfx.getsprite( enemyid );
		// animate between two frames
		auto& data = (EnemyData&)enemy.userdata[0];
		data.delta++;
		if (enemy.usertype != ENEMY_EXPLOSION && data.delta >= 30) {
			data.frameoffset = (data.frameoffset + 1) % 2;
			data.delta = 0;
		}
		enemy.src.x = TSIZE * (data.framebase + data.frameoffset);  // update animation source

		switch ( (ENEMY_T) enemy.usertype ) {
			// straight down orb
			case ENEMY_ORB:
				enemy.pos.y += data.yspeed;
				break;
			// wobble orb
			case ENEMY_ORB_WOBBLE:
				data.xspeed += data.xacc;
				if (abs(data.xspeed) >= 1.0)  data.xacc = -data.xacc;
				enemy.pos.x += data.xspeed;
				enemy.pos.y += data.yspeed;
				break;
			// green alien - go straight down then into a half-loop and return
			case ENEMY_ALIEN_GREEN:
				if (enemy.pos.y > 116) {
					data.yspeed -= 0.02;
					if (enemy.pos.x < (Scene::SCENEW - TSIZE) / 2) 
						data.xspeed += 0.02;
					else
						data.xspeed = max( data.xspeed - 0.02, 0.0 );
				}
				enemy.pos.x += data.xspeed;
				enemy.pos.y += data.yspeed;
				if (enemy.pos.y < -TSIZE * 2)
					return true;
				break;
			// zig-zag - go back and forth
			case ENEMY_ORB_YELLOW:
				if (enemy.pos.x <= 0 || enemy.pos.x >= Scene::SCENEW - TSIZE)
					data.xspeed = -data.xspeed;
				enemy.pos.x += data.xspeed;
				enemy.pos.y += data.yspeed;
				break;
			// power-up - fly all over the place
			case ENEMY_ALIEN_RED:
				if (enemy.pos.x <= 0)
					data.xspeed = 1;
				else if (enemy.pos.x >= Scene::SCENEW - TSIZE)
					data.xspeed = -1;
				else if (rand() % 5 == 0)
					data.xspeed = -data.xspeed;
				enemy.pos.x += data.xspeed;
				enemy.pos.y += data.yspeed;
				break;
			// explosion animation - play 3 frames then delete object
			case ENEMY_EXPLOSION:
				if (data.delta >= 15) {
					data.frameoffset = data.frameoffset + 1;
					data.delta = 0;
					if (data.frameoffset >= 3) {
						kill( enemyid );
					}
				}
				break;
		}

		// check offscreen bottom, return true if dead
		if (enemy.pos.y > Scene::SCENEH)
			return true;
		return false;
	}
};


struct Wave1 : Wave {
	void update() {
		Wave::update();
		int start = 1;

		// 2x rows of straight down orbs
		start = 1;
		for (int frameoffset : vector<int>{ 0, 15, 30, 45, 60 })
			if ( delta == start + frameoffset )
				makeorb( 15 );
		for (int frameoffset : vector<int>{ 0, 15, 30, 45, 60 })
			if ( delta == start + 50 + frameoffset )
				makeorb( Scene::SCENEW - TSIZE - 15 );

		// zig-zag orbs
		start = 150;
		for (int frameoffset : vector<int>{ 0, 15, 30, 45 })
			if ( delta == start + frameoffset )
				makezigzag( 15 );

		// powerup alien
		start = 300;
		if ( delta == start )
			makepowerup( rand() % (Scene::SCENEW - TSIZE) );

		// wobbling orbs
		start = 450;
		for (int frameoffset : vector<int>{ 0, 20, 40, 60, 80, 100, 120 })
			if ( delta == start + frameoffset )
				makewobbleorb( (Scene::SCENEW - TSIZE) / 2 - 5 );

		// green aliens - out then boomerang back
		start = 550;
		for (int frameoffset : vector<int>{ 0, 15, 30 })
			if ( delta == start + frameoffset )
				makegreenalien( 15 );

		// reset
		if (delta >= 800) {
			delta = 0;
			wave++;
		}
	}
};