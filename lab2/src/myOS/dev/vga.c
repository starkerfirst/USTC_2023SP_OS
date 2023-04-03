/*
 * 本文件实现vga的相关功能，清屏和屏幕输出
 * clear_screen和append2screen必须按照如下借口实现
 * 可以增加其他函数供clear_screen和append2screen调用
 */
extern void outb (unsigned short int port_to, unsigned char value);
extern unsigned char inb(unsigned short int port_from);
extern void uart_put_chars(char *str);
//VGA字符界面规格：25行80列
//VGA显存初始地址为0xB8000

short cur_line=0;
short cur_column=0;//当前光标位置
char * vga_init_p=(char *)0xB8000;


void update_cursor(void){//通过当前行值cur_cline与列值cur_column回写光标
	//use global variable cur_line and cur_column tp update the current cursor position
	//here, we will use the inb and outb to interact with port = 0x3D4 and port = 0x3D5 

	// compute h and l 8 bits
	unsigned short pos = cur_line * 80 + cur_column;
	unsigned char h = pos >> 8;
	unsigned char l = pos & 0x00FF;

	// update two regs
	outb(0x3d4, 0x0E);
	outb(0x3d5,h);
	outb(0x3d4, 0x0F);
	outb(0x3d5,l);

}

short get_cursor_position(void){//获得当前光标，计算出cur_line和cur_column的值
	//update global variable cur_line and cur_column using the information from port = 0x3D4 and port = 0x3D5
	//here, we will use the inb and outb to interact with port = 0x3D4 and port = 0x3D5

	// high 8 bits
	outb(0x3d4, 0x0E);
	unsigned char h = inb(0x3d5);

	// low 8 bits
	outb(0x3d4, 0x0F);
	unsigned char l = inb(0x3d5);

	//compute pos
	unsigned short pos = (unsigned short)h << 8 | l;
	// cur_line = pos / 80;
	// cur_column = pos % 80;
	return pos;

}


void clear_screen(void) {
	//clean the whole VGA screen 
	cur_column = 0;
	cur_line = 0;
	update_cursor();
	unsigned short* ptr = vga_init_p;
	for(int i = 0; i < 25; i++)
		for(int j = 0; j < 80; j++)
		{
			*ptr = 0x0720; // we must use space to cover the screen instead of /0
			ptr++;
		}
}   

void append2screen(char *str,int color){ 
	//Send the str to the current cursor position 
	// char a[] = "123";
	// str = a;

	for(int i = 0; str[i] != 0; i++)
		{
			if(str[i]=='\n'){
				if(cur_line+1==25){
					clear_screen();
					cur_line=0;
					cur_column=0;
					update_cursor();
				}
				else{
					cur_line+=1;
					cur_column=0;
					update_cursor();
				}	
			}
			else if(cur_column+1==80){
				if(cur_line+1==25){
					clear_screen();
					cur_line=0;
					cur_column=1;
					update_cursor();
					short pos = get_cursor_position();
					*(vga_init_p+pos*2-1) = (char) color;
					*(vga_init_p+pos*2-2) = str[i];
				}
				else{
					cur_line+=1;
					cur_column=1;
					update_cursor();
					short pos = get_cursor_position();
					*(vga_init_p+pos*2-1) = (char) color;
					*(vga_init_p+pos*2-2) = str[i];
				}
				
			}
			else
			{
				cur_column+=1;
				update_cursor();
				short pos = get_cursor_position();
				*(vga_init_p+pos*2-1) = (char) color;
				*(vga_init_p+pos*2-2) = str[i];
			}
		}    
}


