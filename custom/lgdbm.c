/*
* lgdbm.c
* gdbm interface for Lua 5.2
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 02 Jul 2013 23:17:35
* This code is hereby placed in the public domain.
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gdbm.h"
#ifndef GDBM_NOMMAP
#define GDBM_NOMMAP 0
#endif

#include "lua.h"
#include "lauxlib.h"

#define MYNAME		"gdbm"
#define MYVERSION	MYNAME " library for " LUA_VERSION " / Jul 2013 / \n"
#define MYTYPE		MYNAME " handle"

static datum encode(lua_State *L, int i)
{
 datum d;
 size_t l;
 d.dptr=(char*)luaL_checklstring(L,i,&l);
 d.dsize=(int)l;
 return d;
}

static int decode(lua_State *L, datum d)
{
 if (d.dptr==NULL)
  lua_pushnil(L);
 else
 {
  lua_pushlstring(L,d.dptr,d.dsize);
  free(d.dptr);
 }
 return 1;
}

static const char* errorstring(void)
{
 if (errno!=0)
  return strerror(errno);
 else if (gdbm_errno!=0)
  return gdbm_strerror(gdbm_errno);
 else
  return "(no message)";
}

static int pushresult(lua_State *L, int rc)
{
 if (rc==0)
 {
  lua_settop(L,1);
  return 1;
 }
 else
 {
  lua_pushnil(L);
  lua_pushstring(L,errorstring());
  return 2;
 }
}

static lua_State *LL=NULL;

static void errorhandler(const char *message)
{
 luaL_error(LL,"(gdbm) %s: %s",message,errorstring());
}

static GDBM_FILE Pget(lua_State *L, int i)
{
 errno=0;
 LL=L;
 return *((void**)luaL_checkudata(L,i,MYTYPE));
}

static int Lopen(lua_State *L)			/** open(file,mode) */
{
 const char* file=luaL_checkstring(L,1);
 const char* mode=luaL_optstring(L,2,"r");
 int flags;
 GDBM_FILE dbf;
 GDBM_FILE* p=lua_newuserdata(L,sizeof(GDBM_FILE));
 switch (*mode)
 {
  default:
  case 'r': flags=GDBM_READER;  break;
  case 'w': flags=GDBM_WRITER;  break;
  case 'c': flags=GDBM_WRCREAT; break;
  case 'n': flags=GDBM_NEWDB;   break;
 }
 for (; *mode; mode++)
 {
  switch (*mode)
  {
   case 'L': flags|=GDBM_NOLOCK; break;
   case 'M': flags|=GDBM_NOMMAP; break;
   case 'S': flags|=GDBM_SYNC;   break;
  }
 }
 LL=L;
 errno=0;
 dbf=gdbm_open((char*)file,0,flags,0666,errorhandler);
 if (dbf==NULL)
 {
  lua_pushnil(L);
  lua_pushfstring(L,"cannot open %s: %s",file,errorstring());
  return 2;
 }
 else
 {
  *p=dbf;
  luaL_setmetatable(L,MYTYPE);
  return 1;
 }
}

static int Lclose(lua_State *L)			/** close(file) */
{
 GDBM_FILE dbf=Pget(L,1);
 gdbm_close(dbf);
 lua_pushnil(L);
 lua_setmetatable(L,1);
 return 0;
}

static int Lreorganize(lua_State *L)		/** reorganize(file) */
{
 GDBM_FILE dbf=Pget(L,1);
 int rc=gdbm_reorganize(dbf);
 return pushresult(L,rc);
}

static int Lsync(lua_State *L)			/** sync(file) */
{
 GDBM_FILE dbf=Pget(L,1);
 gdbm_sync(dbf);
 lua_settop(L,1);
 return 1;
}

static int Lexists(lua_State *L)		/** exists(file,key) */
{
 GDBM_FILE dbf=Pget(L,1);
 datum key=encode(L,2);
 int rc=gdbm_exists(dbf,key);
 lua_pushboolean(L,rc);
 return 1;
}

static int Lfetch(lua_State *L)			/** fetch(file,key) */
{
 GDBM_FILE dbf=Pget(L,1);
 datum key=encode(L,2);
 datum dat=gdbm_fetch(dbf,key);
 return decode(L,dat);
}

static int Pstore(lua_State *L, int flags)
{
 GDBM_FILE dbf=Pget(L,1);
 datum key=encode(L,2);
 datum dat=encode(L,3);
 int rc=gdbm_store(dbf,key,dat,flags);
 return pushresult(L,rc);
}

static int Linsert(lua_State *L)		/** insert(file,key,data) */
{
 return Pstore(L,GDBM_INSERT);
}

static int Lreplace(lua_State *L)		/** replace(file,key,data) */
{
 return Pstore(L,GDBM_REPLACE);
}

static int Ldelete(lua_State *L)		/** delete(file,key) */
{
 GDBM_FILE dbf=Pget(L,1);
 datum key=encode(L,2);
 int rc=gdbm_delete(dbf,key);
 return pushresult(L,rc);
}

static int Lfirstkey(lua_State *L)		/** firstkey(file) */
{
 GDBM_FILE dbf=Pget(L,1);
 datum dat=gdbm_firstkey(dbf);
 return decode(L,dat);
}

static int Lnextkey(lua_State *L)		/** nextkey(file,key) */
{
 GDBM_FILE dbf=Pget(L,1);
 datum key=encode(L,2);
 datum dat=gdbm_nextkey(dbf,key);
 return decode(L,dat);
}

#ifdef GDBM_VERSION_MAJOR
static int Lexport(lua_State *L)		/** export(file,output) */
{
 GDBM_FILE dbf=Pget(L,1);
 const char* file=luaL_checkstring(L,2);
 int n=gdbm_export(dbf,file,GDBM_NEWDB,0666);
 return pushresult(L,-(n<0));
}

static int Limport(lua_State *L)		/** import(file,input,[replace]) */
{
 GDBM_FILE dbf=Pget(L,1);
 const char* file=luaL_checkstring(L,2);
 int replace=lua_toboolean(L,3);
 int n=gdbm_import(dbf,file, replace ? GDBM_REPLACE : GDBM_INSERT);
 return pushresult(L,-(n<0));
}
#endif

static int Ltostring(lua_State *L)
{
 GDBM_FILE p=Pget(L,1);
 lua_pushfstring(L,"%s %p",MYTYPE,(void*)p);
 return 1;
}

static const luaL_Reg R[] =
{
	{ "__gc",	Lclose		},
	{ "__tostring",	Ltostring	},	/** __tostring(file) */
	{ "close",	Lclose		},
	{ "delete",	Ldelete		},
	{ "exists",	Lexists		},
	{ "fetch",	Lfetch		},
	{ "firstkey",	Lfirstkey	},
	{ "insert",	Linsert		},
	{ "nextkey",	Lnextkey	},
	{ "open",	Lopen		},
	{ "reorganize",	Lreorganize	},
	{ "replace",	Lreplace	},
	{ "sync",	Lsync		},
#ifdef GDBM_VERSION_MAJOR
	{ "export",	Lexport		},
	{ "import",	Limport		},
#endif
	{ NULL,		NULL		}
};

LUALIB_API int luaopen_gdbm(lua_State *L)
{
 luaL_newmetatable(L,MYTYPE);
 luaL_setfuncs(L,R,0);
 lua_pushliteral(L,"version");			/** version */
 lua_pushliteral(L,MYVERSION);
 lua_pushstring(L,gdbm_version);
 lua_concat(L,2);
 lua_settable(L,-3);
 lua_pushliteral(L,"__index");
 lua_pushvalue(L,-2);
 lua_settable(L,-3);
 return 1;
}
