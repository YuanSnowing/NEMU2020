#ifndef __TLB_H__
#define __TLB_H__

#include "common.h"
/*
在 NEMU 中实现一个 TLB, 它的性质如下:
    TLB 总共有 64 项
    fully associative
    标志位只需要 valid bit 即可
    替换算法采用随机方式
你还需要在 restart()函数中对 TLB 进行初始化, 将所有 valid bit 置为无效即可。
在 page_translate()的实现中, 先查看 TLB, 如果命中, 则不需要进行 page
walk;如果未命中,则需要进行 page walk, 并填充 TLB。另外不要忘记, 更新
CR3 的时候需要强制冲刷 TLB 中的内容。由于 TLB 对程序来说是透明的, 所以
kernel 不需要为 TLB 的功能添加新的代码。
*/

#define TLB_SIZE 64

#define TLB_BIAS_BIT 12
#define TLB_BIAS_SIZE (1<<TLB_BIAS_BIT)
typedef struct{
    bool valid;
    uint32_t tag, data;
}TLB;
TLB tlb[TLB_SIZE];
void init_tlb();
hwaddr_t read_tlb(hwaddr_t addr);
void write_tlb(hwaddr_t addr, hwaddr_t haaddr);

#endif