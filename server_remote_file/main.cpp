#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <map> // słowniki

/* może jsona się jeszcze dowali do tego
#include <nlohmann/json.hpp>
using json = nlohmann::json;
 */

using namespace std;


#pragma comment(lib,"ws2_32.lib")
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
    int send_bits; // ilość wysłanych bitów
    send_bits = send(socket, message.c_str(), message.length()+1, 0);
    return send_bits;
}

int main() {
    string IP_serv = "192.168.56.1";
    int Port = 9021;

    SOCKET gniazdo_root, gniazdo1;
    struct sockaddr_in server, client; // instancje struktury które przechowują dane

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

    if( bind( gniazdo_root,( sockaddr * ) &server, sizeof( server ) ) == SOCKET_ERROR )  // przypisanie adresu(gniazdo ma ten adres) , https://man7.org/linux/man-pages/man2/bind.2.html
    {
        cout << "bind() failed. " << WSAGetLastError() << endl;
        closesocket( gniazdo_root );
        return 0;
    }

    if( listen( gniazdo_root, 1 ) == SOCKET_ERROR ) // max połączeń oczekujących  https://man7.org/linux/man-pages/man2/listen.2.html
        cout << "Error listening" << endl ;


/*
 *      Miejsce na różne dane do działania i wgl na zmienne etc
 */
map < string, string >users; // slownik z uzytkownikami
users["admin"] = "password"; // dodajemy uzytkownikow, kiedys mozna zrobic czytanie z pliku
int count_try = 3, hack=0; // ilosc prob logowania później można przerobić, iż każdy użytownik ma oddzielny licznik, też w pliku np.

    while(1){ // postawienie servera, teraz już cały czas działa i czeka na połączenie
        string message, login, mes_to_send, hash_password;
        if (hack) break; // zamiast tego, można zrobić listę np adresów IP które są już wykluczone, i fizycznie na kompie trzeba je usunąć z tej listy
        // choć to i tak kiepsko, bo można sobie zmienić adres IP typu vpn, więc i tak to nie jest idealne zabezpieczenie

        int dlugosc=sizeof(client); // !!!! ważne fest XD

        cout << "Oczekiwanie na polaczenie..." << endl;
        gniazdo1 = accept(gniazdo_root, (struct sockaddr *) &client, &dlugosc); // gniazdo1 teraz odpowiada za połączenie
        if (gniazdo1 == -1) { WSACleanup(); cout << "blad accept\n" << endl; return 0;}

        else cout <<"zaakceptowano\n" << endl;
        while(1){ //obsługa połączenia przychodzącego, nasz socket to gniazdo1
            int status=0;
            while(!status) {
                message = "Witaj podaj login";
                status = wychodzace(gniazdo1, message);
            }

            login = przychodzace(gniazdo1);
            cout << "login to #" << login << "#" << endl;

            status=0;
            while(!status) {
            message = "podaj haslo";
            status = wychodzace(gniazdo1, message);
            }
            hash_password = przychodzace(gniazdo1);
            cout << "hash_password to #" << hash_password << "#" << endl;

            auto para = users.find(login); // od C++11  https://cpp0x.pl/kursy/Kurs-C++/Poziom-5/Kontenery-asocjacyjne-std-set-i-std-map/589

            if ( para == users.end() ) { // 1 - nie ma loginu, 2- bledne haslo, 3 - prezszlo
                status = wychodzace(gniazdo1, "1");
                if(!status) { cout << "send error"; break; }
                message = przychodzace(gniazdo1);
                cout << message[0];
                if( message[0] == '0') break;
                else continue;
            }
            else if(para->second != hash_password) { // jeśli hash haseł jest różny 2
                    // mes_to_send = "2"+to_string(count_try);
                    status = wychodzace(gniazdo1, "2"+to_string(count_try));
                    count_try--; // zmniejszamy ilość prob
                    if(!status) { cout << "send error"; break; }
                    message = przychodzace(gniazdo1);
                    cout << message[0];
                    if(count_try == 0){
                        status = wychodzace(gniazdo1, "nie mozesz juz probowac. skontaktuj sie z adminem");
                        if(!status) { cout << "send error"; break; }
                        hack = 1;
                        break;
                    }
                    if( message[0] == '0') break;
                    else continue;
            } else
            { // tu się już zalogowaliśmy poprawnie wysyłamy 3
                while(1){
                    string dostepne_funkcje = """Podaj numer funkcji:\n"
                                            "1. Mozesz kupic bulki\n"
                                            "2. Wyjsc na spacer\n"
                                            "3. Spoktac sie z dziewczyna\n"
                                            "   sorki, no tak, informatycy nie posiadaja kobiet\n"
                                            "4. To napisz algorytm na szukanie kobiety, moze pomoze\n""";
                    mes_to_send = "3" + dostepne_funkcje;
                    status = wychodzace(gniazdo1, mes_to_send);
                    cout << "wyslalismy cos" << mes_to_send;

                    if(!status) { cout << "send error"; break; }

                    cout << "normalny program" << endl;
                    przychodzace(gniazdo1);
                    break;
                }
            }

            /*
             *  Po logowaniu
             */
            break;
        }
        closesocket(gniazdo1);
    }


    std::cout << "Hello, World!" << std::endl;

    WSACleanup(); // sprzątamy na koniec całego programu
    return 0;
}


