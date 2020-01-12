#include "llqfixedbuffer.h"
#include "llqreflect.h"
#include "dbpool.h"
#include "module.h"

static PreparedStatement_T dbpoolquery_PreparedStatement(Connection_T con, dbquery_p o) {
	int idx = 0;
	size_t offset;
	char tmbuf[64], buffer[16];
	const char *paramval;
	dbquery_param_t type = DBQUERY_NONE;
    PreparedStatement_T pre = NULL;
    if(con && o && (pre = Connection_prepareStatement(con, o->statement.ptr))) {
		idx = 1;//参数下标是从第一个开始
		while(fixedbuffer_readint32(&o->parameter.ptr, o->parameter.offset, &offset, (int32_t*)(&type)) > 0 && DBQUERY_NONE != type) {
			switch(type) {
			case DBQUERY_TIME:
				fixedbuffer_read(&o->parameter.ptr, o->parameter.offset, &offset, buffer, sizeof(time_t));
                PreparedStatement_setTimestamp(pre, idx, *((int*)buffer));
                DIAGNOSE_DEBUG("%d time sqlite parameter of '%s';\n", idx, tmbuf);
				idx++;
				break;
			case DBQUERY_INT:
				fixedbuffer_read(&o->parameter.ptr, o->parameter.offset, &offset, buffer, sizeof(int32_t));
				PreparedStatement_setInt(pre, idx, *((int32_t*)buffer));
				DIAGNOSE_DEBUG("%d int sqlite parameter of %d;\n", idx, *((int32_t*)buffer));
				idx++;
				break;
			case DBQUERY_LONG:
				fixedbuffer_read(&o->parameter.ptr, o->parameter.offset, &offset, buffer, sizeof(int64_t));
				PreparedStatement_setLLong(pre, idx, *((int64_t*)buffer));
				DIAGNOSE_DEBUG("%d long sqlite parameter of %ld;\n", idx, *((int64_t*)buffer));
				idx++;
				break;
			case DBQUERY_DOUBLE:
				fixedbuffer_read(&o->parameter.ptr, o->parameter.offset, &offset, buffer, sizeof(double));
				PreparedStatement_setDouble(pre, idx, *((double*)buffer));
				DIAGNOSE_DEBUG("%d long sqlite parameter of %f;\n", idx, *((double*)buffer));
				idx++;
				break;
			case DBQUERY_STRING://int sqlite3_bind_text(sqlite3_stmt*, int, const char*, int n, void(*)(void*));
				//第四个参数代表第三个参数中需要传递的长度。-1表示传递全部字符串。第五个参数是一个回调函数，比如执行后做内存清除工作。
				paramval = fixedbuffer_stringof(&o->parameter.ptr, o->parameter.offset, &offset);
				PreparedStatement_setString(pre, idx, paramval);
				DIAGNOSE_DEBUG("%d string sqlite parameter of '%s';\n", idx, paramval);
				idx++;
				break;
			default:break;
			}
		}
    }
    return pre;
}

int dbpoolquery_nonquery(dbpool_p o, dbquery_p query) {
     int recordcount = 0;
    PreparedStatement_T pre = NULL;
    Connection_T con = ConnectionPool_getConnection(o->pool);
    if((pre = dbpoolquery_PreparedStatement(con, query))) {
        PreparedStatement_execute(pre);
        recordcount = (int)PreparedStatement_rowsChanged(pre);
        Connection_close(con);//关闭数据库连接
    }
    return recordcount;
}
int64_t dbpoolquery_scalar(dbpool_p o, dbquery_p query) {
    int recordcount = 0;
    PreparedStatement_T pre = NULL;
    Connection_T con = ConnectionPool_getConnection(o->pool);
    if((pre = dbpoolquery_PreparedStatement(con, query))) {
        ResultSet_T stmt = PreparedStatement_executeQuery(pre);
        if(stmt && ResultSet_next(stmt)) {
            recordcount = ResultSet_getLLong(stmt, 1);
        }
        Connection_close(con);//关闭数据库连接
    }
    return recordcount;
}
int dbpoolquery_recordset(dbpool_p o, dbquery_p query, dbentryread_p record) {
    int recordcount = 0;
    PreparedStatement_T pre = NULL;
    Connection_T con = ConnectionPool_getConnection(o->pool);
    if(record && (pre = dbpoolquery_PreparedStatement(con, query))) {
        ResultSet_T stmt = PreparedStatement_executeQuery(pre);
        if(stmt) {
            recordcount = dbpool_entitys4sequence(stmt, record);
        }
        Connection_close(con);//关闭数据库连接
    }
    return recordcount;
}

