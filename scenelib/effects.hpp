#pragma once
#include "gfx.hpp"
#include <algorithm>
using namespace std;

struct Effects {
	struct Fade;

	static inline double limit(double minv, double x, double maxv) {
		return min( maxv, max( x, minv ) );
	}
};

struct Effects::Fade {
	// fade in/out by float value (0.0 - 1.0)
	static void fade(double fadeval) {
		fade( GFX::screen, fadeval );
	}
	static void fade(GFX::Image& img, double fadeval) {
		fadeval = limit( 0.0, fadeval, 1.0 );
		for (int i = 0; i < (int)img.data.size(); i++) {
			uint8_t* data = (uint8_t*)&img.data[i];
			// data[3] ;  // ignore alpha
			data[2] *= fadeval;  // red
			data[1] *= fadeval;  // green
			data[0] *= fadeval;  // blue
		}
	}

	// multiply colors by color value
	static void multiply(uint32_t col) {
		multiply( GFX::screen, col );
	}
	static void multiply(GFX::Image& img, uint32_t col) {
		uint8_t* colors = (uint8_t*)&col;
		for (int i = 0; i < (int)img.data.size(); i++) {
			uint8_t* data = (uint8_t*)&img.data[i];
			// data[3] ;  // ignore alpha
			data[2] *= colors[2] / 255.0;  // red
			data[1] *= colors[1] / 255.0;  // green
			data[0] *= colors[0] / 255.0;  // blue
		}
	}
};