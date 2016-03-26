#include "quakedef.h"
#include "winquake.h"

int32_t CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int32_t nCmdShow) {
  COM_ParseCmdLine(lpCmdLine);

  int32_t test = COM_CheckParm("-setalpha");
  int32_t value = Q_atoi(com_argv[test + 1]);

  return 0;
}