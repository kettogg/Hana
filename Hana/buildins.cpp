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

#include "buildins.h"

char* copycs(const char* arr, long n)
{
   char* heap_arr = (char*)malloc(sizeof(char) * n);
   memcpy(heap_arr, arr, n * sizeof(char));
   return heap_arr;
}

extern "C" DECLSPEC char* input()
{
   std::string strtmp;
   std::getline(std::cin, strtmp);
   char* tmp = copycs(strtmp.c_str(), (int)strtmp.size() + 1);
   return tmp;
}

extern "C" DECLSPEC int printvalue(int val)
{
   std::cout << "IDEBUG: " << val << "\n";
   return 1;
}

extern "C" DECLSPEC double printdouble(double val)
{
   std::cout << "DDEBUG: " << val << "\n";
   return 1.;
}

extern "C" DECLSPEC void display(char* str, ...)
{
   va_list argp;
   va_start(argp, str);
   vprintf(str, argp);
   va_end(argp);
}

extern "C" DECLSPEC void displayln(char* str, ...)
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

extern "C" DECLSPEC int read(char* fmt, ...)
{
   int rc;
   va_list argp;
   va_start(argp, fmt);
   rc = vscanf(fmt, argp);
   va_end(argp);
   return (rc);
}
