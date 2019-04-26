#ifdef __linux__
#define _GNU_SOURCE 1
#endif
#include <lua.h>

//time.c come from https://github.com/leite/lua-time
#include "time.c"

//Sqlite3 with cipher support
//  sqlitext come from https://github.com/zhaozg/sqlitext
//  lsqlite3.c come from http://lua.sqlite.org/index.cgi/index
#include "sqlitext/amalgamation.c"
#include "lsqlite3.c"

//LuaIPC -- Inter - Process Communication for Lua
// come from https://github.com/siffiejoe/lua-luaipc
#define LUA_IPC_ONE
#include "ipc/one.c"

//Core MIME support, a part of LuaSocket toolkit
// come from https://github.com/diegonehab/luasocket
void  luaL_setfuncs(lua_State *L, const luaL_Reg *l, int nup);
#include "mime/mime.c"

//A self contained Lua MessagePack C implementation.
// come from https://github.com/antirez/lua-cmsgpack
#include "cmsgpack/lua_cmsgpack.c"

//Lua bindings for POSIX iconv
// come from https://github.com/ittner/lua-iconv
#ifdef _WIN32
#include "iconv/win_iconv.c"
#endif
#include "iconv/luaiconv.c"

//misc, maybe changed high frequency,so keep it on last commit
//#include "misc/misc.c"

#if defined(_WIN32)==0 && !defined(__ANDROID__)
#include "lgdbm.c"
#endif

int luvi_custom(lua_State* L) {
  lua_pushcfunction(L, luaopen_time);
  lua_setfield(L, -2, "time");

  lua_pushcfunction(L, luaopen_lsqlite3);
  lua_setfield(L, -2, "lsqlite3");

  lua_pushcfunction(L, luaopen_ipc);
  lua_setfield(L, -2, "ipc");

#ifdef _WIN32
  int luaopen_winreg(lua_State *L);
  lua_pushcfunction(L, luaopen_winreg);
  lua_setfield(L, -2, "winreg");
#endif

  lua_pushcfunction(L, luaopen_mime_core);
  lua_setfield(L, -2, "mime");

  lua_pushcfunction(L, luaopen_cmsgpack);
  lua_setfield(L, -2, "msgpack");

  lua_pushcfunction(L, luaopen_iconv);
  lua_setfield(L, -2, "iconv");

#if 0
  lua_pushcfunction(L, luaopen_misc);
  lua_setfield(L, -2, "misc");
#endif

#if defined(_WIN32)==0 && !defined(__ANDROID__)
  lua_pushcfunction(L, luaopen_gdbm);
  lua_setfield(L, -2, "gdbm");
#endif

  return 0;
}

