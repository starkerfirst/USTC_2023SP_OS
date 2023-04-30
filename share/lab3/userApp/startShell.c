#include "io.h"
#include "myPrintk.h"
#include "uart.h"
#include "vga.h"
#include "i8253.h"
#include "i8259A.h"
#include "tick.h"
#include "wallClock.h"

typedef struct myCommand {
    char name[80];
    char help_content[200];
    int (*func)(int argc, char (*argv)[8]);
} myCommand; 

int strcmp(const char* a, const char* b) {
	while ((*a != 0) && (*b != 0)) {
		if ((*a) != (*b)) return 1; // False
		a++;
		b++;
	}
	return 0; // True
}

int func_cmd(int argc, char (*argv)[8]);
myCommand cmd={"cmd\0","List all command\n\0",func_cmd};

int func_help(int argc, char (*argv)[8]);
myCommand help={"help\0","Usage: help [command]\n\0Display info about [command]\n\0",func_help};
int func_help(int argc, char (*argv)[8]){
    // print help info
    if (argc == 1) {
        myPrintk(0x07,"help\0");
        myPrintk(0x07," -> ");
        myPrintk(0x07,"List all command\n\0");
        myPrintk(0x07,"help [command]\0");
        myPrintk(0x07," -> ");
        myPrintk(0x07,"Display info about [command]\n\0");
        return 0;
    }
    for (int i = 0; i < 2; i++) {
        if (strcmp(argv[1], cmd.name) == 0) {
            myPrintk(0x07,cmd.name);
            myPrintk(0x07," -> ");
            myPrintk(0x07,cmd.help_content);
            return 0;
        }
        if (strcmp(argv[1], help.name) == 0) {
            myPrintk(0x07,help.name);
            myPrintk(0x07," -> ");
            myPrintk(0x07,help.help_content);
            return 0;
        }
    }
}

int func_cmd(int argc, char (*argv)[8]){
	// list all cmds
    // cmd
    myPrintk(0x07,cmd.name);
    myPrintk(0x07," -> ");
    myPrintk(0x07,cmd.help_content);

    // help
    myPrintk(0x07,help.name);
    myPrintk(0x07," -> ");
    myPrintk(0x07,help.help_content);
    
    return 0;
} 


void startShell(void){
//我们通过串口来实现数据的输入
char BUF[256]; //输入缓存区
int BUF_len=0;	//输入缓存区的长度
    
	int argc;
    char argv[8][8];

    do{
        BUF_len=0; 
        myPrintk(0x07,"Student>>\0");
        while((BUF[BUF_len]=uart_get_char())!='\r'){
            uart_put_char(BUF[BUF_len]);//将串口输入的数存入BUF数组中
            BUF_len++;  //BUF数组的长度加
        }
        uart_put_chars(" -PB20000328 ybh\0");
        uart_put_char('\n');

        BUF[BUF_len]=0; 

        // parse command
        argc = 0;
        int i = 0;
        while(BUF[i]!=0){
            if(BUF[i]==' ') {i++; continue;}
            if(BUF[i]==0) break;
            int j=0;
            while(BUF[i] != ' ' && BUF[i] != 0){
                argv[argc][j] = BUF[i];
                i++; 
                j++;
            }
            argv[argc][j] = 0;
            argc++;
        }

        // execute command
        if (strcmp(argv[0], cmd.name) == 0) {
            cmd.func(argc, argv);
        } else if (strcmp(argv[0], help.name) == 0) {
            help.func(argc, argv);
        } else {
            myPrintk(0x07,"Command not found!\n\0");
        }

    }while(1);

}

