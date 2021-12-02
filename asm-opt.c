#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "asm-opt.h"

#if defined(__linux__)

#define SOMEWHAT_SANE_PROC_CPUINFO_SIZE_LIMIT (1024 * 1024) 
#if defined(__i386__)||defined(__amd64__)
#define FEATURES_ID "flags"
#elif defineed(__arm__)
#define FEATURES_ID "Features"
#elif defined(__mips__)
#define FEATURES_ID "cpu modle"
#else
#define FEATURES_ID "?"
#endif


#else


#endif
static bench_info empty[] = { { NULL, 0, NULL } };
#if defined(__i386__)||defined(__amd64__)
#include "x86-sse2.h"

static bench_info x86_sse2[]=
{
    {"MOVSB copy",0,aligned_block_copy_movsb},
    { "MOVSD copy", 0, aligned_block_copy_movsd },
    { "SSE2 copy", 0, aligned_block_copy_sse2 },
    { "SSE2 nontemporal copy", 0, aligned_block_copy_nt_sse2 },
    { "SSE2 copy prefetched (32 bytes step)", 0, aligned_block_copy_pf32_sse2 },
    { "SSE2 copy prefetched (64 bytes step)", 0, aligned_block_copy_pf64_sse2 },
    { "SSE2 nontemporal copy prefetched (32 bytes step)", 0, aligned_block_copy_nt_pf32_sse2 },
    { "SSE2 nontemporal copy prefetched (64 bytes step)", 0, aligned_block_copy_nt_pf64_sse2 },
    { "SSE2 2-pass copy", 1, aligned_block_copy_sse2 },
    { "SSE2 2-pass copy prefetched (32 bytes step)", 1, aligned_block_copy_pf32_sse2 },
    { "SSE2 2-pass copy prefetched (64 bytes step)", 1, aligned_block_copy_pf64_sse2 },
    { "SSE2 2-pass nontemporal copy", 1, aligned_block_copy_nt_sse2 },
    { "SSE2 fill", 0, aligned_block_fill_sse2 },
    { "SSE2 nontemporal fill", 0, aligned_block_fill_nt_sse2 },
    { NULL, 0, NULL }
};

static bench_info x86_sse2_fb[] =
{
    { "MOVSD copy (from framebuffer)", 0, aligned_block_copy_movsd },
    { "MOVSD 2-pass copy (from framebuffer)", 1, aligned_block_copy_movsd },
    { "SSE2 copy (from framebuffer)", 0, aligned_block_copy_sse2 },
    { "SSE2 2-pass copy (from framebuffer)", 1, aligned_block_copy_sse2 },
    { NULL, 0, NULL }
};

static int check_sse2_support(void)
{
#ifdef __amd64__
    return 1; /* We assume that all 64-bit processors have SSE2 support */
#else
    int cpuid_feature_information;
    __asm__ volatile (
        /* According to Intel manual, CPUID instruction is supported
         * if the value of ID bit (bit 21) in EFLAGS can be modified */
        "pushf\n"
        "movl     (%%esp),   %0\n"
        "xorl     $0x200000, (%%esp)\n" /* try to modify ID bit */
        "popf\n"
        "pushf\n"
        "xorl     (%%esp),   %0\n"      /* check if ID bit changed */
        "jz       1f\n"
        "push     %%eax\n"
        "push     %%ebx\n"
        "push     %%ecx\n"
        "mov      $1,        %%eax\n"
        "cpuid\n"
        "pop      %%ecx\n"
        "pop      %%ebx\n"
        "pop      %%eax\n"
        "1:\n"
        "popf\n"
        : "=d" (cpuid_feature_information)
        :
        : "cc");
    return cpuid_feature_information & (1 << 26);
#endif
}

bench_info *get_asm_benchmarks(void)
{
    if(check_sse2_support())
        return x86_sse2;
    else 
        return empty;
}

bench_info *get_asm_framebuffer_benchmarks(void){
    if(check_sse2_support())
        return x86_sse2_fb;
    else 
        return empty;
}

#endif