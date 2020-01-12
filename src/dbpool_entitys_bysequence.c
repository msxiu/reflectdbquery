#include "llqreflect.h"
#include "dbpool.h"
#include "module.h"

static int dbpool_entity_bysequence(ResultSet_T stmt, reflect_p flds, void* record){
    reflect_p fld;
    size_t i = 0;
    if(stmt && flds && record) {
        while((fld=(flds+i)) && fld->type) {
            i++;
            switch (fld->type) {
            case REFLECT_INT: 
                reflect_setintvalue(fld, record, ResultSet_getInt(stmt, i));
                break;
            case REFLECT_UINT: 
                reflect_setuintvalue(fld, record, ResultSet_getLLong(stmt, i));
                break;
            case REFLECT_TIME: 
                reflect_setuintvalue(fld, record, ResultSet_getTimestamp(stmt, i));
                break;
            case REFLECT_FLOAT: 
                reflect_setfloatvalue(fld, record, ResultSet_getDouble(stmt, i));
                break;
            case REFLECT_STRBUF: 
                reflect_setstring(fld, record, ResultSet_getString(stmt, i));
                break;
            case REFLECT_STRING: 
                reflect_mallocstring(fld, record, ResultSet_getString(stmt, i));
                break;
            default: break;
            }     
        }
    }
    return i;
}

int dbpool_entitys4sequence(ResultSet_T stmt, dbentryread_p record) {
    int count = 0;
    offsetptr_t rs = OFFSETPTR_EMPTY;//记录数据区
    if(stmt && record) {
        while (ResultSet_next(stmt)) {
            void* entry = varyrecordset_next((void**)(&rs.ptr), &rs.size, &rs.offset, record->fldsize);
            dbpool_entity_bysequence(stmt, record->flds, entry);
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