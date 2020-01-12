#include "libreflectdbquery.h"

typedef struct testquery {
    int channel, group;
    char name[128];
}testquery_t;


int main(int argc, const char* argv[]) {
    static reflect_t reflds[] = {
        REFLECT_INIT(REFLECT_INT, testquery_t, channel),
        REFLECT_INIT(REFLECT_INT, testquery_t, group),
        REFLECT_INIT(REFLECT_STRBUF, testquery_t, name),
        REFLECT_EMPTY,
    };
    
    testquery_t qval = { 33, 69, "testquery" };
    dbquery_t query = DBQUERY_EMPTY;
    const char *sql = "select * from table_name where channelid=%d and groupid=%d and name=%s;";
    dbentryquery_t par = { sql, sizeof(testquery_t), reflds, &qval };
    dbquery_parameters(&query, sql, 32, 68, "test");
    dbquery_view(&query, stdout);
    dbquery_reclaim(&query);
    dbquery_param_entity(&query, &par);
    dbquery_view(&query, stdout);
    dbquery_reclaim(&query);
    return 0;
}