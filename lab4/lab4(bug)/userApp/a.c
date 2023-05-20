// shell.c --- malloc version
#include "../myOS/userInterface.h"
#include "../userApp/memTestCase.h"
#include "../myOS/include/kmalloc.h"

#define NULL (void *)0
#define SHUTDOWN_LABEL 0x12345678

int getCmdline(unsigned char *buf, int limit) {
	unsigned char *ptr = buf;
	int n = 0;
	while (n < limit) {
		*ptr = uart_get_char();
		if (*ptr == 0xd) {
			*ptr++ = '\n';
			*ptr = '\0';
			uart_put_char('\r');
			uart_put_char('\n');
			return n + 2;
		}
		if (*ptr == 127) {
			if(n>0){
				*ptr = 0;
				ptr--;
				*ptr = 0;
				n--;
				uart_put_char('\b');
				uart_put_char(' ');
				uart_put_char('\b');
			}
			continue;
		}
		uart_put_char(*ptr);
		ptr++;
		n++;
	}

	return n;
}

typedef struct cmd {
	unsigned char cmd[20 + 1];  // TODO: dynamic
	int (*func)(int argc, unsigned char **argv);
	void (*help_func)(void);
	unsigned char description[100 + 1];  // TODO: dynamic?
	struct cmd *nextCmd;
} cmd;

#define cmd_size sizeof(struct cmd)

struct cmd *ourCmds = NULL;

int listCmds(int argc, unsigned char **argv) {
	struct cmd *tmpCmd = ourCmds;
	myPrintf(0x7, "list all registered commands:\n");
	myPrintf(0x7, "command name: description\n");

	while (tmpCmd != NULL) {
		myPrintf(0x7, "% 12s: %s\n", tmpCmd->cmd, tmpCmd->description);
		tmpCmd = tmpCmd->nextCmd;
	}
	return 0;
}

int strcpy_s(const unsigned char *const source, unsigned char *const dest,
			 unsigned int limit) {
	for (int n = 0; n < limit; n++) {
		dest[n] = source[n];
		if (source[n] == 0) {
			break;
		}
		if (n == limit - 1) {
			dest[n] = 0;
			return -1;
		}
	}
	return 0;
}

void addNewCmd(unsigned char *_cmd, int (*func)(int argc, unsigned char **argv),
			   void (*help_func)(void), unsigned char *description) {
	//本函数需要实现！！！
	/*功能：增加命令
	1.使用malloc创建一个cm的结构体，新增命令。
	2.同时还需要维护一个表头为ourCmds的链表。
	*/

	// init new cmd
	cmd *newcmd = (cmd *)kmalloc(cmd_size);
	if (newcmd == 0) return;
	strcpy_s(_cmd, newcmd->cmd, 21);
	strcpy_s(description, newcmd->description, 101);
	newcmd->func = func;
	newcmd->help_func = help_func;
	newcmd->nextCmd = 0;

	if (ourCmds == NULL)
		ourCmds = newcmd;
	else {
		cmd *pcmd = ourCmds;
		while (pcmd->nextCmd != 0) pcmd = pcmd->nextCmd;
		pcmd->nextCmd = newcmd;
	}
	return;
}

void help_help(void) { myPrintf(0x7, "USAGE: help [cmd]\n\n"); }

int help(int argc, unsigned char **argv) {
	int i;
	struct cmd *tmpCmd = ourCmds;
	help_help();

	if (argc == 1) return listCmds(argc, argv);
	if (argc > 2) return 1;

	while (tmpCmd != NULL) {
		if (strcmp(argv[1], tmpCmd->cmd) == 0) {
			if (tmpCmd->help_func != NULL)
				tmpCmd->help_func();
			else
				myPrintf(0x7, "%s\n", tmpCmd->description);
			break;
		}
		tmpCmd = tmpCmd->nextCmd;
	}
	return 0;
}

struct cmd *findCmd(unsigned char *cmd) {
	struct cmd *tmpCmd = ourCmds;
	int found = 0;
	while (tmpCmd != NULL) {  // at lease 2 cmds
		if (strcmp(cmd, tmpCmd->cmd) == 0) {
			found = 1;
			break;
		}
		tmpCmd = tmpCmd->nextCmd;
	}
	return found ? tmpCmd : NULL;
}

int split2Words(unsigned char *cmdline, unsigned char **argv, int limit) {
	unsigned char c, *ptr = cmdline;
	int argc = 0;
	int inAWord = 0;

	while (c = *ptr) {  // not '\0'
		if (argc >= limit) {
			myPrintf(0x7, "cmdline is tooooo long\n");
			break;
		}
		switch (c) {
			case ' ':
				*ptr = '\0';
				inAWord = 0;
				break;  // skip white space
			case '\n':
				*ptr = '\0';
				inAWord = 0;
				break;  // end of cmdline?
			default:    // a word
				if (!inAWord) *(argv + argc++) = ptr;
				inAWord = 1;
				break;
		}
		ptr++;
	}
	return argc;
}

int shutdown(int argc,unsigned char*argv[]){
	if(argc==1)return SHUTDOWN_LABEL;
	else myPrintf(0x07,"too many arguments!");
}

void shutdown_help(void) { myPrintf(0x7, "USAGE: shutdown\n\n"); }

void testMem_help(void) { myPrintf(0x7, "USAGE: testMem -Kernel\n\n"); }

int testMem(int argc,unsigned char*argv[]){
	if(argc!=2){
		myPrintf(0x07,"illegal args!");
	}
	if(strcmp(argv[1],"-Kernel")==0||strcmp(argv[1],"-K")==0){
		kMemtest(argc,argv);
		return 0;
	}
	myPrintf(0x07,"illegal args!");
}

void initShell(void) {
	addNewCmd("cmd\0", listCmds, NULL, "list all registered commands\0");
	addNewCmd("help\0", help, help_help, "help [cmd]\0");
	addNewCmd("shutdown\0",shutdown,shutdown_help,"shut down the shell");
	addNewCmd("testMem\0",testMem,testMem_help,"test Kernel Memory as testdP2");
	// TODO: may be we can add a new command exit or quit
}

unsigned char cmdline[100];
void startShell(void) {
	unsigned char *argv[10];  // max 10
	int argc;
	struct cmd *tmpCmd;
	// myPrintf(0x7,"StartShell:\n");

	while (1) {
		myPrintf(0x3, "Student >:");
		getCmdline(&cmdline[0], 100);
		myPrintf_to_VGA(0x7, cmdline);

		argc = split2Words(cmdline, &argv[0], 10);
		if (argc == 0) continue;

		tmpCmd = findCmd(argv[0]);
		if (tmpCmd){
			if(tmpCmd->func(argc, argv)==SHUTDOWN_LABEL)return;
		}
		else
			myPrintf(0x7, "UNKNOWN command: %s\n", argv[0]);
		myPrintf(0x7,"\n");
	}
}