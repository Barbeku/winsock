//gcc -o output.exe server.c -lws2_32 && output.exe
//https://learn.microsoft.com/ru-ru/windows/win32/api/winsock2/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>

#define FIELD_WIDTH 30
#define FIELD_HEIGHT 15

#define PLAYER_WIDTH 1
#define PLAYER_HEIGHT 2

#define CELL_SIZE 30

#include "game.h"

#define MAXLEN 10
char buf[MAXLEN];
char buf2[MAXLEN];

char playerPos = (int)(FIELD_HEIGHT / 2);

int iterTime = 50;
HWND window;
char text[MAXLEN];

void trackMove();

int main(){
//  system("cls");
  window = GetForegroundWindow();

  printf("active window: %d\n", window);

  unsigned long long prev = 0;

  WSADATA ws;
  WSAStartup(MAKEWORD(2, 2), &ws); //WSAStartup - инициализирует испльзование сокетов(version, &ws)

  SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

  SOCKADDR_IN sa;

  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(1234);

  printf("s - server, c - client\n");
  char c = getchar();

  HWND hwnd;
  HDC dc;
  MSG msg;
  initWindow(&hwnd, &dc);

  //-------------------------------------CLIENT--------------------------------------
  if(c == 'c'){
//    char *string;
//    scanf("%s\0", string);
//    printf("%s, %lld\n", string, inet_addr(string));
    char *string = "127.0.0.1";
    sa.sin_addr.S_un.S_addr = inet_addr(string);

    connect(s, (struct sockaddr*)&sa, sizeof(sa));

    printf("conn\n");

    int iResult = 1;
    while(1){
      if(clock() - prev > iterTime){
        trackMove();

        send(s, buf, MAXLEN, 0);
        iResult = recv(s, buf2, MAXLEN, 0);

        if(iResult > 0)
//          printf("%d, %d\n", buf2[0], playerPos);

        doGame(dc, buf2[0], playerPos);

        PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
        DispatchMessage(&msg);
        prev =  clock();
      }
    }
  }

  //-------------------------------------SERVER--------------------------------------
  else if(c == 's'){
    bind(s, (struct sockaddr*)&sa, sizeof(sa));

    listen(s, 100);

    SOCKET clientSocket;
    SOCKADDR_IN clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    if(clientSocket = accept(s, (struct sockaddr*)&clientAddr, &clientAddrSize)){
      printf("client addr: %lld\n", clientAddr.sin_addr.S_un.S_addr);

      int iResult = 1;
      while(iResult > 0){
        iResult = recv(clientSocket, buf, MAXLEN, 0);

        buf2[0] = playerPos;
        send(clientSocket, buf2, MAXLEN, 0);

        if(iResult > 0){
          doGame(dc, playerPos, buf[0]);
        }

        if(clock() - prev > iterTime){
          trackMove();

          prev = clock();
        }
        PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
        DispatchMessage(&msg);
      }
    }
  }

  return 0;
}

void trackMove(){
  HWND hwndd = GetForegroundWindow();
  if(hwndd != window) return;

  if     (GetKeyState(VK_UP) < 0)   playerPos--;
  else if(GetKeyState(VK_DOWN) < 0) playerPos++;

  if     (playerPos < 0) playerPos = 0;
  else if(playerPos + PLAYER_HEIGHT >= FIELD_HEIGHT) playerPos = FIELD_HEIGHT-PLAYER_HEIGHT-1;

  buf[0] = playerPos;
  buf[1] = '\n';
}

