#include <stdint.h>
#include <stdio.h>

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