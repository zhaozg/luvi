#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int lua_pokercheck(lua_State*L);
int lua_cpuid(lua_State*L);

luaL_Reg poker_reglib[] = {
  { "check", lua_pokercheck },
  { "cpuid", lua_cpuid },
  { 0,0 } 
};

int luaopen_misc(lua_State*L) {
  luaL_register(L, "misc", poker_reglib);
  return 1;
}
