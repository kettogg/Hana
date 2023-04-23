#pragma once

#if defined(_MSC_VER)
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC
#endif

/*
 *! Some Stl functions for Hana
 */

/**
 * TODO ...
 *
 */
// extern "C" DECLSPEC int read(char* fmt, ...); -> Not Working!

/*! Calculates a sinus.
 */
extern "C" DECLSPEC double sinus(double val);

extern "C" DECLSPEC int compstr(char *str1, char *str2);

extern "C" DECLSPEC char *read();

extern "C" DECLSPEC int readint();

extern "C" DECLSPEC double readflt();

// extern "C" DECLSPEC int
/*! Prints an integer value.
 * \param[in] val Integer value to be printed.
 * \return Always one FIXME
 */
extern "C" DECLSPEC int writeint(int val);

/*! Prints a double value.
 * \param[in] val Double value to be printed.
 * \return Always one FIXME
 */
extern "C" DECLSPEC double writeflt(double val);

/*! Built in write function
 * it works like the C printf function and uses the same format string definition.
 * \param[in] str  The format string.
 */
extern "C" DECLSPEC void write(char *str, ...);

/*! Prints formated string like printf but with a CR/'New line'
 * \param[in] str  The format string.
 */
extern "C" DECLSPEC void writeln(char *str, ...);
