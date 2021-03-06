SET(CMAKE_SYSTEM_NAME Windows)

IF(USE_64BITS)
  SET(CROSSCOMPILER x86_64-w64-mingw32-)
ELSE()
  SET(CROSSCOMPILER i686-w64-mingw32-)
ENDIF()
IF($ENV{CROSSCOMPILER})
  SET(CROSSCOMPILER $ENV{CROSSCOMPILER})
ENDIF()

SET(Windows ON)
SET(WIN32 ON)
SET(CMAKE_C_COMPILER   ${CROSSCOMPILER}gcc)
SET(CMAKE_ASM_COMPILER   ${CROSSCOMPILER}gcc)
SET(CMAKE_CXX_COMPILER ${CROSSCOMPILER}g++)
SET(CMAKE_RC_COMPILER  ${CROSSCOMPILER}windres)

execute_process(COMMAND ${CMAKE_C_COMPILER} --print-sysroot OUTPUT_VARIABLE SYSROOT)
IF($ENV{SYSROOT})
  SET(SYSROOT $ENV{SYSROOT})
ENDIF()

SET(CMAKE_FIND_ROOT_PATH ${SYSROOT})
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

