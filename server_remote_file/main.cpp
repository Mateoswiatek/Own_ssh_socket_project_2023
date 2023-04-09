#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
// #pragma comment(lib,"ws2_32.lib")
// definiujemy sobie parametry
#define IP_SERV "192.168.56.1"
#define Port_SERV 9000

int main() {
    WSADATA wsaData;
    int result = WSAStartup( MAKEWORD(2,2), &wsaData); // używamy wersji 2.2
    if(result != NO_ERROR) printf( "Initialization error.\n" ); // jeśli się nie udało to dostajemy komunikat

    SOCKET mainSocket = socket( AF_INET, SOCK_STREAM, 0 ); // IPPROTO_TCP
    if( mainSocket == -1 ){
        printf( "Error creating socket: %d\n", WSAGetLastError() ); // wypisze nam ostatni blad
        WSACleanup(); // "sprzątanie" po WSA
        return 0;
    }




    std::cout << "Hello, World!" << std::endl;
    return 0;
}

