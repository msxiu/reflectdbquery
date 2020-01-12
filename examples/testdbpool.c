#include "dbpool.h"
#include "module.h"


typedef struct testquery {
    int id, interval, gradation;
    char domain[255];
    char title[255];
    time_t addtime;
}testquery_t, * testquery_p;
static reflect_t reflds[] = {
    REFLECT_INIT(REFLECT_INT, testquery_t, id),
    REFLECT_INIT(REFLECT_INT, testquery_t, interval),
    REFLECT_INIT(REFLECT_INT, testquery_t, gradation),
    REFLECT_INIT(REFLECT_STRBUF, testquery_t, domain),
    REFLECT_INIT(REFLECT_STRBUF, testquery_t, title),
    REFLECT_INIT(REFLECT_TIME, testquery_t, addtime),
    REFLECT_EMPTY,
};

extern int debug_dbquery_code;
int main(int argc, const char* argv[]) {
    URL_T url = URL_new("mysql://127.0.0.1/controlcenter_v2?user=tipscrawl&password=TipsCrawl.2");//
    ConnectionPool_T pool = ConnectionPool_new(url);
    ConnectionPool_start(pool);

    Connection_T con = ConnectionPool_getConnection(pool);
    ResultSet_T result = Connection_executeQuery(con, "select `id`,`interval`,`gradation`,`domain`,`title`,`addtime` from `domains` where id > %d", 0);
    dbentryread_t par = { sizeof(testquery_t), reflds, 0, NULL };
    int i=0;
    debug_dbquery_code = 8;
    if(dbpool_entitys4sequence(result, &par)) {
        for(i=0; i < par.recordcount; i++) {
            testquery_p record = (((testquery_p)par.recordset) + i);
            printf("--------- %4d -------------------------------------------\n", i);
            printf("id:%d\n", record->id);
            printf("interval:%d\n", record->interval);
            printf("gradation:%d\n", record->gradation);
            printf("domain:%s\n", record->domain);
            printf("title:%s\n", record->title);
            printf("addtime:%d\n", (int)record->addtime);
        }
    }
    free(par.recordset);
    Connection_close(con);//关闭数据库连接
    
    ConnectionPool_free(&pool);
    URL_free(&url);
    return 0;
}