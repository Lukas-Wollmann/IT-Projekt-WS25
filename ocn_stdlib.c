#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <wchar.h>

#define OCN_BOOL  int8_t
#define OCN_CHAR  int32_t
#define OCN_I32	  int32_t
#define OCN_TRUE  (OCN_BOOL) 1
#define OCN_FALSE (OCN_BOOL) 0

void print_i32(const OCN_I32 i) {
	printf("%d", i);
}

void print_bool(const OCN_BOOL b) {
	printf("%s", b ? "true" : "false");
}

void print_char(const OCN_CHAR c) {
	if (c <= 0x7F) {
		putchar(c);
	} else if (c <= 0x7FF) {
		putchar(0xC0 | (c >> 6));
		putchar(0x80 | (c & 0x3F));
	} else if (c <= 0xFFFF) {
		putchar(0xE0 | (c >> 12));
		putchar(0x80 | ((c >> 6) & 0x3F));
		putchar(0x80 | (c & 0x3F));
	} else if (c <= 0x10FFFF) {
		putchar(0xF0 | (c >> 18));
		putchar(0x80 | ((c >> 12) & 0x3F));
		putchar(0x80 | ((c >> 6) & 0x3F));
		putchar(0x80 | (c & 0x3F));
	}
}

void print_newline() {
	putchar('\n');
}

OCN_I32 read_i32() {
	OCN_I32 num;

	fputs("Enter i32: ", stdout);

	while (scanf("%d", &num) != 1) {
		while (getchar() != '\n') {
		}
	}

	return num;
}

OCN_BOOL read_bool() {
	char input[10];

	fputs("Enter bool: ", stdout);

	while (1) {
		if (scanf("%9s", input) == 1) {
			if (strcasecmp(input, "true") == 0)
				return OCN_TRUE;
			if (strcasecmp(input, "false") == 0)
				return OCN_FALSE;
		}

		while (getchar() != '\n') {
		}
	}
}

OCN_CHAR read_char() {
	fputs("Enter char: ", stdout);
	fflush(stdout);

	int first_byte = getchar();
	if (first_byte == EOF)
		return 0;
	if (first_byte == '\n')
		return '\n';

	OCN_CHAR res = 0;
	int extra_bytes = 0;

	if ((first_byte & 0x80) == 0) {
		res = first_byte;
	} else if ((first_byte & 0xE0) == 0xC0) {
		res = first_byte & 0x1F;
		extra_bytes = 1;
	} else if ((first_byte & 0xF0) == 0xE0) {
		res = first_byte & 0x0F;
		extra_bytes = 2;
	} else if ((first_byte & 0xF8) == 0xF0) {
		res = first_byte & 0x07;
		extra_bytes = 3;
	}

	for (int i = 0; i < extra_bytes; i++) {
		int next_byte = getchar();
		if (next_byte == EOF || (next_byte & 0xC0) != 0x80)
			break;
		res = (res << 6) | (next_byte & 0x3F);
	}

	int c;
	while ((c = getchar()) != '\n' && c != EOF)
		;

	return res;
}