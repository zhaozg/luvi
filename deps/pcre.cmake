
if (WithSharedPCRE)
  find_package(PCRE REQUIRED)
  message("PCRE include dir: ${PCRE_INCLUDE_DIR}")
  message("PCRE libraries: ${PCRE_LIBRARIES}")
  include_directories(${PCRE_INCLUDE_DIR})
  link_directories(${PCRE_ROOT_DIR}/lib)
  list(APPEND LIB_LIST ${PCRE_LIBRARIES})
else (WithSharedPCRE)
  SET(PCRE_MATCH_LIMIT "150000" CACHE STRING
      "Default limit on internal looping. See MATCH_LIMIT in config.h.in for details.")
  OPTION(PCRE_BUILD_PCREGREP "Build pcregrep" OFF)
  OPTION(PCRE_BUILD_TESTS    "Build the tests" OFF)
  OPTION(PCRE_BUILD_PCRECPP "Build the PCRE C++ library (pcrecpp)." OFF)
  SET(PCRE_SUPPORT_UTF ON CACHE BOOL
      "Enable support for Unicode Transformation Format (UTF-8/UTF-16/UTF-32) encoding.")

  SET(PCRE_DIR deps/pcre)
  if (DEFINED ENV{PCRE_DIR})
    SET(PCRE_DIR $ENV{PCRE_DIR})
  endif ()

  include_directories(
    ${PCRE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/pcre.dir
  )
  add_subdirectory(${PCRE_DIR} pcre.dir)
  message("Enabling Static PCRE")
  list(APPEND EXTRA_LIBS pcre)
  add_definitions(-DPCRE_STATIC)
endif (WithSharedPCRE)

add_definitions(-DWITH_PCRE)
include(deps/lrexlib.cmake)

