#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;


// #pragma comment(lib,"ws2_32.lib")
// definiujemy sobie parametry
// #define IP_SERV "192.168.56.1"
// #define Port_SERV 9000

int main() {
    string IP_serv = "192.168.56.1";
    int Port = 9000;

    SOCKET gniazdo_root, gniazdo1;
    int status, dlugosc;
    struct sockaddr_in server, client; // instancje struktury które przechowują dane
    char buf[1024];

    WSADATA wsaData;
    int result = WSAStartup( MAKEWORD(2,2), &wsaData); // używamy wersji 2.2
    if(result != NO_ERROR) cout << "Initialization error.\n"; // jeśli się nie udało to dostajemy komunikat

    gniazdo_root = socket( AF_INET, SOCK_STREAM, 0 ); // IPv4, IPPROTO_TCP
    if( gniazdo_root == SOCKET_ERROR ){ // jeśli jest jakiś błąd
        cout << "Initialization error.\n Error creating socket: %d\n" << WSAGetLastError() << endl; // wypisze nam ostatni blad
        WSACleanup(); // "sprzątanie" po WSA
        return 0;
    }

    memset( & server, 0, sizeof( server ) ); // pamięć na 0, są domyślne wartości w polach struktury
    server.sin_family = AF_INET; // używamy protokołu IPv4
    server.sin_addr.s_addr = inet_addr( "192.168.56.1" );
    server.sin_port = htons( Port );

    // ================ program dla Servera =================================

    if( bind( gniazdo_root,( SOCKADDR * ) &server, sizeof( server ) ) == SOCKET_ERROR )  // przypisanie adresu(gniazdo ma ten adres) , https://man7.org/linux/man-pages/man2/bind.2.html
    {
        cout << "bind() failed." << WSAGetLastError() << endl;
        closesocket( gniazdo_root );
        return 0;
    }

    if( listen( gniazdo_root, 1 ) == SOCKET_ERROR ) // max połączeń oczekujących  https://man7.org/linux/man-pages/man2/listen.2.html
        cout << "Error listening" << endl ;


    while(1){ // postawienie servera, teraz już cały czas działa i czeka na połączenie
        cout << "Oczekiwanie na polaczenie..." << endl;
        gniazdo1 = accept(gniazdo_root, (struct sockaddr *) &client, &dlugosc);
        if (gniazdo1 == -1) {cout << "blad accept\n" << endl; return 0;}
        else cout <<"zaakceptowano\n" << endl;
        while(1){ //obsługa połączenia przychodzącego
            break;
        }
    }


    std::cout << "Hello, World!" << std::endl;

    WSACleanup(); // sprzątamy na koniec całego programu
    return 0;
}


