#include "quakedef.h"

int32_t com_argc = 1;
uint8_t *com_argv[MAX_NUM_ARGVS + 1];

int32_t COM_CheckParm(uint8_t *parm) {
  for (int i = 1; i < com_argc; i++) {
    if (!Q_strcmp(parm, com_argv[i]))
      return i;
  }

  return 0;
}

void COM_ParseCmdLine(uint8_t *lpCmdLine) {
  com_argv[0] = "";

  while (*lpCmdLine && (com_argc < MAX_NUM_ARGVS + 1)) {
    while (*lpCmdLine && ((*lpCmdLine <= 32) || (*lpCmdLine > 126)))
      lpCmdLine++;

    if (*lpCmdLine) {
      com_argv[com_argc] = lpCmdLine;
      com_argc++;

      while (*lpCmdLine && ((*lpCmdLine > 32) && (*lpCmdLine <= 126)))
        lpCmdLine++;
      
      if (*lpCmdLine) {
        *lpCmdLine = 0;
        lpCmdLine++;
      }
    }
  }
}