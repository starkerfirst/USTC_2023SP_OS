#include "wallClock.h"
int system_ticks;
int HH,MM,SS;

void tick(void){
	system_ticks++;

	// 100 ticks for one second
	if (system_ticks % 100 != 0){
		return;
	}

	int second = system_ticks / 100;

	// update
	HH = second / 3600;
	MM = (second % 3600) / 60;
	SS = second % 60;
	
	setWallClock(HH,MM,SS);
	return;
}

