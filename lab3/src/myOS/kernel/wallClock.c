#include "vga.h"
#define START_POS (80 * 24 + 72) // end line of the screen with 8 characters
#define COLOR 0x02

void setWallClock(int HH,int MM,int SS){
	unsigned short *p = (unsigned short *)VGA_BASE;
    p += (START_POS);

	// data buffer
    char buf[12] = {0};
    buf[0] = HH / 10 + '0';
    buf[1] = HH % 10 + '0';
    buf[2] = ':';
    buf[3] = MM / 10 + '0';
    buf[4] = MM % 10 + '0';
    buf[5] = ':';
    buf[6] = SS / 10 + '0';
    buf[7] = SS % 10 + '0';
    for (int i = 0; i < 8; i++) p[i] = (COLOR << 8) | (buf[i]);
    return;
}

void getWallClock(int *HH,int *MM,int *SS){
	unsigned short *p = (unsigned short *)VGA_BASE;
    p += (START_POS);

	// get 8 characters from vga buffer
    *HH = ((p[0] - '0') & 0x00FF) * 10 + (p[1] - '0') & 0x00FF;
    *MM = ((p[3] - '0') & 0x00FF) * 10 + (p[4] - '0') & 0x00FF;
    *SS = ((p[6] - '0') & 0x00FF) * 10 + (p[7] - '0') & 0x00FF;
    return;
}
