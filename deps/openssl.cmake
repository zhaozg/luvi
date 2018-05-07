add_definitions(-DWITH_OPENSSL -DOPENSSL_CCSTC)
include(deps/lua-openssl.cmake)

if (WithSharedOpenSSL)
  find_package(OpenSSL REQUIRED)

  message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
  message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

  include_directories(${OPENSSL_INCLUDE_DIR})
  link_directories(${OPENSSL_ROOT_DIR}/lib)
  list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})
else (WithSharedOpenSSL)
  message("Enabling Static OpenSSL")
  IF(DEFINED ENV{OPENSSL_ROOT_DIR})
    SET(OPENSSL_ROOT_DIR $ENV{OPENSSL_ROOT_DIR})
  ELSE()
    set(OPENSSL_ROOT_DIR deps/openssl)
  ENDIF()
  message("OpenSSL source dir: ${OPENSSL_ROOT_DIR}")
  include(${OPENSSL_ROOT_DIR}/openssl.cmake)
  if (WithOpenSSLExtends)
    message("Enabling OpenSSL Extends")
    include(${OPENSSL_ROOT_DIR}/extends/extends.cmake)
  endif (WithOpenSSLExtends)
endif (WithSharedOpenSSL)


