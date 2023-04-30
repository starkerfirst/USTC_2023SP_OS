
extern unsigned char inb(unsigned short int port_from);
extern void outb (unsigned short int port_to, unsigned char value);

#define uart_base 0x3F8

void uart_put_char(unsigned char c){
	unsigned short int port_to = uart_base;
	outb(port_to, c);
}

unsigned char uart_get_char(void){
	unsigned char c;
	unsigned short int port_from = uart_base;
	c = inb(port_from);    
	return c;     
}

void uart_put_chars(char *str){ 
	for(int i = 0; str[i] != 0; i++)
		{
			uart_put_char(str[i]);
			if(str[i]=='\n') uart_put_char('\r');

		}    
}