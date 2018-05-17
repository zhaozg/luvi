project ( gdbm C )
cmake_minimum_required ( VERSION 2.8 )

set ( PACKAGE_BUGREPORT "" )
set ( PACKAGE_NAME "gdbm" )
set ( PACKAGE_STRING "gdbm 1.14.1" )
set ( PACKAGE_TARNAME "gdbm" )
set ( PACKAGE_VERSION "1.14.1" )

if ( HAVE_SIZEOF_OFF_T )
  set ( HAVE_OFF_T 1 )
else ( HAVE_SIZEOF_OFF_T )
  set ( HAVE_OFF_T 0 )
  set ( off_t "long" )
endif ( HAVE_SIZEOF_OFF_T )

set(GDBM_DIR deps/gdbm)
if (DEFINED ENV{GDBM_DIR})
  set(GDBM_DIR $ENV{GDBM_DIR})
endif ()

file (GLOB_RECURSE GDBM_COMPAT_CFILES "${GDBM_DIR}/compat/*.c")
file (GLOB_RECURSE GDBM_SRC_CFILES "${GDBM_DIR}/src/*.c")
list (REMOVE_ITEM GDBM_SRC_CFILES "${GDBM_DIR}/src/debug.c"
  "${GDBM_DIR}/src/gdbm_dump.c"
  "${GDBM_DIR}/src/gdbm_load.c"
  "${GDBM_DIR}/src/gdbmtool.c"
  "${GDBM_DIR}/src/input-rl.c"
  )
# configure_file ( gdbm.proto gdbm.h COPYONLY )

include_directories ( 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_LIST_DIR}
  ${GDBM_DIR}/src
)

add_library ( gdbm ${GDBM_COMPAT_CFILES} ${GDBM_SRC_CFILES})
set(EXTRA_LIBS ${EXTRA_LIBS} gdbm)
#install_library ( gdbm )
#install_header ( gdbmdefs.h extern.h gdbmerrno.h systems.h dbm.h ndbm.h gdbmconst.h proto.h ${CMAKE_CURRENT_BINARY_DIR}/gdbm.h )
