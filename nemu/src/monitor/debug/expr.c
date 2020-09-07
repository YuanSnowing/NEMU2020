#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#define DEBUG
enum {
	NOTYPE = 256, EQ, HEX, NUM,REG,AND,OR,NOE,DER,NEG,

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"0x[0-9a-f]+$", HEX},
	{"[0-9]+", NUM},
	{"\\$(([a-z][a-z][a-z])|([a-z][a-z]))", REG},
	{"\\(", '('},
	{"\\)", ')'},
	{"\\+", '+'},					// plus
	{"-", '-'},
	{"\\*", '*'},
	{"/", '/'},
	{"==", EQ},						// equal
	{"!=", NOE},
	{"&&", AND},
	{"\\|\\|",OR},
	{"!",'!'},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	int value;
	char str[32];
} Token;

Token tokens[32];
int nr_token;
bool isdanmu(){
	if (nr_token == 0) return true;
	if(tokens[nr_token-1].type == NUM || tokens[nr_token-1].type == ')') return false;
	return true;
}
int getPri(int type){
	if(type == '(' || type == ')') return 1;
	if(type == '!' || type == NEG || type == DER) return 2;
	if(type == '/' || type == '*') return 3;
	if(type == '+' || type == '-') return 4;
	if(type == EQ || type == NOE) return 7;
	if(type == AND) return 11;
	if(type == OR) return 12;
}
static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				int rtt = rules[i].token_type;
				if(rtt == NOTYPE) continue;
				strncpy(tokens[nr_token].str,substr_start,substr_len);
				if(rtt == NUM){
					sscanf(tokens[nr_token].str,"%d",&tokens[nr_token].value);
					tokens[nr_token].type = NUM;
				}else if(rtt == HEX){
					sscanf(tokens[nr_token].str,"%x",&tokens[nr_token].value);
					tokens[nr_token].type = NUM;
				}else if(rtt == REG){
					int flag = 0, j;
					for(j = 0;j < 4; ++ j) tokens[nr_token].str[j] = tokens[nr_token].str[j+1];
#ifdef DEBUG
					printf("reg str: %s\n",tokens[nr_token].str);
#endif
					for(j = 0;j < 8; ++ j){
						if(strcmp(tokens[nr_token].str, regsl[j]) == 0)
							tokens[nr_token].value = reg_l(j), flag = 1;
						if(strcmp(tokens[nr_token].str, regsw[j]) == 0)
							tokens[nr_token].value = reg_w(j), flag = 1;
						if(strcmp(tokens[nr_token].str, regsb[j]) == 0)
							tokens[nr_token].value = reg_b(j), flag = 1;
					}
					if (!flag){
						printf("invalid register: $%s\n", tokens[nr_token].str);
						return false;
					}else tokens[nr_token].type = NUM;
				}else{
					if(rtt == '-' && isdanmu()) tokens[nr_token].type = NEG;
					else if(rtt == '*' && isdanmu()) tokens[nr_token].type = DER;
					else tokens[nr_token].type = rtt;
					tokens[nr_token].value = getPri(tokens[nr_token].type);
				}
				nr_token ++;
				break;
			}
		}
		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	return true; 
}

uint32_t expr(char *e, bool *success) {
	int i, len=strlen(e);
	for(i = 0;i < len;  ++ i){
		if(e[i] >= 'A' && e[i] <= 'Z') e[i] += 'a'-'A';
	}
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	eval(0,nr_token-1);

	return 0;
}

