#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(){
	if(free_ == NULL){
		printf("wp_pool is full!\n");
		return NULL;
	}
	WP *tmp = free_->next;
	free_->next = head;
	head = free_;
	free_ = tmp;
	return head;
}

void free_wp(WP *wp){
	WP *tmp = head, *pre = NULL;
	while(tmp != NULL){
		if(tmp == wp){
			pre->next = tmp->next;
			tmp->next = free_->next;
			free_ = tmp;
			return;
		}
		pre = tmp;
		tmp = tmp->next;
	}
	return;
}

WP *wp_find(int N){
	WP *tmp = head;
	while(tmp != NULL){
		if(tmp->NO == N) return tmp;
		tmp = tmp->next;
	}
	printf("Watchpoint No.%d is not using!\n",N);
	return NULL;
}

void wp_print(){
	int tot = 0;
	WP *tmp = head;
	while(tmp != NULL){
		printf("No.%d\texpression%s\tvalues%d\n",tmp->NO,tmp->exp,tmp->value);
		tot ++;
		tmp = tmp->next;
	}
	printf("Totla: %d\n",tot);
}
