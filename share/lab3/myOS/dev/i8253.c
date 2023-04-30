#include "io.h"

void init8253(void){
	outb(0x0043, 0x34);
	// parameter: 11932
	outb(0x0040, 0x9C);
	outb(0X0040, 0X2E);
}
