//gcc -o output.exe server.c -lws2_32 && output.exe
//https://learn.microsoft.com/ru-ru/windows/win32/api/winsock2/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>

#define MAXLEN 10
char buf[MAXLEN];

#define FIELD_SIZE 10
char playerPos = (int)(FIELD_SIZE / 2);

int iterTime = 50;

void trackMove();

HWND window;

char text[MAXLEN];

int main(){
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

  //-------------------------------------CLIENT--------------------------------------
  if(c == 'c'){
//    sa.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    char *string;
    scanf("%s\0", string);
    printf("%s, %lld\n", string, inet_addr(string));
    sa.sin_addr.S_un.S_addr = inet_addr(string);

    connect(s, (struct sockaddr*)&sa, sizeof(sa));

    printf("conn\n");
    sprintf(buf, "some\n");

    while(1){
      if(clock() - prev > iterTime){
        trackMove();
        send(s, buf, MAXLEN, 0);
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
      printf("connect\n");
//    sa.sin_addr.S_un.S_addr = inet_addr("192.168.110.150");

//      printf("connect type: %d\n", clientAddr.sin_addr.S_un.S_addr);
      printf("connect type: %lld\n", clientAddr.sin_addr.S_un.S_addr);

      int iResult = 1;
      while(iResult > 0){
        iResult = recv(clientSocket, buf, MAXLEN, 0);
        if(iResult > 0)
//          printf("%d, %d\n", playerPos, buf[0]);
          ;
        if(clock() - prev > iterTime){
          trackMove();
          prev = clock();
        }
      } 
    }
  }

  return 0;
}

void trackMove(){
  HWND hwnd = GetForegroundWindow();
  if(hwnd != window) return;

  if     (GetKeyState(VK_UP) < 0)   playerPos++;
  else if(GetKeyState(VK_DOWN) < 0) playerPos--;

  if     (playerPos < 0) playerPos = 0;
  else if(playerPos >= FIELD_SIZE) playerPos = FIELD_SIZE-1;

  buf[0] = playerPos;
  buf[1] = '\n';
}
