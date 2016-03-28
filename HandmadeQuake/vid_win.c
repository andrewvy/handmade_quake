#include "quakedef.h"
#include "winquake.h"

static int BufferWidth = 640;
static int BufferHeight = 480;
static int WindowWidth = 640;
static int WindowHeight = 480;
static int BytesPerPixel = 4;

HWND MainWindow;
void *BackBuffer = NULL;

typedef enum {
  MS_WINDOWED,
  MS_FULLSCREEN
} modestate_t;

typedef struct {
  modestate_t type;
  int32_t width;
  int32_t height;
  uint32_t Hz;
} vmode_t;

vmode_t ModeList[40];
int32_t GlobalModeIndex = 0;
int32_t FirstFullscreenMode = -1;

BITMAPINFO BitMapInfo = { 0 };

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  LRESULT Result = 0;

  switch (uMsg) {
  case WM_ACTIVATE:
  case WM_CREATE:
    break;
  case WM_DESTROY:
    break;
  case WM_KEYDOWN: {
    switch (wParam) {
      case 'A':
        VID_SetMode(0);
        break;
      case 'S':
        VID_SetMode(1);
        break;
      case 'D':
        VID_SetMode(2);
        break;
      case 'F':
        VID_SetMode(3);
        break;
      case '1':
        VID_SetMode(FirstFullscreenMode);
        break;
      case '2':
        VID_SetMode(FirstFullscreenMode + 1);
        break;
      case 'Q':
        Sys_Shutdown();
        break;
    }
  } break;
  default:
    Result = DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return Result;
}

void VID_InitWindowedMode() {
  ModeList[GlobalModeIndex].type = MS_WINDOWED;
  ModeList[GlobalModeIndex].width = 320;
  ModeList[GlobalModeIndex].height = 240;
  ModeList[GlobalModeIndex].Hz = 0;
  GlobalModeIndex++;

  ModeList[GlobalModeIndex].type = MS_WINDOWED;
  ModeList[GlobalModeIndex].width = 640;
  ModeList[GlobalModeIndex].height = 480;
  ModeList[GlobalModeIndex].Hz = 0;
  GlobalModeIndex++;

  ModeList[GlobalModeIndex].type = MS_WINDOWED;
  ModeList[GlobalModeIndex].width = 800;
  ModeList[GlobalModeIndex].height = 600;
  ModeList[GlobalModeIndex].Hz = 0;
  GlobalModeIndex++;

  ModeList[GlobalModeIndex].type = MS_WINDOWED;
  ModeList[GlobalModeIndex].width = 1024;
  ModeList[GlobalModeIndex].height = 768;
  ModeList[GlobalModeIndex].Hz = 0;
  GlobalModeIndex++;
}

void VID_InitFullscreenMode() {
  DEVMODE Device;
  int ModeIndex = 0;
  BOOL Success = 0;
  int OldWidth = 0, OldHeight = 0;

  // Before populating our video mode list, keep track
  // of the first full screen mode index in the global
  // video mode list
  FirstFullscreenMode = GlobalModeIndex;

  do {
    Success = EnumDisplaySettings(NULL, ModeIndex, &Device);

    if (Device.dmPelsHeight == OldHeight && Device.dmPelsWidth == OldWidth) {
      // If a mode resolution has a higher frequency, use that one instead.
      if (Device.dmDisplayFrequency > ModeList[GlobalModeIndex - 1].Hz)
        ModeList[GlobalModeIndex].Hz = Device.dmDisplayFrequency;

      ModeIndex++;
    } else {
      ModeList[GlobalModeIndex].type = MS_FULLSCREEN;
      ModeList[GlobalModeIndex].width = Device.dmPelsWidth;
      ModeList[GlobalModeIndex].height = Device.dmPelsHeight;
      ModeList[GlobalModeIndex].Hz = Device.dmDisplayFrequency;
      GlobalModeIndex++;

      OldHeight = Device.dmPelsHeight;
      OldWidth = Device.dmPelsWidth;
      ModeIndex++;
    }

  } while (Success);
}

void VID_SetWindowedMode(int ModeIndex) {
  DWORD WindowExStyle = 0;
  DWORD WindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

  RECT r = { 0 };
  r.right = WindowWidth;
  r.bottom = WindowHeight;

  AdjustWindowRectEx(&r, WindowStyle, 0, WindowExStyle);

  // Create Window
  MainWindow = CreateWindowEx(
    WindowExStyle,
    "Module 3",
    "Lesson 3.6",
    WindowStyle,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    r.right - r.left,
    r.bottom - r.top,
    NULL,
    NULL,
    GlobalInstance,
    0
  );
}

void VID_SetFullscreenMode(int ModeIndex) {
  DWORD WindowExStyle = 0;
  DWORD WindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

  DEVMODE dmScreenSettings = { 0 };
  dmScreenSettings.dmSize = sizeof(dmScreenSettings);
  dmScreenSettings.dmPelsWidth = BufferWidth;
  dmScreenSettings.dmPelsHeight = BufferHeight;
  dmScreenSettings.dmBitsPerPel = 32;
  dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

  if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
    WindowExStyle = WS_EX_APPWINDOW;
    WindowStyle = WS_POPUP;
  }

  RECT r = { 0 };
  r.right = WindowWidth;
  r.bottom = WindowHeight;

  AdjustWindowRectEx(&r, WindowStyle, 0, WindowExStyle);

  // Create Window
  MainWindow = CreateWindowEx(
    WindowExStyle,
    "Module 3",
    "Lesson 3.6",
    WindowStyle,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    r.right - r.left,
    r.bottom - r.top,
    NULL,
    NULL,
    GlobalInstance,
    0
  );

  SetWindowLong(MainWindow, GWL_STYLE, 0);
}

void VID_SetMode(int ModeIndex) {
  if (BackBuffer) {
    VID_Shutdown();
  }

  BufferWidth = WindowWidth = ModeList[ModeIndex].width;
  BufferHeight = WindowHeight = ModeList[ModeIndex].height;

  if (ModeList[ModeIndex].type == MS_WINDOWED) {
    VID_SetWindowedMode(ModeIndex);
  } else {
    VID_SetFullscreenMode(ModeIndex);
  }

  ShowWindow(MainWindow, SW_SHOWDEFAULT);

  // Define BitMapInfo
  BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
  BitMapInfo.bmiHeader.biWidth = BufferWidth;
  BitMapInfo.bmiHeader.biHeight = -BufferHeight;
  BitMapInfo.bmiHeader.biPlanes = 1;
  BitMapInfo.bmiHeader.biBitCount = 8 * BytesPerPixel;
  BitMapInfo.bmiHeader.biCompression = BI_RGB;

  BackBuffer = malloc(BufferWidth * BufferHeight * BytesPerPixel);
}

void VID_Init(void) {
  WNDCLASSEX wc = { 0 };
  wc.cbSize = sizeof(wc);
  wc.lpfnWndProc = MainWndProc;
  wc.hInstance = GlobalInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.lpszClassName = "Module 3";

  if (!RegisterClassExA(&wc))
    exit(EXIT_FAILURE);

  VID_InitWindowedMode();
  VID_InitFullscreenMode();
  VID_SetMode(0);
}

void VID_Update(void) {
  // Acquire the DeviceContext for the Main Window.
  HDC DeviceContext = GetDC(MainWindow);

  // Apply BackBuffer
  StretchDIBits(
    DeviceContext,
    0, 0, WindowWidth, WindowHeight,
    0, 0, BufferWidth, BufferHeight,
    BackBuffer, (BITMAPINFO*) &BitMapInfo,
    DIB_RGB_COLORS, SRCCOPY
  );

  // Release DeviceContext
  ReleaseDC(MainWindow, DeviceContext);
}

void VID_Shutdown(void) {
  ChangeDisplaySettings(NULL, 0);
  DestroyWindow(MainWindow);
  free(BackBuffer);
  BackBuffer = NULL;
}