#include "llqreflect.h"
#include "dbpool.h"
#include "module.h"

#define MAX_COLUMN_KEY  256
typedef struct columnkeys {
    int used;
    const char *columns[MAX_COLUMN_KEY];
} columnkeys_t, * columnkeys_p;

static int getIndex(columnkeys_p cs, const char* key){
    int i=0;
    for(i=0;i<cs->used;i++) {
        if(0 == strcasecmp(key, cs->columns[i])) return i;
    }
    return -1;
}

static int dbpool_entity_byname(columnkeys_p cs, ResultSet_T stmt, reflect_p flds, void* record){
    reflect_p fld;
    size_t i = 0;
    int idx = 0;
    if(stmt && flds && record) {
        if(0 == cs->used) {
            cs->used = ResultSet_getColumnCount(stmt);
            for(idx = 0; idx <cs->used; idx++) {
                cs->columns[idx]= ResultSet_getColumnName(stmt, idx);
            }
        }
        while((fld=(flds+i)) && fld->type) {
            i++;
            switch (fld->type) {
            case REFLECT_INT: 
                if((idx = getIndex(cs, fld->fldname)) > 0) {
                    reflect_setintvalue(fld, record, ResultSet_getInt(stmt, idx));
                }
                break;
            case REFLECT_UINT: 
                if((idx = getIndex(cs, fld->fldname)) > 0) {
                    reflect_setuintvalue(fld, record, ResultSet_getLLong(stmt, idx));
                }
                break;
            case REFLECT_TIME: 
                if((idx = getIndex(cs, fld->fldname)) > 0) {
                    reflect_setuintvalue(fld, record, ResultSet_getTimestamp(stmt, idx));
                }
                break;
            case REFLECT_FLOAT: 
                if((idx = getIndex(cs, fld->fldname)) > 0) {
                    reflect_setfloatvalue(fld, record, ResultSet_getDouble(stmt, idx));
                }
                break;
            case REFLECT_STRBUF: 
                if((idx = getIndex(cs, fld->fldname)) > 0) {
                    reflect_setstring(fld, record, ResultSet_getString(stmt, idx));
                }
                break;
            case REFLECT_STRING: 
                if((idx = getIndex(cs, fld->fldname)) > 0) {
                    reflect_mallocstring(fld, record, ResultSet_getString(stmt, idx));
                }
                break;
            default: break;
            }     
        }
    }
    return i;
}

int dbpool_entitys4name(ResultSet_T stmt, dbentryread_p record) {
    int count = 0;
    offsetptr_t rs = OFFSETPTR_EMPTY;//记录数据区
    columnkeys_t columns;
    if(stmt && record) {
        memset(&columns, 0, sizeof(columnkeys_t));
        while (ResultSet_next(stmt)) {
            void* entry = varyrecordset_next((void**)(&rs.ptr), &rs.size, &rs.offset, record->fldsize);
            dbpool_entity_byname(&columns, stmt, record->flds, entry);
            count++;
        }
        if(count > 0) {
            record->recordset = (void*)rs.ptr;
            record->recordcount = count;
            return 1;
        }
    }
    return 0;
}