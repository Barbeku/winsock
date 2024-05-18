//gcc -o output.exe server.c -lws2_32 && output.exe
//https://learn.microsoft.com/ru-ru/windows/win32/api/winsock2/
#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>

#define FIELD_WIDTH 30
#define FIELD_HEIGHT 15

#define PLAYER_WIDTH 1
#define PLAYER_HEIGHT 3

#define CELL_SIZE 10

#include "game.h"

#define MAXLEN 10
char buf[MAXLEN];
char buf2[MAXLEN];

char playerPos = (int)(FIELD_HEIGHT / 2);
char enemyPos = (int)(FIELD_HEIGHT / 2);

unsigned long long prev = 0;
int iterTime = 20;
HWND window;
char text[MAXLEN];

void trackMove();

HWND hwnd;
HDC dc;
MSG msg;

SOCKET socketToWork;

int gameIterTime = 10;

DWORD WINAPI ThreadServer(){
  while(1){
    if(clock() - prev > iterTime){
      trackMove();
      prev = clock();
    }

    buf2[0] = playerPos;

    send(socketToWork, buf2, MAXLEN, 0);
    recv(socketToWork, buf, MAXLEN, 0);

    enemyPos = buf[0];
    printf("%d, %d\n", playerPos, enemyPos);
  }
  return 0;
}

DWORD WINAPI ThreadClient(){
  int iResult = 1;
  while(1){
    if(clock() - prev > iterTime){
      trackMove();
      prev =  clock();
    }

    send(socketToWork, buf, MAXLEN, 0);
    recv(socketToWork, buf2, MAXLEN, 0);

    enemyPos = buf2[0];

    printf("%d, %d\n", playerPos, enemyPos);

  }
  return 0;
}

DWORD WINAPI ThreadGame(){
  while (1) {
    doGame(dc, playerPos, enemyPos);
    PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
    DispatchMessage(&msg);

    Sleep(gameIterTime);
  }
  return 0;
}



int main(){
  HANDLE hThread1, hThread2;
  DWORD dwThreadId1, dwThreadId2;

  //  system("cls");
  window = GetForegroundWindow();

  printf("active window: %d\n", window);


  WSADATA ws;
  WSAStartup(MAKEWORD(2, 2), &ws); //WSAStartup - инициализирует испльзование сокетов(version, &ws)

  SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

  SOCKADDR_IN sa;

  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(1234);

  printf("s - server, c - client\n");
  char c = getchar();
  initWindow(&hwnd, &dc);

  //-------------------------------------CLIENT--------------------------------------
  if(c == 'c'){
    char ipAddres[100];
    scanf("%s\0", ipAddres);
//    char *string = "127.0.0.1";
    sa.sin_addr.S_un.S_addr = inet_addr(ipAddres);

    connect(s, (struct sockaddr*)&sa, sizeof(sa));

    socketToWork = s;
    hThread1 = CreateThread(NULL, 0, ThreadClient, NULL, 0, &dwThreadId1);

    printf("conn\n");

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

      socketToWork = clientSocket;
      hThread1 = CreateThread(NULL, 0, ThreadServer, NULL, 0, &dwThreadId1);
    }
  }
  hThread2 = CreateThread(NULL, 0, ThreadGame, NULL, 0, &dwThreadId2);

  if (hThread1 == NULL || hThread2 == NULL) {
      printf("Failed to create thread\n");
      return 1;
  }

  WaitForSingleObject(hThread1, INFINITE);
  WaitForSingleObject(hThread2, INFINITE);

  CloseHandle(hThread1);
  CloseHandle(hThread2);

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

