AUTOMAKE_OPTIONS=foreign 
include_HEADERS = libacbmtree.h

#SUBDIRS     = example

# noinst_LTLIBRARIES      = libreflectdbquery.la
lib_LTLIBRARIES           = libreflectdbquery.la
libreflectdbquery_la_SOURCES    = src/dbpool_commit_entrys.c src/dbpool_entitys_byname.c src/dbpool_entitys_bysequence.c \
                                    src/dbpoolentry.c src/dbpoolexecute.c src/dbpoolquery.c src/dbquery.c

bin_PROGRAMS     = testdbpool
testdbpool_SOURCES    = example/testdbpool.c
testdbpool_LDFLAGS    = -lz -lpthread -ldbpool -lreflectdbquery