#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;


// #pragma comment(lib,"ws2_32.lib")
// definiujemy sobie parametry
// #define IP_SERV "192.168.56.1"
// #define Port_SERV 9000

void init_socket(const string IP, int Port){
    sockaddr_in service;
    memset( & service, 0, sizeof( service ) ); 
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr( "192.168.56.1" );
    service.sin_port = htons( Port );
}

int main() {
    string IP_serv = "192.168.56.1";
    int Port = 9000;

    WSADATA wsaData;
    int result = WSAStartup( MAKEWORD(2,2), &wsaData);
    if(result != NO_ERROR) printf( "Initialization error.\n" );

    SOCKET mainSocket = socket( AF_INET, SOCK_STREAM, 0 );
    if( mainSocket == -1 ){
        printf( "Error creating socket: %d\n", WSAGetLastError() );
        WSACleanup(); // "sprzątanie" po WSA
        return 0;
    }
    init_socket(IP_serv, Port);
    

    std::cout << "Hello, World!" << std::endl;

    WSACleanup(); // sprzątamy na koniec całego programu
    return 0;
}


