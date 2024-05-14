#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 5000
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char **argv) 
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    const char *sendbuf = "//gcc -o output.exe server.c -lws2_32 && output.exe\n//https://learn.microsoft.com/ru-ru/windows/win32/api/winsock2/\n#include <stdio.h>\n#include <unistd.h>\n#include <stdlib.h>\n#include <time.h>\n#include <winsock2.h>\n\n#define MAXLEN 10\nchar buf[MAXLEN];\n\n#define FIELD_SIZE 10\nchar playerPos = (int)(FIELD_SIZE / 2);\n\nint iterTime = 50;\n\nvoid trackMove();\n\nHWND window;\n\nchar text[MAXLEN];\n\nint main(){\n  window = GetForegroundWindow();\n\n  printf(\"active window: %d\n\", window);\n\n  unsigned long long prev = 0;\n\n  WSADATA ws;\n  WSAStartup(MAKEWORD(2, 2), &ws); //WSAStartup - инициализирует испльзование сокетов(version, &ws)\n\n  SOCKET s = socket(AF_INET, SOCK_STREAM, 0);\n\n  SOCKADDR_IN sa;\n\n  memset(&sa, 0, sizeof(sa));\n  sa.sin_family = AF_INET;\n  sa.sin_port = htons(1234);\n\n  printf(\"s - server, c - client\n\");\n  char c = getchar();\n\n  //-------------------------------------CLIENT----------------------------------------------------\n  if(c == 'c'){\n    sa.sin_addr.S_un.S_addr = inet_addr(\"127.0.0.1\");\n\n    connect(s, (struct sockaddr*)&sa, sizeof(sa));\n\n    printf(\"conn\n\");\n    sprintf(buf, \"some\n\");\n\n    while(1){\n      if(clock() - prev > iterTime){\n        trackMove();\n        send(s, buf, MAXLEN, 0);\n        prev =  clock();\n      }\n    }\n  }\n\n  //-------------------------------------SERVER----------------------------------------------------\n  else if(c == 's'){\n    bind(s, (struct sockaddr*)&sa, sizeof(sa));\n\n    listen(s, 100);\n\n    SOCKET clientSocket;\n    SOCKADDR_IN clientAddr;\n    int clientAddrSize = sizeof(clientAddr);\n\n    if(clientSocket = accept(s, (struct sockaddr*)&clientAddr, &clientAddrSize)){\n      printf(\"connect\n\");\n      int iResult = 1;\n      while(iResult > 0){\n        iResult = recv(clientSocket, buf, MAXLEN, 0);\n        if(iResult > 0)\n          printf(\"%d, %d\n\", playerPos, buf[0]);\n        if(clock() - prev > iterTime){\n          trackMove();\n          prev = clock();\n        }\n      } \n    }\n  }\n\n  return 0;\n}\n\nvoid trackMove(){\n  HWND hwnd = GetForegroundWindow();\n  if(hwnd != window) return;\n\n  if     (GetKeyState(VK_UP) < 0)   playerPos++;\n  else if(GetKeyState(VK_DOWN) < 0) playerPos--;\n\n  if     (playerPos < 0) playerPos = 0;\n  else if(playerPos >= FIELD_SIZE) playerPos = FIELD_SIZE-1;\n\n  buf[0] = playerPos;\n  buf[1] = '\n';\n}\n` ";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("5, getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 )
            printf("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( iResult > 0 );

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
