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
	WP *tmp = head;
	if(tmp == wp){
		head = wp->next;
	}else while(tmp != NULL){
		if(tmp->next == wp){
			tmp->next = tmp->next->next;
			break;
		}
		tmp = tmp->next;
	}
	printf("Delete watchpoint No.%d at %s, current value is %d.\n",wp->NO, wp->exp, wp->value);
	wp->value = 0;
	wp->exp[0] = '\0';
	wp->next = free_->next;
	free_ = wp;
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
		printf("No.%d\texpression: %s\tvalues: %d\n",tmp->NO,tmp->exp,tmp->value);
		tot ++;
		tmp = tmp->next;
	}
	printf("Total: %d\n",tot);
}

bool wp_check(){
	WP *tmp = head;
	uint32_t val;
	bool success=true, ret=false;
	while(tmp != NULL){
		val = expr(tmp->exp,&success);
		if(val != tmp->value){
			ret = true;
			printf("No.%d\texpression: %s\tvalue: 0x%x\tchanged to 0x%x\n",tmp->NO,tmp->exp,tmp->value,val);
			tmp->value = val;
		}
		tmp = tmp->next;
	}
	return ret;
}
