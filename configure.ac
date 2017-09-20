AC_INIT([ch-indexing-service], [0.0.1])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects foreign -Wall -Werror])
AM_SILENT_RULES([yes])
AC_HEADER_STDC
AC_TYPE_SIZE_T
AC_SUBST([AM_CXXFLAGS])
AC_PROG_CC([gcc])
AC_PROG_CXX([g++])
AM_PROG_AR
AC_CHECK_TOOLS(CXX, [$CCC c++ g++ gcc CC cxx cc++ cl], gcc)
PKG_CHECK_MODULES(PROTOBUF, protobuf >= 3.3.0)
AC_SUBST(PROTOBUF_LIBS)
AC_SUBST(PROTOBUF_CFLAGS)
AC_SUBST(PROTOBUF_VERSION)
AC_LANG([C])
# AC_CHECK_HEADER_STDBOOL

CXXFLAGS="$CXXFLAGS -Wall -std=c++0x"
LIBS="-lpthread -lrt -lm -levent -lprotobuf $LIBS"

##########################################################################
# check for ch-pal library
##########################################################################
CPPFLAGS="$CPPFLAGS -I/usr/local/include"
CXXFLAGS="$CXXFLAGS -I/usr/local/include"
LDFLAGS="$LDFLAGS -L/usr/local/lib"

# dnl add the external library to the list of libraries which will be used for our test program
LIBS="-lch-pal -pthread -lrt $LIBS"

# dnl check for the presence of 'libtotest'
AC_MSG_CHECKING([for the ch-pal availability])
AC_LINK_IFELSE([
                AC_LANG_PROGRAM([#include <ch-pal/exp_pal.h>],
                                [pal_env_init()])
                ],
                [LIBCH_PAL_PRESENCE=1], [LIBCH_PAL_PRESENCE=0])

if test "$LIBCH_PAL_PRESENCE" = "1"; then
    AC_MSG_RESULT([found])
else
    AC_MSG_ERROR([not found !])
fi

##########################################################################
# check for ch-utils library
##########################################################################
#dnl add the external library to the list of libraries which will be used for our test program
LIBS="-lch-utils $LIBS"

#dnl check for the presence of 'libtotest'
AC_MSG_CHECKING([for the ch-utils availability])
AC_LINK_IFELSE([
                AC_LANG_PROGRAM([#include <ch-pal/exp_pal.h>
                                 #include <ch-utils/exp_list.h>],
                                [list_print_all_nodes(NULL)])
                ],
                [LIBCH_UTILS_PRESENCE=1], [LIBCH_UTILS_PRESENCE=0])

if test "$LIBCH_UTILS_PRESENCE" = "1"; then
    AC_MSG_RESULT([found])
else
    AC_MSG_ERROR([not found !])
fi

##########################################################################
# check for ch-sockmon library
##########################################################################
#dnl add the external library to the list of libraries which will be used for our test program
LIBS="-lch-sockmon $LIBS"

#dnl check for the presence of 'libtotest'
AC_MSG_CHECKING([for the ch-sockmon availability])
AC_LINK_IFELSE([
                AC_LANG_PROGRAM([#include <ch-pal/exp_pal.h>
                                 #include <ch-sockmon/exp_sockmon.h>],
                                [sockmon_create(NULL, NULL)])
                ],
                [LIBCH_SOCKMON_PRESENCE=1], [LIBCH_SOCKMON_PRESENCE=0])

if test "$LIBCH_SOCKMON_PRESENCE" = "1"; then
    AC_MSG_RESULT([found])
else
    AC_MSG_ERROR([not found !])
fi

##########################################################################
# check for ch-cpp-utils library
##########################################################################
#dnl add the external library to the list of libraries which will be used for our test program
LIBS="-lch-cpp-utils $LIBS"

AC_LANG_PUSH([C++])
#dnl check for the presence of 'libtotest'
AC_MSG_CHECKING([for the ch-cpp-utils availability])
AC_LINK_IFELSE([
                AC_LANG_PROGRAM([#include <ch-cpp-utils/fts.hpp>],
                                [FtsOptions options])
                ],
                [LIBCH_CPP_UTILS_PRESENCE=1], [LIBCH_CPP_UTILS_PRESENCE=0])

if test "$LIBCH_CPP_UTILS_PRESENCE" = "1"; then
    AC_MSG_RESULT([found])
else
    AC_MSG_ERROR([not found !])
fi
AC_LANG_POP

##########################################################################
# check for ch-protos library
##########################################################################
#dnl add the external library to the list of libraries which will be used for our test program
LIBS="-lch-protos $LIBS"

AC_LANG_PUSH([C++])
#dnl check for the presence of 'libtotest'
AC_MSG_CHECKING([for the ch-protos availability])
AC_LINK_IFELSE([
                AC_LANG_PROGRAM([#include <ch-protos/communication.pb.h>],
                                [indexer::Packet packet])
                ],
                [LIBCH_PROTOS_PRESENCE=1], [LIBCH_PROTOS_PRESENCE=0])

if test "$LIBCH_PROTOS_PRESENCE" = "1"; then
    AC_MSG_RESULT([found])
else
    AC_MSG_ERROR([not found !])
fi
AC_LANG_POP

##########################################################################

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(Makefile src/Makefile src/client/Makefile)
AC_OUTPUT
