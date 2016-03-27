#pragma once

#define MAX_NUM_ARGVS 50 

#undef true
#undef false

typedef enum { false, true } qboolean;

extern int32_t com_argc;
extern uint8_t *com_argv[MAX_NUM_ARGVS + 1];

void COM_ParseCmdLine(uint8_t *lpCmdLine);
int32_t COM_CheckParm(uint8_t *parm);