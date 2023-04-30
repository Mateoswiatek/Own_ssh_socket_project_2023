#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <functional>
#include <sstream>

using namespace std;


// do funkcji, ale ma problem
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

string hashowanie(string tekst){
    hash<string> hasher;
    size_t hash_value = hasher(tekst);
    ostringstream oss;
    oss << hash_value;
    string hash_str = oss.str();
    return hash_str;
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

        // moze funkcje logujaca można jako jedną rzecz zrobić
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
        // hashowanie
        hash_haslo = hashowanie(haslo);
        status = wychodzace(gniazdo1, hash_haslo); // wysyłamy haslo
        if(!status) { cout << "send error"; break; }


        message = przychodzace(gniazdo1); // informacja o logowaniu

        if(message[0] == '0'){
            cout<< " czyli nie ma takiego loginu!" << endl; // << message << endl;
            status = wychodzace(gniazdo1, "1");
            if(!status) { cerr << "send error"; break; }
            continue;
        }
        else if(message[0] == '2'){ // bledne haslo
            cout<< "Bledne haslo!" << endl;
            status = wychodzace(gniazdo1, "1");
            if(!status) { cerr << "send error"; break; }
            continue;
        }
        else if(message[0] == '1'){ // zalogowaliśmy się
            cout<< "Zalogowales sie!" << endl;
            status = wychodzace(gniazdo1, "1");
            if(!status) { cerr << "send error"; break; }
        }

        message = przychodzace(gniazdo1); // czy to root rozdzielenie na normalny i rootowski profil
        cout << message;
        if(message.substr(0, 4) == "root"){
            status = wychodzace(gniazdo1, "1"); // okejka na roota
            if(!status) { cerr << "send error"; break; }

            while(1) {
                message = przychodzace(gniazdo1); // podaj login
                cout << message << endl;

                cin >> mes_to_send; // wpisujemy login
                status = wychodzace(gniazdo1, mes_to_send); // wyslij login
                if(strcmp(mes_to_send.c_str(), "0") == 0){ break; } // wysylamy ze nie chcemy, przechodizmy do normalnych, chcemy odbierac


                message = przychodzace(gniazdo1); // podaj haslo
                cout << message << endl;

                cin >> mes_to_send;
                hash_haslo=hashowanie(mes_to_send);
                status = wychodzace(gniazdo1, hash_haslo); // wyslij haslo

                message = przychodzace(gniazdo1); // podaj czy admin
                cout << message << endl;

                cin >> mes_to_send;
                status = wychodzace(gniazdo1, mes_to_send); // wyslij czy admin
            }
        }
        else{
            status = wychodzace(gniazdo1, "1"); // okejka usera
            if(!status) { cerr << "send error"; break; }
        }


        while(1) {
            message = przychodzace(gniazdo1); // normalne komendy
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
                    message = przychodzace(gniazdo1); //wyswietla nam zawartosc folderu
                    cout << message << "\n\n\n";
                    status = wychodzace(gniazdo1, "1"); // okejka
                    if (!status) { cerr << "send error"; error=1;}
                    break;
                case 2:
                    message = przychodzace(gniazdo1); // podawanie nazwy
                    cout << message << "\n";

                    cin >> mes_to_send;
                    status = wychodzace(gniazdo1, mes_to_send); // wysylanie gdzie przechodzimy
                    if (!status) { cerr << "send error"; error=1;}

                    message = przychodzace(gniazdo1); // wyswietlenie aktualnej sciezki
                    cout << message << "\n";

                    status = wychodzace(gniazdo1, "1"); // okejka
                    if (!status) { cerr << "send error"; error=1;}

                    break;

                case 3:
                    message = przychodzace(gniazdo1); // tworzenie folderu, podajemy nazwe
                    cout << message << "\n";

                    cin >> mes_to_send;
                    status = wychodzace(gniazdo1, mes_to_send); // wysylamy nazwe folderu
                    if (!status) { cerr << "send error"; error=1;}

                    message = przychodzace(gniazdo1); // info zwrotne czy sie udalo
                    cout << message << "\n";

                    status = wychodzace(gniazdo1, "1"); // okejka
                    if (!status) { cerr << "send error"; error=1;}

                    break;

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

        // tu poza komendami normalnymi
        break;
    }

    cout << "blad po stronie servera!" << endl;

    WSACleanup(); // sprzątamy na koniec całego programu
    return 0;
}


