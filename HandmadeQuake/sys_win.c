#include "quakedef.h"
#include "winquake.h"

static BOOL IsRunning = TRUE;

HINSTANCE GlobalInstance;

static double GTimePassed = 0;
static double SecondsPerTick = 0;
static __int64 GTimeCount = 0;

float Sys_InitFloatTime(void) {
  LARGE_INTEGER Frequency;
  LARGE_INTEGER Counter;

  QueryPerformanceFrequency(&Frequency);
  SecondsPerTick = 1.0 / (double)Frequency.QuadPart;
  QueryPerformanceCounter(&Counter);
  GTimeCount = Counter.QuadPart;

  return 0;
}

float Sys_FloatTime(void) {
  LARGE_INTEGER Counter;
  QueryPerformanceCounter(&Counter);

  __int64 Interval = Counter.QuadPart - GTimeCount;
  GTimeCount = Counter.QuadPart;

  double SecondsGoneBy = (double)Interval * SecondsPerTick;
  GTimePassed += SecondsGoneBy;

  return (float)GTimePassed;
}

void Sys_SendKeyEvents(void) {
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void Sys_Shutdown(void) {
  IsRunning = FALSE;
}

int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
  GlobalInstance = hInstance;

  COM_ParseCmdLine(lpCmdLine);

  Host_Init();

  float oldtime = Sys_InitFloatTime();
  while (IsRunning) {
    float newtime = Sys_FloatTime();
    Host_Frame(newtime - oldtime);
    oldtime = newtime;
  }

  Host_Shutdown();

  return 0;
}