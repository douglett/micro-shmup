#pragma once
#include <vector>
using namespace std;

struct DPad {
	enum KEYSTATE {
		KUP = -1,
		KRELEASED = 0,
		KPRESSED = 1,
		KDOWN = 2,
	};
	static const char BUTTON_A = 'Z', BUTTON_B = 'X';
	int u=0, d=0, l=0, r=0, a=0, b=0;
	int xaxis=0, yaxis=0;
	char str[100] = {0};

	void normalize() {
		// reset keyup/keydown state on keys
		vector<int*> buttons = { &u, &d, &l, &r, &a, &b };
		for (auto b : buttons)
			if      (*b == KDOWN) *b = KPRESSED;
			else if (*b == KUP)   *b = KRELEASED;
	}

	void setaxis() {
		// handle d-pad axis
		if (u ==  2)  yaxis = -1;
		if (d ==  2)  yaxis =  1;
		if (l ==  2)  xaxis = -1;
		if (r ==  2)  xaxis =  1;
		if (u == -1)  yaxis = d > 0 ?  1 : 0;
		if (d == -1)  yaxis = u > 0 ? -1 : 0;
		if (l == -1)  xaxis = r > 0 ?  1 : 0;
		if (r == -1)  xaxis = l > 0 ? -1 : 0;
	}

	const char* getstate1() {
		snprintf( str, 100, "%02d %02d %02d %02d - %02d %02d - %c %c - %c %c", 
			u, r, d, l, xaxis, yaxis,
			a ? 'A' : '_',
			b ? 'B' : '_',
			xaxis < 0 ? 17 : xaxis > 0 ? 16 : '.',
			yaxis < 0 ? 30 : yaxis > 0 ? 31 : '.'
			);
		return str;
	}

	const char* getstate2() {
		snprintf( str, 100, "%c %c  %c %c", 
			xaxis < 0 ? 17 : xaxis > 0 ? 16 : '.',
			yaxis < 0 ? 30 : yaxis > 0 ? 31 : '.',
			a ? 'A' : '_',
			b ? 'B' : '_'
			);
		return str;
	}
};