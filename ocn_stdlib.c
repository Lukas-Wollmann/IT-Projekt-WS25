#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void print_i32(const int32_t i) {
	printf("%d", i);
}

void print_bool(const int8_t b) {
	printf("%s", b ? "true" : "false");
}

void print_char(const int32_t cp) {
	if (cp <= 0x7F) {
		putchar(cp);
	} else if (cp <= 0x7FF) {
		putchar(0xC0 | (cp >> 6));
		putchar(0x80 | (cp & 0x3F));
	} else if (cp <= 0xFFFF) {
		putchar(0xE0 | (cp >> 12));
		putchar(0x80 | ((cp >> 6) & 0x3F));
		putchar(0x80 | (cp & 0x3F));
	} else if (cp <= 0x10FFFF) {
		putchar(0xF0 | (cp >> 18));
		putchar(0x80 | ((cp >> 12) & 0x3F));
		putchar(0x80 | ((cp >> 6) & 0x3F));
		putchar(0x80 | (cp & 0x3F));
	}
}

void print_newline() {
	putchar('\n');
}

typedef struct {
	size_t refCount;
	void (*dtor)(void *);
} ControlBlock;

void *__sp_create(size_t size, void (*dtor)(void *)) {
	void *ptr = malloc(size + sizeof(ControlBlock));
	ControlBlock *cbptr = (ControlBlock *) ptr;
	cbptr->refCount = 1;
	cbptr->dtor = dtor;
	return cbptr + 1;
}

void *__sp_copy(void *ptr) {
	ControlBlock *cbptr = (ControlBlock *) ptr - 1;
	cbptr->refCount++;
	return ptr;
}

void __sp_drop(void *ptr) {
	ControlBlock *cbptr = (ControlBlock *) ptr - 1;
	cbptr->refCount--;
	if (cbptr->refCount == 0) {
		if (cbptr->dtor)
			cbptr->dtor(ptr);
		free(cbptr);
	}
}