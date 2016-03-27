#include "quakedef.h"
#include "winquake.h"

static int BufferWidth = 640;
static int BufferHeight = 480;
static int WindowWidth = 640;
static int WindowHeight = 480;
static int BytesPerPixel = 4;

HWND MainWindow;

void *BackBuffer = NULL;

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
        VID_SetMode(640, 480);
        break;
      case 'S':
        VID_SetMode(800, 600);
        break;
      case 'D':
        VID_SetMode(1024, 768);
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

void VID_SetMode(int Width, int Height) {
  if (BackBuffer) {
    VID_Shutdown();
  }

  BufferWidth = WindowWidth = Width;
  BufferHeight = WindowHeight = Height;

  BOOL Fullscreen = FALSE;
  DWORD WindowExStyle = 0;
  DWORD WindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

  if (Fullscreen) {
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
  }

  RECT r = { 0 };
  r.right = WindowWidth;
  r.bottom = WindowHeight;

  AdjustWindowRectEx(&r, WindowStyle, 0, WindowExStyle);

  // Create Window
  MainWindow = CreateWindowEx(
    WindowExStyle,
    "Module 3",
    "Lesson 3.5",
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

  ShowWindow(MainWindow, SW_SHOWDEFAULT);

  // Define BitMapInfo
  BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
  BitMapInfo.bmiHeader.biWidth = BufferWidth;
  BitMapInfo.bmiHeader.biHeight = -BufferHeight;
  BitMapInfo.bmiHeader.biPlanes = 1;
  BitMapInfo.bmiHeader.biBitCount = 8 * BytesPerPixel;
  BitMapInfo.bmiHeader.biCompression = BI_RGB;

  BackBuffer = malloc(BufferWidth * BufferHeight * BytesPerPixel);

  HDC DeviceContext = GetDC(MainWindow);
  PatBlt(DeviceContext, 0, 0, BufferWidth, BufferHeight, BLACKNESS);
  ReleaseDC(MainWindow, DeviceContext);
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

  VID_SetMode(WindowWidth, WindowHeight);
}

void VID_Update(void) {
  HDC dc = GetDC(MainWindow);
  StretchDIBits(
    dc,
    0, 0, WindowWidth, WindowHeight,
    0, 0, BufferWidth, BufferHeight,
    BackBuffer, (BITMAPINFO*) &BitMapInfo,
    DIB_RGB_COLORS, SRCCOPY
  );

  ReleaseDC(MainWindow, dc);
}

void VID_Shutdown(void) {
  DestroyWindow(MainWindow);
  free(BackBuffer);
  BackBuffer = NULL;
}