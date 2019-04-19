#ifndef HELPER_H
#define HELPER_H
#ifdef DEBUG

#define printD(...) {do{ printf(__VA_ARGS__);}while(0); }
#else
#define printD(...) {do{ }while(0); }
#endif

#define printE(...) {do{ fprintf(stderr, __VA_ARGS__);}while(0); }


#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cassert>
#include <cstdio>
#include "parser.h"

#endif // !HELPER_H
