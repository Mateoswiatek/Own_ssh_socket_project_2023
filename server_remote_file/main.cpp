#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <map> // słowniki
#include <vector>
#include <set>
#include <filesystem>

using namespace std;

#pragma comment(lib,"ws2_32.lib")

class User{
public:
    User() : _login("0"){}
    User(string login, string hash_pass, int acc_type = 0) : _login(login), _hashpassword(hash_pass), _is_logged(0), _count_login_try(0), _block(0), _account_type(acc_type){}
    int try_login(string login, string password){ // 1 - zalogowano; 2 - bledne haslo; 0 - nie ma loginu
        if(login==_login){
            if(strcmp(_hashpassword.c_str(), password.c_str()) == 0 ){
                _is_logged = 1;
                return 1; // zalogowano
            }
            _count_login_try++;
            return 2; // bledne haslo
        }
        return 0; // nie ma loginu
    }
    string getlogin(){return _login;}
    bool get_is_logged(){return _is_logged;}

private:
    string _login;
    string _hashpassword;
    int _count_login_try;
    int _account_type; // 1 admin, 0 user
    bool _is_logged;
    bool _block; // czy jest zablokowany
};

string przychodzace(SOCKET socket){ // tu dorobić sprawdzanie czy to co przychodzi jest stringiem
    int recv_bits;
    char recv_frame[1024]; // musimy gdzieś zapisywać to co przychodzi
    recv_bits = recv( socket, recv_frame, 1024, 0 );
    if( recv_bits < 1 ) {
        strcpy(recv_frame,"-1" );
    } // jeśli nie otrzymaliśmy odpowiedzi
    return recv_frame;
}

int wychodzace(SOCKET socket, string message){
    int send_bits; // ilość wysłanych bitów
    send_bits = send(socket, message.c_str(), message.length()+1, 0);
    return send_bits;
}

vector<User> users;

int add_user(string login, string hash_pass, int acc_type = 0){
    User new_user(login, hash_pass, acc_type);
    users.push_back(new_user);
}

string wypisz_zawartosc(){
    string message = "";
    filesystem::path folder_path = "./";
    for (const auto& entry : filesystem::directory_iterator(folder_path))
    {
        message.append(entry.path().filename().string());
        message.append("\n");
    }
    return message;
}











int main() {
    // dodawanie admina
    User admin("admin", "admin", 1);
    users.push_back(admin);

    // dorobić czy konfiguracja czy normalna praca.
    // konfugiruacja jako oddzielna funkcja, aby admin tez mogl wejsc / ryzykowne


    // to wszystko do funkcji walnąć
    string IP_serv = "192.168.56.1";
    int Port = 9021;

    SOCKET gniazdo_root, client_socket;
    struct sockaddr_in server, client; // instancje struktury które przechowują dane

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
        cerr << "Initialization error.\n"; // jeśli się nie udało to dostajemy komunikat
        return 1; // wystąpił błąd
    }

    gniazdo_root = socket( AF_INET, SOCK_STREAM, 0 ); // IPv4, IPPROTO_TCP
    if( gniazdo_root == SOCKET_ERROR ){ // jeśli jest jakiś błąd
        cerr << "Initialization error.\n Error creating socket: %d\n" << WSAGetLastError() << endl; // wypisze nam ostatni blad
        WSACleanup(); // "sprzątanie" po WSA
        return 1;
    }

    // podpięcie gniazda pod port
    memset( & server, 0, sizeof( server ) ); // pamięć na 0, są domyślne wartości w polach struktury
    server.sin_family = AF_INET; // używamy protokołu IPv4
    server.sin_addr.s_addr = inet_addr( "192.168.56.1" );
    server.sin_port = htons( Port );

    // ================ program dla Servera =================================

    if(bind( gniazdo_root,( sockaddr* ) &server, sizeof( server ) ) == SOCKET_ERROR )  // przypisanie adresu(gniazdo ma ten adres) , https://man7.org/linux/man-pages/man2/bind.2.html
    {
        cerr << "bind() failed." << WSAGetLastError() << endl;
        closesocket( gniazdo_root );
        return 1;
    }

    if( listen( gniazdo_root, 1 ) == SOCKET_ERROR ) { // max połączeń oczekujących  https://man7.org/linux/man-pages/man2/listen.2.html
        cerr << "Error listening" << endl;
        return 1;
    }

// wszystko powyzej można do funkcji ?

// alternatywnie, można zrobić pole dozwolonych adresow ip i tylko za ich pomocą łączyć, ew jeśli jest nowy adres ip a dane sie zgadzaja, to trzeba potwierdzic mailem
// ewentualnie całe podsiecie banowac i trzymac tylko poczatkowe wartosci
    set<string> ban_IP;
    ban_IP.insert("192.168.56.2"); // zmienic na 1


    while(1){ // postawienie servera, teraz już cały czas działa i czeka na połączenie
        int dlugosc=sizeof(client); // !!!! ważne fest XD

        cout << "Oczekiwanie na polaczenie..." << endl;
        client_socket = accept(gniazdo_root, (struct sockaddr *) &client, &dlugosc); // client_socket teraz odpowiada za połączenie
        if (client_socket == INVALID_SOCKET) {closesocket(gniazdo_root); cerr << "blad accept\n" << endl; return 1;}

        // sprawdzanie adresu IP przychodzącego, można byłoby jakoś przed nawiązaniem połączenia to zrobić

        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client.sin_addr), clientIp, INET_ADDRSTRLEN);
        cout << "adres IP: " << clientIp << endl;

        if (ban_IP.find(clientIp) != ban_IP.end()) { // jeśli ip jest na zbanowanym
            wychodzace(client_socket, "jestes zbanowany, ez");
            closesocket(client_socket);
            continue;
        }


        while(1){ //obsługa połączenia przychodzącego, nasz socket to client_socket
            string message, login, hash_password;
            int status=0;

            // moze to tez moznaby było dać do funkcji?
            message = "Witaj podaj login";
            status = wychodzace(client_socket, message);
            if(!status){break;}

            login = przychodzace(client_socket);
            if( login == "-1"){cerr << "error przy odbieraniu danych"; break;}
            cout << "login to #" << login << "#" << endl;

            message = "podaj haslo";
            status = wychodzace(client_socket, message);
            if(!status){break;}

            hash_password = przychodzace(client_socket);
            if( hash_password == "-1"){cerr << "error przy odbieraniu danych"; break;}
            cout << "hash_password to #" << hash_password << "#" << endl;

            User zalogowany_user;
            int status_loginu=0;
            for (auto& user : users) {
                status_loginu=0;
                if(user.getlogin() == login) { // bo lecimy po kazdym elemencie
                    status_loginu = user.try_login(login, hash_password); // probujemy sie zalogowac na tego usera
                    status = wychodzace(client_socket, to_string(status_loginu)); // wysylamy stan
                    if (!status) { status_loginu = -2; break; }

                    message = przychodzace(client_socket); // wysylamy 1
                    if (message[0] != '1') { status_loginu = -2; break; }

                    if (status_loginu == 1) {
                        zalogowany_user = user;
                        break;
                    }
                }
                else{
                    status_loginu = 0;
                    status = wychodzace(client_socket, "0"); // wysylamy stan
                    if (!status) {
                        status_loginu = -2;
                        break;
                    }
                    message = przychodzace(client_socket); // wysylamy 1
                    if (message[0] != '1') {
                        status_loginu = -2;
                        break;
                    }
                } // jesli nie znaleziono takiego usera
            }
            if(status_loginu == -2){
                cerr << "error logowania";
                break;
            }
            else if(status_loginu != 1){ continue;}

            // zrobic rozdzielenie na rozne typy - admin / user

            while(1){ // gdy zalogowano:
                string dostepne_funkcje = """Podaj numer funkcji:\n"
                                        "1. wyswietl zawartosc foldera\n"
                                        "2. przejdz do folderu\n"
                                        "3. pobierz plik\n"
                                        "4. wyslij plik\n"
                                        "5. To napisz algorytm na szukanie kobiety, moze pomoze\n""";
                status = wychodzace(client_socket, dostepne_funkcje);
                if(!status) { cout << "send error"; break; }

                int wybor =stoi(przychodzace(client_socket));
                if( wybor== -1){cerr << "error przy odbieraniu danych"; break;}

                cout << wybor;

                bool error=0;
                switch (wybor) {
                    case 1:
                        message = wypisz_zawartosc();
                        cout << "\n\n\n\n" << message << endl;
                        status = wychodzace(client_socket, message);
                        if(!status) { cout << "send error"; error = 1; break; }

                        przychodzace(client_socket);
                        if( login == "-1"){cerr << "error przy odbieraniu danych"; break;}

                        break;
                    case 2:
                        break;

                }
                if(error){ break;}


            }
            break;
        }
        closesocket(client_socket);
    }


    std::cout << "Hello, World!" << std::endl;

    WSACleanup(); // sprzątamy na koniec całego programu
    return 0;
}


