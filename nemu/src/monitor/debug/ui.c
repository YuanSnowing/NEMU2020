#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <elf.h>
#define TESTargs if(args == NULL){ printf("parameter invalid!\n"); return 0; }
#define TESTsuccess if(!success) { printf("invalid RE!\n"); return 0; }
void cpu_exec(uint32_t);
hwaddr_t cmd_page_translate(lnaddr_t addr);
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
	TESTargs
	if(args[0] == 'r'){
		int i=0;
		for(; i<8;++i){
			printf("$%s: 0x%08x\n", regsl[i],reg_l(i));
		}
		printf("$eip: 0x%08x\n",cpu.eip);
	}else if(args[0] == 'w'){
		wp_print();
	}else{
		printf("parameter invalid!\n");
	}
	return 0;
}

static int cmd_p(char* args){
	TESTargs
	bool success = true;
	int ans = expr(args, &success);
	TESTsuccess
	printf("%d\n",ans);
	return 0;
}

static int cmd_x(char *args){
	TESTargs
	int N,start,i;
	char *exp = strtok(args, " ");
	sscanf(exp, "%d", &N);
	exp = exp + strlen(exp) + 1;
	bool success = true;
	start = expr(exp, &success);
	TESTsuccess
	for(i = start; i< N*4+start; i+= 4){
		printf("0x%08x: \t%02x %02x %02x %02x\n",i,swaddr_read(i, 1, R_DS), swaddr_read(i+1, 1, R_DS),
				swaddr_read(i+2, 1, R_DS), swaddr_read(i+3, 1, R_DS));
	}
	return 0;
}

static int cmd_w(char *args){
	TESTargs
	uint32_t val;
	bool success = true;
	val = expr(args, &success);
	TESTsuccess
	WP *wp = new_wp();
	if(wp == NULL) return 0;
	wp->value = val;
	strcpy(wp->exp, args);
	printf("Set watchpoint No.%d at %s, current value is %d.\n",wp->NO, wp->exp, wp->value);
	return 0;
}

static int cmd_d(char *args){
	TESTargs
	int N;
	sscanf(args,"%d",&N);
	WP *wp = wp_find(N);
	if(wp == NULL) return 0;
	free_wp(wp);

	return 0;
}
void getBt(swaddr_t eip, char* str);
static int cmd_bt(char *args){
	int cnt = 1;
	swaddr_t ebp = cpu.ebp, eip = cpu.eip;
	char str[100];
	for(;ebp; eip = swaddr_read(ebp+4, 4, R_SS),ebp = swaddr_read(ebp, 4, R_SS), ++ cnt){
		getBt(eip, str);
		if(str[0] == '\0') break;
		printf("#%d\t0x%08x:\t%s\targ1:0x%08x arg2:0x%08x arg3:0x%08x arg4:0x%08x\n", cnt, eip, str, 
			swaddr_read(ebp+8,4,R_SS), swaddr_read(ebp+12,4,R_SS), swaddr_read(ebp+16,4,R_SS), swaddr_read(ebp+20,4,R_SS));
	}
	return 0;
}


static int cmd_page(char* args){
	TESTargs
	uint32_t addr;
	sscanf(args, "%x", &addr);
	hwaddr_t ans = cmd_page_translate(addr);
	if(ans) printf("Addr is 0x%08x\n",ans);
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
	{ "page", "addr->page", cmd_page},
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
