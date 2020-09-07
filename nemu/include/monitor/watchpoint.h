#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	char exp[32];
	int value;
	/* TODO: Add more members if necessary */


} WP;

#endif
