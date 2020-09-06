#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args){
	int N=1;
	if(args != NULL){
		sscanf(args,"%d",&N);
	}
	cpu_exec(N);
	return 0;
}

static int cmd_info(char *args){
	if(args == NULL){
		printf("parameter invalid!\n");
		return 0;
	}
	if(args[0] == 'r'){
		int i=0;
		for(; i<8;++i){
			printf("$%s: %8x\n", regsl[i],reg_l(i));
		}
		printf("$eip: %x\n",cpu.eip);
	}else if(args[0] == 'w'){
		return 0;
	}else{
		printf("parameter invalid!\n");
	}
	return 0;
}

static int cmd_p(char* args){
	if(args == NULL){
		printf("parameter invalid!\n");
		return 0;
	}
	bool success = false;
	int ans = expr(args, &success);
	if(success){
		printf("%d\n",ans);
	}else{
		printf("invalid RE!\n");
	}
	return 0;
}

static int cmd_x(char *args){
	if(args == NULL){
		printf("parameter invalid!\n");
		return 0;
	}
	int N,start,i;
	//char expr[32];
	sscanf(args, "%d%x", &N, &start);
	for(i = 0; i< N; ++ i){
		printf("0x%x :  %x %x %x %x\n",start,swaddr_read(start, 1), swaddr_read(start+1, 1),
				swaddr_read(start+2, 1), swaddr_read(start+3, 1));
	}
	return 0;
}

static int cmd_w(char *args){
	if(args == NULL){
		printf("parameter invalid!\n");
		return 0;
	}
	return 0;
}

static int cmd_d(char *args){
	if(args == NULL){
		printf("parameter invalid!\n");
		return 0;
	}
	return 0;
}

static int cmd_bt(char *args){
	if(args == NULL){
		printf("parameter invalid!\n");
		return 0;
	}
	return 0;
}

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Step instruction for N=1", cmd_si},
	{ "info", "info of regs or watchpoints", cmd_info},
	{ "p", "Calculate the value of the RE", cmd_p},
	{ "x", "scan the memory, address at expr", cmd_x},
	{ "w", "set watchpoint, pause when expr's value changes", cmd_w},
	{ "d", "delete watchpoint numbered N", cmd_d},
	{ "bt", "print the stack link", cmd_bt},
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
