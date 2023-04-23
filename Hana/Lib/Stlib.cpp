#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cmath>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

#include "Stlib.h"

char* copycs(const char* arr, long n)
{
    char* heap_arr = (char*)malloc(sizeof(char) * n);
    memcpy(heap_arr, arr, n * sizeof(char));
    return heap_arr;
}

extern "C" DECLSPEC char* read()
{
    std::string strtmp;
    std::getline(std::cin, strtmp);
    char* tmp = copycs(strtmp.c_str(), (int)strtmp.size() + 1);
    return tmp;
}

extern "C" DECLSPEC int readint()
{
    std::string strtmp;
    std::getline(std::cin, strtmp);
    char* tmp = copycs(strtmp.c_str(), (int)strtmp.size() + 1);
    int val = std::atoi(tmp);
    return val;
}

extern "C" DECLSPEC double readflt()
{
    std::string strtmp;
    std::getline(std::cin, strtmp);
    char* tmp = copycs(strtmp.c_str(), (int)strtmp.size() + 1);
    char* stopString;
    double val = std::strtod(tmp, &stopString);
    // Implement something to print Error if it has string ...
    return val;
}

// For comparing two Strings
extern "C" DECLSPEC int compstr(char* str1, char* str2)
{
    if(!std::strcmp(str1, str2)) return 1;
    return 0;
}

extern "C" DECLSPEC int writeint(int val)
{
    std::cout << val << "\n";
    return 1;
}

extern "C" DECLSPEC double writeflt(double val)
{
    std::cout << val << "\n";
    return 1.;
}

extern "C" DECLSPEC void write(char* str, ...)
{
    va_list argp;
    va_start(argp, str);
    vprintf(str, argp);
    va_end(argp);
}

extern "C" DECLSPEC void writeln(char* str, ...)
{
    char* outstr;
    va_list argp;
    va_start(argp, str);
    outstr = (char*)malloc(strlen(str) + 2);
    strcpy(outstr, str);
    strcat(outstr, "\n");
    vprintf(outstr, argp);
    va_end(argp);
    free(outstr);
}

extern "C" DECLSPEC double sinus(double val)
{
    return std::sin(val);
}
