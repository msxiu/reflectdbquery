#include "llqreflect.h"
#include "dbpool.h"
#include "module.h"

extern PreparedStatement_T dbpoolentry_PreparedStatement(Connection_T con, dbentryquery_p query);

static int dbpool_commit_entry(Connection_T con, dbcommitquery_p par){
    int real_count=0, i;
    PreparedStatement_T pre = NULL;
    if(con && par) {
        Connection_beginTransaction(con);
        unsigned char* recordset = (unsigned char*)par->query.entry;
        for(i=0; i < par->recordcount; i++) {//遍历记录
            par->query.entry = (recordset + (i * par->query.fldsize));//移动到当前记录
            if((pre = dbpoolentry_PreparedStatement(con, &par->query))) {//PreparedStatement_getParameterCount(pre) != 11
                PreparedStatement_execute(pre);
                if(1 != PreparedStatement_rowsChanged(pre)) {
                    return -1; 
                }
                real_count++;
            }
        }
    }
    return real_count;
}

/**事务方式提交一批数据*/
int dbpool_commit_entrys(dbpool_p o, int argc, dbcommitquery_p argv[]) {
    int real_count = 0, i, invokecount = 0;
    Connection_T con = NULL;//从数据库连接池中获取连接
    if((con = ConnectionPool_getConnection(o->pool))){
        for(i=0; i<argc; i++) {
            if(-1 == (invokecount = dbpool_commit_entry(con, argv[i]))) {
                Connection_rollback(con);
                Connection_close(con);//关闭数据库连接
                return 0;
            }
            real_count += invokecount;
        }
        Connection_commit(con);
        Connection_close(con);//关闭数据库连接
    }
    return real_count;
}