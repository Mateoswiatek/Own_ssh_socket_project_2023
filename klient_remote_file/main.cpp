#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>

using namespace std;

// #pragma comment(lib,"ws2_32.lib")
// definiujemy sobie parametry
// #define IP_SERV "192.168.56.1"
// #define Port_SERV 9000
// funkcja
/*
SOCKET open_connect(const string ip, const int port, struct sockaddr_in* ser){
    SOCKET _gniazdo;
    WSADATA _wsaData;
    if (WSAStartup( MAKEWORD( 2, 0 ), &_wsaData )){printf("blad WSDATA\n"); return 0;}

    _gniazdo = socket( AF_INET, SOCK_STREAM, 0 );
    if( _gniazdo == SOCKET_ERROR ){ // jeśli jest jakiś błąd
        cout << "Initialization error.\n Error creating socket: %d\n" << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }

    memset( & ser, 0, sizeof( ser ) ); // pamięć na 0, są domyślne wartości w polach struktury
    ser->sin_family = AF_INET; // używamy protokołu IPv4
    ser->sin_addr.s_addr = inet_addr( ip.c_str() ); // bo inet_addr potrzebuje typu "const char*"
    ser->sin_port = htons( port );
    return _gniazdo;
}
*/

string przychodzace(SOCKET socket){
    int recv_bits;
    char recv_frame[1024]; // musimy gdzieś zapisywać to co przychodzi
    recv_bits = recv( socket, recv_frame, 1024, 0 );
    if( recv_bits < 1 ) { strcpy(recv_frame,"Connection closed / Error" ) ; } // jeśli nie otrzymaliśmy odpowiedzi
    return recv_frame;
}

int wychodzace(SOCKET socket, string message){
    int send_bits; // ilość wysłanych bitów
    send_bits = send(socket, message.c_str(), message.length()+1, 0);
    return send_bits;
}

int main() {

    const string IP_serv = "192.168.56.1";
    const int Port = 9021;
    /*
    SOCKET gniazdo1;
    struct sockaddr_in ser;
    gniazdo1 = open_connect(IP_serv, Port, &ser);
     */

    SOCKET gniazdo1;
    struct sockaddr_in ser; // instancje struktury które przechowują dane

    WSADATA wsaData;
    int result = WSAStartup( MAKEWORD(2,2), &wsaData); // używamy wersji 2.2
    if(result != NO_ERROR) cout << "Initialization error.\n"; // jeśli się nie udało to dostajemy komunikat

    gniazdo1 = socket( AF_INET, SOCK_STREAM, 0 ); // IPv4, IPPROTO_TCP
    if( gniazdo1 == SOCKET_ERROR ){ // jeśli jest jakiś błąd
        cout << "Initialization error.\n Error creating socket: %d\n" << WSAGetLastError() << endl; // wypisze nam ostatni blad
        WSACleanup(); // "sprzątanie" po WSA
        return 0;
    }

    memset( &ser , 0, sizeof( ser ) ); // pamięć na 0, są domyślne wartości w polach struktury
    ser.sin_family = AF_INET; // używamy protokołu IPv4
    ser.sin_addr.s_addr = inet_addr( "192.168.56.1" );
    ser.sin_port = htons( Port );


    // ============== Część Klienta =======================

    // łączenie się z serverem
    if ( connect(gniazdo1, (struct sockaddr *) &ser, sizeof(ser)) == SOCKET_ERROR) { cout << "error connect" << endl; WSACleanup(); return 0; }

    while(1){
        int status;
        string haslo, message, mes_to_send, hash_haslo;

        // można jako jedną rzecz zrobić
        message = przychodzace(gniazdo1);
        if (message == ""){cout << "error po stornie servera"; break;}
        cout<< message<< endl;
        cin >> mes_to_send;
        status = wychodzace(gniazdo1, mes_to_send); // wysyłamy login
        if(!status) { cout << "send error"; break; }
        message = przychodzace(gniazdo1);
        cout << message << endl;
        if (message == ""){cout << "error po stornie servera"; break;}
        cin >> haslo;

        // tutaj dorobić hashowanie hasła

        status = wychodzace(gniazdo1, haslo); // wysyłamy haslo
        if(!status) { cout << "send error"; break; }


        message = przychodzace(gniazdo1); // informacja o logowaniu

        if(message[0] == '0'){
            cout<< " czyli nie ma takiego loginu!"; // << message << endl;
            status = wychodzace(gniazdo1, "1");
            if(!status) { cerr << "send error"; break; }
            continue;
        }
        else if(message[0] == '2'){ // bledne haslo
            cout<< "Bledne haslo!";
            status = wychodzace(gniazdo1, "1");
            if(!status) { cerr << "send error"; break; }
            continue;
        }
        else if(message[0] == '1'){ // zalogowaliśmy się, wiadomosc o tym juz dostalismy
            cout<< "Zalogowales sie!" << endl;
            status = wychodzace(gniazdo1, "1");
            if(!status) { cerr << "send error"; break; }
        }


        /*
         *  część po zalogowaniu jakieś menu, opcje do wyboru, każdy wybór wysyła inną waidomość i inaczej działa z tym co przyjdzie.
         *  np może być wysłanie pliku, to w tedy wybieramy plik i dalej wysyła się jego zawartość... etc
         *  możemy wyświetlić pliki, to w tedy leci odpowiednia komenda, i zwraca listę elementów, np dwójkami, nazwa,rozmiar
         *  a tu na kliencie będzie to przedstawiane w postaci listy / później w postaci kafelek, albo wgl, obiektów wektor obiektów,
         *  gdzie każdy będzie miał tą swoją nazwę i rozmiar. i później te kafelki się będą wyświetlać.
         */
        while(1) {
            message = przychodzace(gniazdo1);
            cout << message;
            cin >> mes_to_send;
            int wybor =stoi(mes_to_send);
            status = wychodzace(gniazdo1, mes_to_send);
            if (!status) {
                cerr << "send error";
                break;
            }

            bool error=0;
            switch (wybor) {
                case 1:
                    message = przychodzace(gniazdo1);
                    cout << message << "\n\n\n";
                    status = wychodzace(gniazdo1, "1");
                    if (!status) { cerr << "send error"; error=1;}
                    break;
                case 2:
                    message = przychodzace(gniazdo1);
                    cout << message << "\n";

                    cin >> mes_to_send;
                    status = wychodzace(gniazdo1, mes_to_send);
                    if (!status) { cerr << "send error"; error=1;}

                    message = przychodzace(gniazdo1);
                    cout << message << "\n";

                    status = wychodzace(gniazdo1, "1");
                    if (!status) { cerr << "send error"; error=1;}

                    break;
                case 3:
                    message = przychodzace(gniazdo1); // ze musimy folder podac
                    cout << message << "\n";

                    cin >> mes_to_send;
                    status = wychodzace(gniazdo1, mes_to_send); // wysylamy nazwe folderu
                    if (!status) { cerr << "send error"; error=1;}

                    message = przychodzace(gniazdo1); // info zwrotne czy sie udalo
                    cout << message << "\n";

                    status = wychodzace(gniazdo1, "1");
                    if (!status) { cerr << "send error"; error=1;}
                case 4:

                    message = przychodzace(gniazdo1); // ze musimy nazwe pliku podac
                    cout << message << "\n";

                    cin >> mes_to_send;
                    status = wychodzace(gniazdo1, mes_to_send); // wysylamy nazwe pliku
                    if (!status) { cerr << "send error"; error=1;}
                    ofstream file(mes_to_send);

                    int x =0;
                    while(1){ // dopoki sa jeszcze dane
                        x++;
                        message = przychodzace(gniazdo1); // ze musimy nazwe pliku podac

                        if(strcmp(message.c_str(), "END") == 0){
                            break;
                        }

                        file.write(message.c_str(), message.size());

                        status = wychodzace(gniazdo1, "1"); // wysylamy nazwe pliku
                        if (!status) { cerr << "send error"; error=1;}

                    }
                    file.close();
                    cout << "wyszlismy" << endl;
                    break;

            }
            if(error){cout<<"blad wysylania danych";}


        }


        break;
    }
    

    cout << "blad po stronie servera!" << endl;

    WSACleanup(); // sprzątamy na koniec całego programu
    return 0;
}


