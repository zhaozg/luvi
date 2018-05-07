#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

struct bit {
  unsigned char b : 1;				/* defines ONE bit */
};				                    /* BIT FIELD STRUCTURE      */

typedef struct bit BitField;

struct inputCheck {
  BitField*   bits;				    /* Pointer to BitField sequence          */
  int         bitsNumber;		  /* Number of bits in the 'bits' sequence */
};

typedef struct inputCheck           BITS_CHECKED;

double cephes_igamc(double a, double x);

static double poker(BITS_CHECKED* pBits, int m) {
  int nBlocks = (int)floor((double)pBits->bitsNumber / (double)m);
  int nSize = (int)pow(2, m);
	
  long* n = (long*)calloc(nSize, sizeof(long));
	if(n!=NULL) {
    int i, j;
    double sum = 0.0;
    double dPValue = 0.0;

    memset(n, 0, sizeof(long) * nSize);
    for (i = 0; i < nBlocks; ++i) {
      long decRep = 0;
      for (j = 0; j < m; j++)
      {
        decRep += pBits->bits[(i - 1)*m + j].b * (long)pow(2, m - 1 - j);
      }
      ++n[decRep];
    }

    for (i = 0; i < nSize; ++i) {
      sum += pow(n[i], 2);
    }

    sum = nSize / (double)pBits->bitsNumber * sum;
    sum = sum - (double)pBits->bitsNumber;
    dPValue = cephes_igamc(((nSize - 1) / 2), sum / 2);

    free(n);
    return dPValue;
	}
  return 0;
}

int lua_pokercheck(lua_State*L) {
  size_t len;
  const char* data = luaL_checklstring(L, 1, &len);
  int m = luaL_checkint(L, 2);
  double g_dSigLevel = lua_tonumber(L, 3);

  double dValue;
  unsigned int nBitsRead = 0;

  size_t i, j;
  unsigned char bit = 0;
  unsigned char mask = 1 << 7;
  unsigned int n0 = 0, n1 = 0;

  luaL_argcheck(L, m == 4 || m == 8, 2, "only accept number 4 or 8");

  BITS_CHECKED seq = { 0 };
  seq.bitsNumber = len * 8;

  seq.bits = (BitField *)calloc((len * 8), sizeof(BitField));
  if (seq.bits == NULL) {
    luaL_error(L, "out of memory");
    return 0;
  }

  for (i = 0; i < len; ++i) {
    unsigned char ucByte = ((unsigned char*)data)[i];

    for (j = 0; j < 8; ++j) {
      if (ucByte & mask) {
        bit = 1;
        ++n1;
      } else {
        bit = 0;
        ++n0;
      }

      ucByte <<= 1;
      seq.bits[nBitsRead].b = bit;

      ++nBitsRead;
    }
  }

  dValue = poker(&seq, m);
  free(seq.bits);

  lua_pushboolean(L, dValue > g_dSigLevel);
  lua_pushnumber(L, dValue);
  lua_pushnumber(L, g_dSigLevel);
  return 3;
}
