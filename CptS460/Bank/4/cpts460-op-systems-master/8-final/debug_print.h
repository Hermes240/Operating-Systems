/**********************************************************************
*           Author: Alan Tyson & Daniel Johnson
*           Last Update: 2016-03-30
*
*     DEBUG_PRINT.H sets up print statements that write log messages
*   to the output stream (dbgstream). Statements will be prepended
*   with filename, and line number, followed by the message defined.
*
*     NOTE: code blatently stolen and modified from the following SO answers
*   http://stackoverflow.com/questions/327836/multi-file-c-program-how-best-to-implement-optional-logging/
*
**********************************************************************/

#include <stdio.h>

extern FILE *dbgstream = 0;
extern int debug_level = 6;

#define LOG_FATAL    (1)
#define LOG_ERR      (2)
#define LOG_WARN     (3)
#define LOG_INFO     (4)
#define LOG_DBG      (5)


#define LOG(level, ...) do {  \
                            if (level >= debug_level) { \
                              fprintf(dbgstream, "%s:%d:%s() ", __FILE__, __LINE__, __func__); \
                              fprintf(dbgstream, __VA_ARGS__); \
                            } \
                        } while (0)
