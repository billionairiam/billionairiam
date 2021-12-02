#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

double gettime(void);
double fmin(double, double);

void aligned_block_copy(int64_t * __restrict dst,
                        int64_t * __restrict src,
                        int                  size);

void aligned_block_copy_backwards(int64_t * __restrict dst,
                                  int64_t * __restrict src,
                                  int                  size);
void aligned_block_copy_backwards_bs32(int64_t * __restrict dst,
                                       int64_t * __restrict src,
                                       int                  size);
void aligned_block_copy_backwards_bs64(int64_t * __restrict dst,
                                       int64_t * __restrict src,
                                       int                  size);

void aligned_block_copy_pf32(int64_t * __restrict dst,
                             int64_t * __restrict src,
                             int                  size);
void aligned_block_copy_pf64(int64_t * __restrict dst,
                             int64_t * __restrict src,
                             int                  size);

void aligned_block_fill(int64_t * __restrict dst,
                        int64_t * __restrict src,
                        int                  size);
void aligned_block_fill_shuffle16(int64_t * __restrict dst,
                                  int64_t * __restrict src,
                                  int                  size);
void aligned_block_fill_shuffle32(int64_t * __restrict dst,
                                  int64_t * __restrict src,
                                  int                  size);
void aligned_block_fill_shuffle64(int64_t * __restrict dst,
                                  int64_t * __restrict src,
                                  int                  size);

void *alloc_four_nonaliased_buffers(void **buf1, int size1,
                                    void **buf2, int size2,
                                    void **buf3, int size3,
                                    void **buf4, int size4);

#endif