#include "llqreflect.h"
#include "dbpool.h"
#include "module.h"

PreparedStatement_T dbpoolentry_PreparedStatement(Connection_T con, dbentryquery_p query) {
    PreparedStatement_T pre = NULL;
    reflect_p fld;
    if(con && query && (pre = Connection_prepareStatement(con, query->sql))) {
        int fi = 0;
        void* entry = query->entry;
        while((fld = (query->flds + fi)) && fld->size) {
            switch (fld->type) {
            case REFLECT_INT: 
            case REFLECT_UINT: 
                if(fld->size >= 8) {
                    PreparedStatement_setLLong(pre, fi, reflect_getintvalue(fld, entry));
                } else {
                    PreparedStatement_setInt(pre, fi, (int)reflect_getintvalue(fld, entry));
                }
                break;
            case REFLECT_TIME: 
                PreparedStatement_setTimestamp(pre, fi, (time_t)reflect_getuintvalue(fld, entry));
                break;
            case REFLECT_FLOAT: 
                PreparedStatement_setDouble(pre, fi, reflect_getfloatvalue(fld, entry));
                break;
            case REFLECT_STRBUF: 
            case REFLECT_STRING: 
                PreparedStatement_setString(pre, fi, reflect_getstring(fld, entry));
                break;
            default: break;
            }
            fi++;
        }
    }
    return pre;
}

int dbpoolentry_nonquery(dbpool_p o, dbentryquery_p query) {
     int recordcount = 0;
    PreparedStatement_T pre = NULL;
    Connection_T con = ConnectionPool_getConnection(o->pool);
    if((pre = dbpoolentry_PreparedStatement(con, query))) {
        PreparedStatement_execute(pre);
        recordcount = (int)PreparedStatement_rowsChanged(pre);
        Connection_close(con);//关闭数据库连接
    }
    return recordcount;
}
int64_t dbpoolentry_scalar(dbpool_p o, dbentryquery_p query) {
    int recordcount = 0;
    PreparedStatement_T pre = NULL;
    Connection_T con = ConnectionPool_getConnection(o->pool);
    if((pre = dbpoolentry_PreparedStatement(con, query))) {
        ResultSet_T stmt = PreparedStatement_executeQuery(pre);
        if(stmt && ResultSet_next(stmt)) {
            recordcount = ResultSet_getLLong(stmt, 1);
        }
        Connection_close(con);//关闭数据库连接
    }
    return recordcount;
}
int dbpoolentry_recordset(dbpool_p o, dbentryquery_p query, dbentryread_p record) {
    int recordcount = 0;
    PreparedStatement_T pre = NULL;
    Connection_T con = ConnectionPool_getConnection(o->pool);
    if(record && (pre = dbpoolentry_PreparedStatement(con, query))) {
        ResultSet_T stmt = PreparedStatement_executeQuery(pre);
        if(stmt) {
            recordcount = dbpool_entitys4sequence(stmt, record);
        }
        Connection_close(con);//关闭数据库连接
    }
    return recordcount;
}

