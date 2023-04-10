#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;


// #pragma comment(lib,"ws2_32.lib")
// definiujemy sobie parametry
// #define IP_SERV "192.168.56.1"
// #define Port_SERV 9000

string przychodzace(SOCKET socket){
    int recv_bits;
    char recv_frame[1024]; // musimy gdzieś zapisywać to co przychodzi
    recv_bits = recv( socket, recv_frame, 1024, 0 );
    if( recv_bits == 0 || recv_bits == WSAECONNRESET ) { strcpy(recv_frame,"Connection closed / Error" ) ; } // jeśli nie otrzymaliśmy odpowiedzi
    return recv_frame;
}

int wychodzace(SOCKET socket, string message){
    cout << "wiadomosc to: #" << message << "#" << endl;
    int send_bits; // ilość wysłanych bitów
    send_bits = send(socket, message.c_str(), message.length()+1, 0);
    return send_bits;
}

int main() {

    string IP_serv = "192.168.56.1";
    int Port = 9021;

    SOCKET gniazdo1;
    struct sockaddr_in ser;

    WSADATA wsaData;
    if (WSAStartup( MAKEWORD( 2, 0 ), &wsaData )){printf("blad WSDATA\n"); return 0;}

    gniazdo1 = socket( AF_INET, SOCK_STREAM, 0 );
    if( gniazdo1 == SOCKET_ERROR ){ // jeśli jest jakiś błąd
        cout << "Initialization error.\n Error creating socket: %d\n" << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }

    memset( & ser, 0, sizeof( ser ) ); // pamięć na 0, są domyślne wartości w polach struktury
    ser.sin_family = AF_INET; // używamy protokołu IPv4
    ser.sin_addr.s_addr = inet_addr( "192.168.56.1" );
    ser.sin_port = htons( Port );

    // ============== Część Klienta =======================

    // łączenie się z serverem
    if ( connect(gniazdo1, (struct sockaddr *) &ser, sizeof(ser)) == SOCKET_ERROR) { cout << "error connect" << endl; WSACleanup(); return 0; }

    while(1){
        int status;
        string haslo, login, message, hash_haslo;

        // można jako jedną rzecz zrobić
        message = przychodzace(gniazdo1);
        cout << "#" << message << "#";
        if (message == ""){cout << "error po stornie servera"; break;}
        cin >> login;
        status = wychodzace(gniazdo1, login); // wysyłamy login
        if(!status) { cout << "send error"; break; }
        message = przychodzace(gniazdo1);
        cout << message;
        if (message == ""){cout << "error po stornie servera"; break;}
        cin >> haslo;

        // tutaj dorobić hashowanie hasła

        status = wychodzace(gniazdo1, haslo); // wysyłamy haslo
        if(!status) { cout << "send error"; break; }


        message = przychodzace(gniazdo1); // server wysyła OK - zalogowano, ilość pozostałych prób.
        // gdy będzie równe 0, np blokuje adres ip i dodaje adres do listy (wiadomo, że można to obejść zmieniając
        // adres ip, ale to tylko projekt przykładowy) tu aż się roi od dziur w bezpieczeństwie
        if (message != "OK"){
            cout << "Bledne login lub haslo!\nPozostalo prob:" << message <<endl;
            // ew tu dopisać wysłanie jakiejś wiadomości, aby nie było błędu żadego
            continue;
        }
        /*
         *  część po zalogowaniu jakieś menu, opcje do wyboru, każdy wybór wysyła inną waidomość i inaczej działa z tym co przyjdzie.
         *  np może być wysłanie pliku, to w tedy wybieramy plik i dalej wysyła się jego zawartość... etc
         *  możemy wyświetlić pliki, to w tedy leci odpowiednia komenda, i zwraca listę elementów, np dwójkami, nazwa,rozmiar
         *  a tu na kliencie będzie to przedstawiane w postaci listy / później w postaci kafelek, albo wgl, obiektów wektor obiektów,
         *  gdzie każdy będzie miał tą swoją nazwę i rozmiar. i później te kafelki się będą wyświetlać.
         */


        break;
    }
    

    std::cout << "Hello, World!" << std::endl;

    WSACleanup(); // sprzątamy na koniec całego programu
    return 0;
}


