#include <setjmp.h>
jmp_buf jbuf;
void raise_intr(uint8_t NO){
	/* TODO: Trigger an interrupt/exception with NO
	 * use NO to index the IDT
	 */

     
    /* Jump back to cpu_exec() */
    longjmp(jbuf, 1);
}