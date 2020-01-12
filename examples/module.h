#ifndef INNER_DBQUERY_H_
#define INNER_DBQUERY_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>

#include "libreflectdbquery.h"


extern int debug_dbquery_code;
/**向控制台输出诊断提示*/
#define DIAGNOSE_PROMPT(k, format, args...)	if((debug_dbquery_code >= (k))) {\
	fprintf(stdout, "%s:%d=>", __FILE__, __LINE__); \
	fprintf(stdout, format, ##args); fprintf(stdout, "%s", "\r\n"); fflush(stdout);\
}
#define DIAGNOSE_DEBUG(format, args...) DIAGNOSE_PROMPT(5, format, ##args)
#define DIAGNOSE_INFO(format, args...) DIAGNOSE_PROMPT(4, format, ##args)
#define DIAGNOSE_WARN(format, args...) DIAGNOSE_PROMPT(3, format, ##args)
#define DIAGNOSE_ERROR(format, args...) DIAGNOSE_PROMPT(2, format, ##args)
#define DIAGNOSE_FATAL(format, args...) DIAGNOSE_PROMPT(1, format, ##args)



#endif
