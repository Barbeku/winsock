#include <windows.h>

#define WINDOW_HEIGHT 300
#define WINDOW_WIDTH 700

int isClose = 0;
RECT rct;

LRESULT megaMiddleware(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam){
  switch(message){
    case WM_DESTROY:
      PostQuitMessage(0);
      isClose = 1;
      break;
    case WM_SIZE:
      GetClientRect(hwnd, &rct);
      break;
    default:
      return DefWindowProcA(hwnd, message, wparam, lparam);
      break;
  }
}

void drawField(HDC, int, int);

void initWindow();


void doGame(HDC dc, int player1Pos, int player2Pos){
  drawField(dc, player1Pos, player2Pos);
}

void clearField(HDC dc){
  SelectObject(dc, GetStockObject(DC_BRUSH));
  SetDCBrushColor(dc, RGB(255, 255, 255));

  Rectangle(dc, rct.left, rct.top, rct.right, rct.bottom);
}


void drawPlayer(HDC dc, int x, int y){
  SelectObject(dc, GetStockObject(DC_BRUSH));
  SetDCBrushColor(dc, RGB(0, 0, 0));

  Rectangle(dc, x*CELL_SIZE, y*CELL_SIZE, x*CELL_SIZE + PLAYER_HEIGHT*CELL_SIZE, y*CELL_SIZE+PLAYER_WIDTH*CELL_SIZE);
}

void drawField(HDC dc, int player1Pos, int player2Pos){
  HDC memDC = CreateCompatibleDC(dc);
  HBITMAP memBM = CreateCompatibleBitmap(dc, rct.right - rct.left, rct.bottom - rct.top);
  SelectObject(memDC, memBM);

  clearField(memDC);
//  printf("%d, %d\n", player1Pos, player2Pos);

  drawPlayer(memDC, 0, player1Pos);
  drawPlayer(memDC, FIELD_WIDTH-1, player2Pos);

  BitBlt(dc, 0, 0, rct.right - rct.left, rct.bottom - rct.top, memDC, 0, 0, SRCCOPY);
  DeleteDC(memDC);
  DeleteObject(memBM);
}


void initWindow(HWND* hwnd, HDC* dc){
  WNDCLASSA wcl;

  memset(&wcl, 0, sizeof(wcl));
  wcl.lpszClassName = "my window";
  wcl.lpfnWndProc = megaMiddleware;

  RegisterClassA(&wcl);

  *hwnd = CreateWindow(
      "my window",
      "hello w",
      WS_OVERLAPPEDWINDOW,
      10, 10,
      WINDOW_WIDTH,
      WINDOW_HEIGHT,
      NULL, NULL, NULL, NULL
  );
  *dc = GetDC(*hwnd);

  ShowWindow(*hwnd, SW_SHOWNORMAL);
}
