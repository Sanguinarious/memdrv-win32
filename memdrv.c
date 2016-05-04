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

#include <windows.h>
#include "memdrv.h"

int memdrv_init(memdrv *m, uint32_t s) {
	char *b;
	uint32_t round=0;
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);
	if(!m->page_alloc_multiple)
		m->page_alloc_multiple = 4;
	round = ((s/si.dwPageSize)+m->page_alloc_multiple)*si.dwPageSize;
	if((b=VirtualAlloc(NULL, round, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE))==NULL) return MEMDRV_ALLOC;
	m->p = m->data = b;
	m->size = round;
	m->page_size = si.dwPageSize;
	return 0;
}

int memdrv_write(memdrv *m, char *b, uint32_t s) {
	int ret=0;
	uint32_t left=m->size-m->seek_p;
	if(s > left) { // Needs Realloc
		if((ret=memdrv_inc(m, s-left))<0)
			return ret;
	}
	CopyMemory(m->p, b, s);
	m->seek_p += s;
	m->p += s;
	return s;
}

int memdrv_read(memdrv *m, char *b, uint32_t *s) {
	uint32_t left=m->seek_p-m->size;
	if(left < *s) *s = left;
	CopyMemory(b, m->p, *s);
	m->seek_p += *s;
	m->p += *s;
	return *s;
}

uint32_t memdrv_getp(memdrv *m) {
	return m->seek_p;
}

int memdrv_setp(memdrv *m, uint32_t p) {
	if(p > m->size) return MEMDRV_BOUNDS;
	m->p = m->data + p;
	m->seek_p = p;
}

uint32_t memdrv_size(memdrv *m) {
	return m->size;
}

char *memdrv_datap(memdrv *m, uint32_t p) {
	if(p > m->size) return NULL;
	return m->data + p;
}

int memdrv_inc(memdrv *m, uint32_t s) {
	uint32_t round=((s/m->page_size)+m->page_alloc_multiple)*m->page_size;
	unsigned char *newb, *end = m->data+m->size;
	// In order for this to work, need to keep track of all 'end' pointers to call virtualfree on or
	// causes a memory leak as it won't allow freeing of specific size blocks, etc. so left commented out
	//if(!(newb=VirtualAlloc(end, round, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE))) {
		// Couldn't Allocate and append pages at end of current pages
		// Now Allocate new pages of size and move data to it
		if(!(newb=VirtualAlloc(NULL, m->size+round, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)))
		       return MEMDRV_REALLOC; // Couldn't alloc new pages
		CopyMemory(newb, m->data, m->size); // Copy old pages to new one
		VirtualFree(m->data, 0, MEM_RELEASE); // Release old pages;
		m->data = newb;
		m->p = newb+m->seek_p;

	//}
	m->size += round;
	return m->size;
}

void memdrv_free(memdrv *m) {
	VirtualFree(m->data, 0, MEM_RELEASE);
	m->data = NULL;
	m->p = NULL;
	m->seek_p = 0;
	m->size = 0;
}

