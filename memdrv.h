/*
* Copyright (C) 2016 William H. Welna All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY William H. Welna ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL William H. Welna BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MEMDRV_H
#define MEMDRV_H

#include <pstdint.h>

// Error Codes
#define MEMDRV_UNDEF -1
#define MEMDRV_BOUNDS -2
#define MEMDRV_ALLOC -3
#define MEMDRV_REALLOC -4


typedef struct {
	uint32_t page_size; // Size of a page
	uint32_t page_alloc_multiple; // How many pages to pre-allocate
	unsigned char *data; // Start of Data Allocated
	unsigned char *p; // Current Pointer
	uint32_t seek_p; // = *p - *data
	uint32_t size; // Size of Data Allocated
} memdrv;

// Allocate of Size
int memdrv_init(memdrv *, uint32_t);

// Writes buffer to memory of size, if memdrv too small, tries to realloc, increases p
int memdrv_write(memdrv *, char *, uint32_t);

// Tries write contents to buffer of length, then sets pointer to int of actual size, increases p
int memdrv_read(memdrv *, char *, uint32_t *);

// Returns memdrv->seek_p and sanity checks/recalculates
uint32_t memdrv_getp(memdrv *);

// sets memdrv->seek_p and data + seek_p checking bounds by size
int memdrv_setp(memdrv *, uint32_t);

// Returns pointer to data at x position, null if out of bounds
char *memdrv_datap(memdrv *, uint32_t);

// Returns size of data buffer
uint32_t memdrv_size(memdrv *);

// Increases current size of buffer by x amount
int memdrv_inc(memdrv *, uint32_t);

// frees memory allocated in memdrv structure, doesn't call free(memdrv *)
void memdrv_free(memdrv *);


#endif
