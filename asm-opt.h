#ifndef __ASM_OPT_H__
#define __ASM_OPT_H__

#include <stdint.h>
typedef struct 
{
    const char *description;
    int use_tmpbuf;
    void (*f)(int64_t *,int64_t *,int);
}bench_info;

bench_info *get_asm_benchmarks(void);
bench_info *get_asm_framebuffer_benchmarks(void);

#endif