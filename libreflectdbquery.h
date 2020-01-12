/*
 * llqdbquery.h
 *  Created on: 2017-8-1
 *      Author: Administrator
 */
#ifndef LLQDBQUERY_H_
#define LLQDBQUERY_H_
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include "llqvarymem.h"
#include "llqreflect.h"

#ifndef INLINE
#if defined(WIN32)
#define INLINE __inline
#else
#define INLINE inline
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum DBQUERY_PARAM_TYPE {//查询参数类型
	DBQUERY_NONE	= 0,//未知
	DBQUERY_INT		= 1,//整数型
	DBQUERY_LONG	= 2,//长整 型
	DBQUERY_TIME	= 3,//时间
	DBQUERY_DOUBLE	= 4,//小数
	DBQUERY_STRING	= 5,//字符串
} dbquery_param_t;
typedef struct {//数据库查询结构体
	offsetptr_t statement;//查询语句
	offsetptr_t parameter;//查询参数
	uint32_t parcount;//参数个数
} dbquery_t, *dbquery_p;
#define DBQUERY_EMPTY			{ OFFSETPTR_EMPTY, OFFSETPTR_EMPTY, 0 }
#define DBQUERY_INITIALIZE()	DBQUERY_EMPTY


/** 数据库查询结对象销毁 */
void dbquery_reclaim(dbquery_p o);
/**给查询语句附加数据 */
extern int dbquery_statement(dbquery_t* o, const char* sql);
extern int dbquery_addparameter(dbquery_t* o, dbquery_param_t type, const void* buffer);
/**查询语句设置time_t型参数 */
static INLINE int dbquery_param_time(dbquery_t* o, time_t v) { return dbquery_addparameter(o, DBQUERY_TIME, &v); }
/**查询语句设置整型参数 */
static INLINE int dbquery_param_integer(dbquery_t* o, int v) { return dbquery_addparameter(o, DBQUERY_INT, &v); }
/**查询语句设置整型参数*/
static INLINE int dbquery_param_long(dbquery_t* o, int64_t v) { return dbquery_addparameter(o, DBQUERY_LONG, &v); }
/**查询语句设置整型参数*/
static INLINE int dbquery_param_double(dbquery_t* o, double v) { return dbquery_addparameter(o, DBQUERY_DOUBLE, &v); }
/**查询语句设置字符串参数 */
static INLINE int dbquery_param_string(dbquery_t* o, const char* v) { return dbquery_addparameter(o, DBQUERY_STRING, v); }
/**查询语句设置字符串参数 */
extern int dbquery_parameters(dbquery_t* o, const char *fmt,	...);

typedef struct dbentryquery {
	const char* sql;//查询的SQL语句
	size_t fldsize;//实体结构长度
	reflect_p flds;//参数字段列表
	void* entry;//实体数据
} dbentryquery_t, * dbentryquery_p;
extern int dbquery_param_entity(dbquery_t* o, dbentryquery_p query);
/**内部调用,查询绑定*/
extern int dbquery_view(dbquery_p o, FILE* out);


//--------------------------------------------------------------------------------------------
typedef struct URL_S *URL_T;
typedef struct ConnectionPool_S *ConnectionPool_T;
typedef struct ResultSet_S *ResultSet_T;
typedef struct {
	URL_T url;
	ConnectionPool_T pool;
} dbpool_t, * dbpool_p;
extern int dbpool_open(dbpool_p o, const char* connstr);
extern void dbpool_close(dbpool_p o);


typedef struct dbentryread {
	size_t fldsize;//实体结构长度
	reflect_p flds;//参数字段列表
	int recordcount;//记录个数
	void* recordset;//实体数据列表
} dbentryread_t, * dbentryread_p;
extern int dbpool_entitys4sequence(ResultSet_T stmt, dbentryread_p record);
extern int dbpool_entitys4name(ResultSet_T stmt, dbentryread_p record);


extern int dbpoolexecute_nonquery(dbpool_p o, const char* sql);
extern int64_t dbpoolexecute_scalar(dbpool_p o, const char* sql);
extern int dbpoolexecute_recordset(dbpool_p o, const char* sql, dbentryread_p record);


extern int dbpoolquery_nonquery(dbpool_p o, dbquery_p query);
extern int64_t dbpoolquery_scalar(dbpool_p o, dbquery_p query);
extern int dbpoolquery_recordset(dbpool_p o, dbquery_p query, dbentryread_p record);


extern int dbpoolentry_nonquery(dbpool_p o, dbentryquery_p query);
extern int64_t dbpoolentry_scalar(dbpool_p o, dbentryquery_p query);
extern int dbpoolentry_recordset(dbpool_p o, dbentryquery_p query, dbentryread_p record);


typedef struct dbcommitquery { //实体参数查询
	dbentryquery_t query;
	int recordcount;//记录个数
} dbcommitquery_t, *dbcommitquery_p;
/**事务方式提交一批数据*/
extern int dbpool_commit_entrys(dbpool_p o, int argc, dbcommitquery_p argv[]);


#ifdef __cplusplus
}
#endif

#endif /* LLQDBQUERY_H_ */
