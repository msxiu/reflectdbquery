#include "llqreflect.h"
#include "dbpool.h"
#include "module.h"


int dbpool_open(dbpool_p o, const char* connstr) {
    if(o && connstr && (o->url = URL_new(connstr))) {
        if((o->pool = ConnectionPool_new(o->url))) {
            ConnectionPool_start(o->pool);
            return 1;
        }
    }
    return 0;
}

void dbpool_close(dbpool_p o) {
    ConnectionPool_free(&o->pool);
    URL_free(&o->url);
}

int dbpoolexecute_nonquery(dbpool_p o, const char* sql) {
    int recordcount = 0;
    Connection_T con = ConnectionPool_getConnection(o->pool);
    if(con) {
        Connection_execute(con, sql);
        recordcount = (int)Connection_rowsChanged(con);
        Connection_close(con);//关闭数据库连接
    }
    return recordcount;
}
int64_t dbpoolexecute_scalar(dbpool_p o, const char* sql) {
    int recordcount = 0;
    Connection_T con = ConnectionPool_getConnection(o->pool);
    if(con) {
        ResultSet_T stmt = Connection_executeQuery(con, sql);
        if(stmt && ResultSet_next(stmt)) {
            recordcount = ResultSet_getLLong(stmt, 1);
        }
        Connection_close(con);//关闭数据库连接
    }
    return recordcount;
}
int dbpoolexecute_recordset(dbpool_p o, const char* sql, dbentryread_p record) {
    int recordcount = 0;
    Connection_T con = ConnectionPool_getConnection(o->pool);
    if(con) {
        ResultSet_T stmt = Connection_executeQuery(con, sql);
        if(stmt) {
            recordcount = dbpool_entitys4sequence(stmt, record);
        }
        Connection_close(con);//关闭数据库连接
    }
    return recordcount;
}


