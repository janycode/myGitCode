
INC_DIR = -I../inc

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -fno-inline \
        -fno-short-enums \
         -fPIC \
         $(DIAG_CFLAGS) \
        ${INC_DIR}
        
AM_CPPFLAGS = -D__packed__= \
         -fPIC \
         $(DIAG_CFLAGS) \
        ${INC_DIR}

libsqlite3_db_c_sources = \
	    db_common.c \
		db_usage_sqlite3.c
 
requiredlibs = -lc $(DIAG_LIBS) -lsqlite3
lib_LTLIBRARIES = libsqlite3_db.la
libsqlite3_db_la_CC = @CC@
libsqlite3_db_la_SOURCES = $(libsqlite3_db_c_sources)
libsqlite3_db_la_CFLAGS = $(AM_CFLAGS)
libsqlite3_db_la_CPPFLAGS = $(AM_CPPFLAGS)
libsqlite3_db_la_LDFLAGS = $(requiredlibs)
#libsqlite3_db_la_LIBADD = ##add linked libs##



