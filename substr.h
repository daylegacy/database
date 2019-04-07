#ifndef SUBSTR_H
#define SUBSTR_H
#include <cstdio>
struct substr {
	const char * start;
	const char * end;
	int len;
	substr(const char * start, int len)
		:start(start), end(start + len), len(len)
	{}
	substr(const char * start, const char * end)
		:start(start), end(end), len(end - start)
	{}
	substr(const char * start, const char * end, int len)
		:start(start), end(end), len(len)
	{}

	void set_end(const char * end) {
		this->end = end;
		len = end - start;
	}
	void set_len(int len) {
		this->len = len;
		end = start + len;
	}
	void set_start(const char * start) {
		this->start = start;
		len = end - start;
	}
	void move_ends(int len) {
		this->start = start + len;
		this->end = end + len;
	}
	void print() {
		auto w = start;
		while (w != end) {
			printf("%c", *w);
			w++;
		}
		printf("\n");
	}
};
#endif // !SUBSTR_H

