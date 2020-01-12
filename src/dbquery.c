#include "llqstrchr.h"
#include "llqvarymem.h"
#include "llqfixedbuffer.h"
#include "llqreflect.h"
#include "module.h"


int debug_dbquery_code = 0;

/** 数据库查询结对象销毁 */
void dbquery_reclaim(dbquery_p o) {
	if(o) {
		offsetptr_reclaim(&(o->statement));
		offsetptr_reclaim(&(o->parameter));
		memset(o, 0, sizeof(dbquery_t));
	}
}

/**给查询语句附加数据 */
int dbquery_statement(dbquery_t* o, const char* sql) {
	if(o && sql) {
		offsetptr_writestr(&o->statement, sql);
		return 1;
	}
	return 0;
}

static int dbquery_condition_exists(dbquery_t* o) {
	if(o && o->statement.ptr) {
		if(-1 == strchscaseindextail((const char*)(o->statement.ptr), "where")) {
			offsetptr_write(&o->statement, " WHERE ", 7);
			return 1;
		} else {
			offsetptr_write(&o->statement, " AND ", 5);
			return 2;
		}
	}
	return 0;
}


int dbquery_addparameter(dbquery_t* o, dbquery_param_t type, const void* buffer) {
	if(o && buffer) {
		switch(type) {
		case DBQUERY_INT:
			offsetptr_writeint32(&o->parameter, (int)DBQUERY_INT);
			offsetptr_write(&o->parameter, buffer, sizeof(int32_t));
			o->parcount++;//参数个数加1
			return 1;
		case DBQUERY_LONG:
			offsetptr_writeint32(&o->parameter, (int)DBQUERY_LONG);
			offsetptr_write(&o->parameter, buffer, sizeof(int64_t));
			o->parcount++;//参数个数加1
			return 1;
		case DBQUERY_TIME:
			offsetptr_writeint32(&o->parameter, (int)DBQUERY_TIME);
			offsetptr_write(&o->parameter, buffer, sizeof(time_t));
			o->parcount++;//参数个数加1
			return 1;
		case DBQUERY_DOUBLE:
			offsetptr_writeint32(&o->parameter, (int)DBQUERY_DOUBLE);
			offsetptr_write(&o->parameter, buffer, sizeof(double));
			o->parcount++;//参数个数加1
			return 1;
		case DBQUERY_STRING:
			offsetptr_writeint32(&o->parameter, (int)DBQUERY_STRING);
			offsetptr_writestring(&o->parameter, (const char*)buffer);
			o->parcount++;//参数个数加1
			return 1;
		default: break;
		}
	}
	return 0;
}


/**查询语句设置字符串参数 */
int dbquery_parameters(dbquery_t* o, const char *fmt,	...) {
	const char*p = NULL;
	int ret = 0;
	va_list ap;

	if(o && fmt) {
		if(fmt) {
			dbquery_condition_exists(o);
		}
		va_start(ap, fmt);
		for(p=fmt; *p; p++) {
			if(*p != '%') {
				offsetptr_writeint8(&o->statement, *p);
				continue;
			}
			switch(*++p) {
			case 'd':
				offsetptr_writeint8(&o->statement, '?');
				dbquery_param_integer(o, va_arg(ap, int));
				ret ++;
				break;
			case 'l':
				offsetptr_writeint8(&o->statement, '?');
				dbquery_param_long(o, va_arg(ap, long));
				ret ++;
				break;
			case 't':
				offsetptr_writeint8(&o->statement, '?');
				dbquery_param_time(o, va_arg(ap, time_t));
				ret ++;
				break;
			case 'f':
				offsetptr_writeint8(&o->statement, '?');
				dbquery_param_double(o, va_arg(ap, double));
				ret ++;
				break;
			case 's':
				offsetptr_writeint8(&o->statement, '?');
				dbquery_param_string(o, va_arg(ap, const char*));
				ret ++;
				break;
			default:
				offsetptr_writeint8(&o->statement, *p);
				break;
			}
		}
		va_end(ap);
	}
	return ret;
}
int dbquery_param_entity(dbquery_t* o, dbentryquery_p query) {
	int i = 0;
	if(o && query){
		void* v = query->entry;
		if(query->sql) {
			dbquery_condition_exists(o);
			offsetptr_writestr(&o->statement, query->sql);
		}
		if(query->flds && v) {
			reflect_p f;
			while((f = (query->flds + i)) && (REFLECT_NONE != f->type) && (f->size)) {
				switch(f->type){
				case REFLECT_INT:
				case REFLECT_UINT:
					if(sizeof(int) == f->size) {
						dbquery_param_integer(o, (int32_t)reflect_getintvalue(f, v));
					} else {
						dbquery_param_long(o, reflect_getintvalue(f, v));
					}
					break;
				case REFLECT_TIME:
					dbquery_param_time(o, (time_t)reflect_getintvalue(f, v));
					break;
				case REFLECT_FLOAT:
					dbquery_param_double(o, reflect_getfloatvalue(f, v));
					break;
				case REFLECT_STRBUF:
				case REFLECT_STRING:
					dbquery_param_string(o, reflect_getstring(f, v));
					break;
				default: break;
				}
				i++;
			}
		}
	}
	return i;
}

/**内部调用,查询绑定*/
int dbquery_view(dbquery_p o, FILE* out) {
	int idx = 0;
	size_t offset = 0;
	char buffer[16];
	if(o) {
		fprintf(out, "sql:%s\n", (char*)(o->statement.ptr));
		if(o->parcount) {
			idx = 1;//参数下标是从第一个开始
			dbquery_param_t type = DBQUERY_NONE;
			while(fixedbuffer_readint32(o->parameter.ptr, o->parameter.offset, &offset, (int*)(&type)) > 0 && DBQUERY_NONE != type) {
				switch(type) {
				case DBQUERY_TIME:
					fixedbuffer_read(o->parameter.ptr, o->parameter.offset, &offset, buffer, sizeof(time_t));
					fprintf(out, "%d:TIME %u;\n", idx, *((int32_t*)((time_t*)buffer)));
					idx++;
					break;
				case DBQUERY_INT:
					fixedbuffer_read(o->parameter.ptr, o->parameter.offset, &offset, buffer, sizeof(int));
					fprintf(out, "%d:INT %d;\n", idx, *((int*)buffer));
					idx++;
					break;
				case DBQUERY_LONG:
					fixedbuffer_read(o->parameter.ptr, o->parameter.offset, &offset, buffer, sizeof(int));
					fprintf(out, "%d:LONG %ld;\n", idx, *((int64_t*)buffer));
					idx++;
					break;
				case DBQUERY_DOUBLE:
					fixedbuffer_read(o->parameter.ptr, o->parameter.offset, &offset, buffer, sizeof(double));
					fprintf(out, "%d:DOUBLE %f;\n", idx, *((double*)buffer));
					idx++;
					break;
				case DBQUERY_STRING:
					fprintf(out, "%d:STRING '%s';\n", idx, fixedbuffer_stringof(o->parameter.ptr, o->parameter.offset, &offset));
					idx++;
					break;
				default:break;
				}
			}
		}
	}
	return idx;
}
