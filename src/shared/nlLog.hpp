#pragma once

#ifndef INTROMODE
#define NL_LOG(fmt, ...)\
    QString __buffer;\
    __buffer.sprintf(fmt"[%s:%u]", __VA_ARGS__,__FILE__,__LINE__);\
    __buffer.replace("\n","");\
    QByteArray __ba = __buffer.toLocal8Bit();\
    logInfo(__ba);

#define NL_ERR(fmt, ...)\
    QString __buffer;\
    __buffer.sprintf(fmt"%s[%s:%u]", __VA_ARGS__,__FILE__,__LINE__);\
    __buffer.replace("\n","");\
    QByteArray __ba = __buffer.toLocal8Bit();\
    logError( __ba );

#define NL_ERR2(fmt, ...)\
    HANDLE __hStdOut = (HANDLE) GetStdHandle( STD_OUTPUT_HANDLE );\
    CONSOLE_SCREEN_BUFFER_INFO __csbi;\
    ZeroMemory( &__csbi, sizeof(CONSOLE_SCREEN_BUFFER_INFO) );\
    GetConsoleScreenBufferInfo ( __hStdOut, &__csbi);\
    SetConsoleTextAttribute ( __hStdOut, FOREGROUND_RED | FOREGROUND_INTENSITY );\
    NL_LOG(fmt, __VA_ARGS__);\
    SetConsoleTextAttribute ( __hStdOut, __csbi.wAttributes );\

#else
#define NL_LOG(fmt, ...)
#define NL_ERR(fmt, ...)
#endif

/* èÓïÒ */
#define INF_001 "I001:getId() [%s:%s]"
#define INF_002 "I002:clearId() [%s]"
#define INF_003 "I003:setId() [%s:%s]"
#define INF_004 "I004:setId() [%s]@[%s]"
#define INF_005 "I005:import scene(num node:%d)"
#define INF_006 "I006:changed resource file[%s]"
#define INF_007 "I007:copyed %d items."

/* åxçê */
#define WAR_000 "W000:changed id!! prop:[%s] (id:%d->%d)"

/* ÉGÉâÅ[ */
#define ERR_001 "E001:failed setId() cant find group[%s]"
#define ERR_002 "E002:failed setId() tag [%s] have been reserved."
#define ERR_003 "E003:ps error[%s]"
#define ERR_004 "E004:failed clear() cant find group[%s]"
#define ERR_005 "E005:failed load vertex shader[%s]"
#define ERR_006 "E006:failed load pixel shader[%s]"
#define ERR_008 "E008:failed setId() tag [%s] have been reserved."
#define ERR_009 "E009:load texshader error"
#define ERR_010 "E010:failed load[%s]"
#define ERR_011 "E011:cant find id[%s@%s]"
#define ERR_012 "E012:cant find idGroup[%s]"
#define ERR_013 "E013:failed exec command[%s]"
#define ERR_014 "E014:failed resolve node name[%s]"
#define ERR_015 "E015:unknown RT format[%s]"
#define ERR_016 "E016:unknown RT size[%s]"
#define ERR_017 "E017:unmatch target size@SetRenderTarget rtNo[%d] commandId[%d] DS[%d:%d] RT[%d:%d]"
#define ERR_018 "E018:failed load PS[%s]"
#define ERR_019 "E019:failed exec command[%s]"
#define ERR_020 "E020:failed load path[%s]"
#define ERR_021 "E021:something wrong arond getNodeIdx()"
#define ERR_022 "E022:failed load path[%d][%s]"
#define ERR_023 "E023:failed open file[%s]"
#define ERR_024 "E024:curve is empty[%s]"
#define ERR_025 "E025:not sorted node."
#define ERR_026 "E026:failed load GS[%s]"
