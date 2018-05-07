/*
* this is a:
* time lib for lua it provide microseconds, miliseconds and seconds and diference between them
*
* author:
* @xxleite
*
* date:
* 13:09 10/8/2011
*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <xxleite@gmail.com> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer in return
* ----------------------------------------------------------------------------
*/

#include <stdio.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <WinSock2.h>
#include <time.h>
static int gettimeofday(struct timeval *tp, void *tzp)
{
  time_t clock;
  struct tm tm;
  SYSTEMTIME wtm;

  GetLocalTime(&wtm);
  tm.tm_year = wtm.wYear - 1900;
  tm.tm_mon = wtm.wMonth - 1;
  tm.tm_mday = wtm.wDay;
  tm.tm_hour = wtm.wHour;
  tm.tm_min = wtm.wMinute;
  tm.tm_sec = wtm.wSecond;
  tm.tm_isdst = -1;
  clock = mktime(&tm);
  tp->tv_sec = (long)clock;
  tp->tv_usec = wtm.wMilliseconds * 1000;

  return (0);
}

static  int settimeofday(struct timeval *tp, void *tzp)
{
  BOOL ret;
  time_t clock = tp->tv_sec;
  SYSTEMTIME wtm = { 0 };

  struct tm *tm = localtime(&clock);
  wtm.wYear = tm->tm_year + 1900;
  wtm.wMonth = tm->tm_mon + 1;
  wtm.wDay = tm->tm_mday;
  wtm.wHour = tm->tm_hour;
  wtm.wMinute = tm->tm_min;
  wtm.wSecond = tm->tm_sec;
  wtm.wMilliseconds = (WORD)(tp->tv_usec / 1000);
  ret = SetLocalTime(&wtm);

  return ret ? 0 : -1;
}

#endif

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#ifndef LUA_API
#define LUA_API  __declspec(dllexport)
#endif

#ifndef UINT
#define UINT unsigned int
#endif

const UINT SEC  = 	2;
const UINT MSEC = 	4;
const UINT USEC = 	8;

/* time helper function */
static double get_time( UINT k ){

	struct timeval tv;
	gettimeofday( &tv, NULL );

	if( k==SEC ) 		return tv.tv_sec;
	else if( k==MSEC )	return (tv.tv_sec + (double)((int)(tv.tv_usec*0.001) * 0.001));
	else if( k==USEC )	return (tv.tv_usec*0.000001);
	else 				return 0;
}

/* get miliseconds relative to seconds since EPOCH */
static int t_mili (lua_State *L) {

	lua_pushnumber(L, get_time( MSEC ) );
	return 1;
}

/* get seconds since EPOCH */
static int t_seconds (lua_State *L) {

	lua_pushnumber( L, get_time( SEC ) );
	return 1;
}

/* get microseconds relative to seconds since EPOCH */
static int t_micro (lua_State *L) {

	lua_pushnumber(L, get_time( USEC ) );
	return 1;
}

/* return the diference in miliseconds relative to seconds since EPOCH */
static int t_diff (lua_State *L){

	double v1= (double)luaL_checknumber( L, 1 );

	lua_pushnumber( L, ( get_time( MSEC ) - v1 ) );
	return 1;
}

/* return seconds, miliseconds and microseconds */
static int t_time (lua_State *L){

	struct timeval tv;
	gettimeofday( &tv, NULL );

	lua_pushnumber( L, tv.tv_sec );
	lua_pushnumber( L, (double)((int)(tv.tv_usec*0.001) * 0.001) );
	lua_pushnumber( L, (double)(tv.tv_usec * 0.000001) );

	return 3;
}

static int t_set(lua_State *L) {

  int ret;
  struct timeval tv;
  tv.tv_sec = luaL_checkinteger(L, 1);
  tv.tv_usec = luaL_optinteger(L, 2, 0);
  ret = settimeofday(&tv, NULL);

  lua_pushboolean(L, ret == 0);
  return 1;
}

/* register functions */
const luaL_Reg time_lib[] = {
  {"getMiliseconds", t_mili},
  {"getSeconds", t_seconds},
  {"getMicroseconds", t_micro},
  {"getDiff", t_diff},
  {"getTime", t_time},
  {"setTime", t_set},
  {NULL, NULL}
};

/* register lib */
LUALIB_API int luaopen_time (lua_State *L) {

  luaL_register(L, "time", time_lib);
  return 1;
}
