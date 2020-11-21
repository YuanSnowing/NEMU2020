#ifndef __MISC_H__
#define __MISC_H__

extern Gate_info *idt_desc;

make_helper(nop);
make_helper(int3);
make_helper(lea);
make_helper(std);
make_helper(cld);

make_helper(intr);
make_helper(iret);
make_helper(cli);

make_helper(hlt);
#endif
