#include <stdio.h>

// intrinsics
#if defined(__GNUC__)    // GCC
#include <cpuid.h>
#elif defined(_MSC_VER)    // MSVC
    #if _MSC_VER >=1400    // VC2005
#include <intrin.h>
    #endif    // #if _MSC_VER >=1400
#else
#error Only supports MSVC or GCC.
#endif    // #if defined(__GNUC__)

void getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue)
{
#if defined(__GNUC__)    // GCC
    __cpuid_count(InfoType, ECXValue, CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#elif defined(_MSC_VER)    // MSVC
    #if defined(_WIN64) || _MSC_VER>=1600    // 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
        __cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
    #else
        if (NULL==CPUInfo)    return;
        _asm{
            // load. 读取参数到寄存器.
            mov edi, CPUInfo;    // 准备用edi寻址CPUInfo
            mov eax, InfoType;
            mov ecx, ECXValue;
            // CPUID
            cpuid;
            // save. 将寄存器保存到CPUInfo
            mov    [edi], eax;
            mov    [edi+4], ebx;
            mov    [edi+8], ecx;
            mov    [edi+12], edx;
        }
    #endif
#endif    // #if defined(__GNUC__)
}

void getcpuid(unsigned int CPUInfo[4], unsigned int InfoType)
{
#if defined(__GNUC__)    // GCC
    __cpuid(InfoType, CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#elif defined(_MSC_VER)    // MSVC
    #if _MSC_VER>=1400    // VC2005才支持__cpuid
        __cpuid((int*)(void*)CPUInfo, (int)InfoType);
    #else
        getcpuidex(CPUInfo, InfoType, 0);
    #endif
#endif    // #if defined(__GNUC__)
}

// 取得CPU厂商（Vendor）.
//
// result: 成功时返回字符串的长度（一般为12）。失败时返回0.
// pvendor: 接收厂商信息的字符串缓冲区。至少为13字节.
int cpu_getvendor(char* pvendor)
{
    unsigned int dwBuf[4];
    if (NULL==pvendor)    return 0;
    // Function 0: Vendor-ID and Largest Standard Function
    getcpuid(dwBuf, 0);
    // save. 保存到pvendor
    *(unsigned int *)&pvendor[0] = dwBuf[1];    // ebx: 前四个字符.
    *(unsigned int *)&pvendor[4] = dwBuf[3];    // edx: 中间四个字符.
    *(unsigned int *)&pvendor[8] = dwBuf[2];    // ecx: 最后四个字符.
    pvendor[12] = '\0';
    return 12;
}

// 取得CPU商标（Brand）.
//
// result: 成功时返回字符串的长度（一般为48）。失败时返回0.
// pbrand: 接收商标信息的字符串缓冲区。至少为49字节.
int cpu_getbrand(char* pbrand)
{
    unsigned int dwBuf[4];
    if (NULL==pbrand)    return 0;
    // Function 0x80000000: Largest Extended Function Number
    getcpuid(dwBuf, 0x80000000U);
    if (dwBuf[0] < 0x80000004U)    return 0;
    // Function 80000002h,80000003h,80000004h: Processor Brand String
    getcpuid((unsigned int *)&pbrand[0], 0x80000002U);    // 前16个字符.
    getcpuid((unsigned int *)&pbrand[16], 0x80000003U);    // 中间16个字符.
    getcpuid((unsigned int *)&pbrand[32], 0x80000004U);    // 最后16个字符.
    pbrand[48] = '\0';
    return 48;
}

#if 0
char szBuf[64];
unsigned int dwBuf[4];

int main(int argc, char* argv[])
{
    // test
    getcpuidex(dwBuf, 0,0);
    printf("1 %.8X\t%.8X\t%.8X\t%.8X\n", dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);
    getcpuid(dwBuf, 0);
    printf("2 %.8X\t%.8X\t%.8X\t%.8X\n", dwBuf[0],dwBuf[1],dwBuf[2],dwBuf[3]);

    cpu_getvendor(szBuf);
    printf("CPU Vendor:\t%s\n", szBuf);

    cpu_getbrand(szBuf);
    printf("CPU Name:\t%s\n", szBuf);

    return 0;
}
#else
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int lua_cpuid(lua_State*L) {
  unsigned int dwBuf[4];
  getcpuidex(dwBuf, 0,  0);
  lua_pushfstring(L,"%.8X-%.8X-%.8X-%.8X", dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);
  return 1;
}
#endif
