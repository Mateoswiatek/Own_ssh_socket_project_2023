#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <map> // słowniki
#include <vector>
#include <set>
#include <filesystem>
#include <fstream>


using namespace std;

#pragma comment(lib,"ws2_32.lib")

class User{
public:
    User() : _login("0"){}
    User(string login, string hash_pass, int acc_type = 0) : _login(login), _hashpassword(hash_pass), _is_logged(0), _count_login_try(0), _block(0), _account_type(acc_type){}
    int try_login(string login, string password){ // 1 - zalogowano; 2 - bledne haslo; 0 - nie ma loginu
        if(login==_login){
            if(_count_login_try == 4) { _block=1; return -1;} // jesli 5 razy ktos probował to blokujemy
            if(strcmp(_hashpassword.c_str(), password.c_str()) == 0 ){
                _is_logged = 1;
                _count_login_try=0; // zerujemy proby
                return 1; // zalogowano
            }
            _count_login_try++;
            return 2; // bledne haslo
        }
        return 0; // nie ma loginu
    }
    string getlogin(){return _login;}
    string getpassword(){return _hashpassword;}
    bool get_is_logged(){return _is_logged;} // dlaszy rozwoj, np dla admina kto zalogowany jest
    bool is_block(){return _block;}
    bool get_account_type(){ return _account_type;}

private:
    string _login;
    string _hashpassword;
    int _count_login_try;
    int _account_type; // 1 admin, 0 user
    bool _is_logged;
    bool _block; // czy jest zablokowany
    // mozna dodac pole z czasem ostatniego logowania, jesli aktualna jest mniejsza niz 2 min to nie dac wpisac, jesli prob zrobil 3 no to na 1h i tak dalej, jak w telefonach
};

string przychodzace(SOCKET socket){ // tu mozna by dorobić sprawdzanie czy to co przychodzi jest stringiem, sql injection
    int recv_bits;
    char recv_frame[1024]; // bufor
    recv_bits = recv( socket, recv_frame, 1024, 0 );
    if( recv_bits < 1 ) {
        strcpy(recv_frame,"-1" );
    } // jeśli nie otrzymaliśmy odpowiedzi dajemy "-1"
    return recv_frame;
}

int wychodzace(SOCKET socket, string message){
    int send_bits; // ilość wysłanych bitów
    send_bits = send(socket, message.c_str(), message.length()+1, 0);
    return send_bits;
}

vector<User> users; // vektor z userami

int add_user(string login, string hash_pass, int acc_type = 0){ // dodawanie userow
    User new_user(login, hash_pass, acc_type);
    users.push_back(new_user);
}

string wypisz_zawartosc(){ // wyswietlamy zawartosc folderu
    string message = "";
    //filesystem::path folder_path = "./";
    for (const auto& entry : filesystem::directory_iterator("./"))
    {
        message.append(entry.path().filename().string());
        message.append("\n");
    }
    return message;
}

int main() {
    // dodawanie admina
    User admin("admin", "8141937955048696534", 1); // haslo to "admin"
    users.push_back(admin);
    // zwyklego usera
    User zwykly("abc", "3663726644998027833"); // haslo to abc
    users.push_back(zwykly);


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

// alternatywnie, można zrobić pole dozwolonych adresow ip i tylko za ich pomocą łączyć, ew jeśli jest nowy adres ip a dane sie zgadzaja, to trzeba potwierdzic mailem
// ewentualnie całe podsiecie banowac i trzymac tylko poczatkowe wartosci
    set<string> ban_IP;
    ban_IP.insert("192.168.56.2"); // zmienic na 1 w tedy nie bede mogl sie zalogowac


    while(1){ // postawienie servera, teraz już cały czas działa i czeka na połączenie
        int dlugosc=sizeof(client); // !!!! ważne fest XD

        cout << "Oczekiwanie na polaczenie..." << endl;
        client_socket = accept(gniazdo_root, (struct sockaddr *) &client, &dlugosc); // client_socket teraz odpowiada za połączenie
        if (client_socket == INVALID_SOCKET) {closesocket(gniazdo_root); cerr << "blad accept\n" << endl; return 1;}

        // sprawdzanie adresu IP przychodzącego, można byłoby jakoś przed nawiązaniem połączenia to zrobić inny interfejs komunikacji

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

            login = przychodzace(client_socket); // odbieramy login
            if( login == "-1"){cerr << "error przy odbieraniu danych"; break;}

            message = "Podaj haslo";
            status = wychodzace(client_socket, message);
            if(!status){break;}

            hash_password = przychodzace(client_socket);
            if( hash_password == "-1"){cerr << "error przy odbieraniu danych"; break;}

            User zalogowany_user;
            int status_loginu=0;
            for (auto& user : users) {
                //if(user.is_block()){ ban_IP.insert(clientIp); }
                if(strcmp(user.getlogin().c_str(), login.c_str()) == 0 and user.is_block() == 0) { // bo lecimy po kazdym elemencie tylko nie zablokowanych
                    status_loginu = user.try_login(login, hash_password); // probujemy sie zalogowac na tego usera
                    if (status_loginu == 1){
                        zalogowany_user = user; // zapisujemy aktualnie zalogowanego
                        break;
                    }
                    else{ // status loginu jest rozny od 1,czyli albo -1 albo 2 albo 0 // bledne haslo + ban, bledne haslo, nie ma loginu
                        if (status_loginu == -1) { ban_IP.insert(clientIp); } // dodajemy aktualny adres IP do listy zablokowanych numerów IP, przy kolejnym połączeniu wywali go
                        break;
                    }
                }
                else{ // status loginu po wszystkich userach = 0,nie ma loginu
                    status_loginu = 0; // nie ma takiego loginu
                }
            }

            status = wychodzace(client_socket, to_string(status_loginu)); // wysylamy stan
            if (!status) { break; }

            message = przychodzace(client_socket); // dostajemy 1
            if (message[0] != '1') { cerr << "error przy odbieraniu danych"; break; }

            if(status_loginu != 1){ // jesli sie nie udalo zalogowac
                continue;
            }


            string nowy_login, hash_haslo;  // funkcjonalnosci admin / user
            int czy_admin;
            if(zalogowany_user.get_account_type() == 1){ // czy jest rootem
                status = wychodzace(client_socket, "root");
                przychodzace(client_socket); // okejka roota

                while(1) {
                    status = wychodzace(client_socket, "podaj nowego usera, nie? to wpisz 0");
                    nowy_login = przychodzace(client_socket);
                    cout << "nowy login to:" << nowy_login << endl;
                    if(strcmp(nowy_login.c_str(), "0") == 0) { cout<< "wyszlismy"; break;} // dostalismy ze nie chce, przechodzimy do normalnych, wysylamy

                    status = wychodzace(client_socket, "podaj haslo");
                    cout << "status jest rowny: " << status << endl;
                    hash_password = przychodzace(client_socket);

                    status = wychodzace(client_socket, "czy ma byc adminem?");
                    czy_admin = stoi(przychodzace(client_socket));

                    User nowy(nowy_login, hash_password, czy_admin );
                    users.push_back(nowy);
                }
                for (auto& user : users) {
                    cout << "nazwa = " << user.getlogin() << endl << "haslo =" << user.getpassword() << endl << "typ =" << user.get_account_type() << "\n\n";
                    // dla nas, bardziej do testow
                }

            }else{
                status = wychodzace(client_socket, "user");
                przychodzace(client_socket); // okejka usera
            }

            while (1) { // gdy zalogowano:
                string dostepne_funkcje = """Podaj numer funkcji:\n"
                                          "1. wyswietl zawartosc folderu\n"
                                          "2. przejdz do folderu .. (nadrzedny)\n"
                                          "3. stworz folder\n"
                                          "4. pobierz plik\n"
                                          "5. wyslij plik\n""";
                status = wychodzace(client_socket, dostepne_funkcje);
                if (!status) {
                    cout << "send error";
                    break;
                }

                int wybor = stoi(przychodzace(client_socket)); // tu przychodzi wybor usera
                if (wybor == -1) {
                    cerr << "error przy odbieraniu danych";
                    break;
                }

                cout << wybor;

                string folder, nazwa_pliku, potwierdzenie;

                bool error = 0;
                switch (wybor) {
                    case 1:

                        message = wypisz_zawartosc();
                        cout << "\n\n\n\n" << message << endl;
                        status = wychodzace(client_socket, message);
                        if (!status) {
                            cout << "send error";
                            error = 1;
                            break;
                        }

                        message = przychodzace(client_socket);
                        if (message == "-1") {
                            cerr << "error przy odbieraniu danych";
                            break;
                        }
                        break;
                    case 2:
                        message = "podaj nazwe folderu";
                        status = wychodzace(client_socket, message);
                        if (!status) {
                            cout << "send error";
                            error = 1;
                            break;
                        }

                        folder = przychodzace(client_socket);
                        if (folder == "-1") {
                            cerr << "error przy odbieraniu danych";
                            error = 1;
                            break;
                        }

                        filesystem::current_path("./" + folder);

                        message = "jestes w: " + filesystem::current_path().string(); // wysyłamy ścieżkę
                        status = wychodzace(client_socket, message);
                        if (!status) {
                            cerr << "send error";
                            error = 1;
                            break;
                        }

                        message = przychodzace(client_socket);
                        if (message == "-1") {
                            cerr << "error przy odbieraniu danych";
                            error = 1;
                            break;
                        }
                        break;

                    case 3:
                        message = "podaj nazwe folderu ktory chcesz utworzyc";
                        status = wychodzace(client_socket, message);
                        if (!status) {
                            cerr << "send error";
                            error = 1;
                            break;
                        }

                        folder = przychodzace(client_socket);
                        if (folder == "-1") {
                            cerr << "error przy odbieraniu danych";
                            error = 1;
                            break;
                        }

                        filesystem::create_directory(folder);

                        message = "utworzyles nowy folder w " + wypisz_zawartosc();
                        status = wychodzace(client_socket, message);
                        if (!status) {
                            cerr << "send error";
                            error = 1;
                            break;
                        }

                        message = przychodzace(client_socket);
                        if (message == "-1") {
                            cerr << "error przy odbieraniu danych";
                            error = 1;
                            break;
                        }
                        break;

                    case 4:{
                        message = "podaj nazwe pliku ktory chcesz pobrac";
                        status = wychodzace(client_socket, message);
                        if (!status) {
                            cout << "send error";
                            error = 1;
                            break;
                        }

                        nazwa_pliku = przychodzace(client_socket);
                        if (nazwa_pliku == "-1") {
                            cerr << "error przy odbieraniu danych";
                            break;
                        }


                        char bufor[1024];
                        ifstream plik(nazwa_pliku);
                        while (plik) {
                            plik.read(bufor, 1023);
                            streamsize bytes_read = plik.gcount(); // liczba odczytanych bajtow
                            bufor[bytes_read] = '\0'; // dopisuje na 1024 pozycji koniec wiadomości
                            if (bytes_read > 0) {
                                status = wychodzace(client_socket, bufor);
                                if (!status) {
                                    cout << "send error";
                                    error = 1;
                                    break;
                                }
                            }
                            potwierdzenie = przychodzace(client_socket);
                            if (potwierdzenie == "-1") {
                                cerr << "error przy odbieraniu danych";
                                break;
                            }
                        }
                        status = wychodzace(client_socket, "END"); // gdy sie skonczyl plik
                        potwierdzenie = przychodzace(client_socket);
                        if (potwierdzenie == "-1") {
                            cerr << "error przy odbieraniu danych";
                            break;
                        }
                        plik.close();
                        break;}
                    case 5:{

                        status = wychodzace(client_socket, "Podaj nazwe pliku pod jaka ma byc zapisany wybrany plik");
                        if (!status) { cout << "send error"; error = 1; break; }

                        string nazwa_pliku;
                        nazwa_pliku = przychodzace(client_socket);
                        if (nazwa_pliku == "-1") { cerr << "error przy odbieraniu danych"; break; }

                        status = wychodzace(client_socket, "1"); // okejka
                        if (!status) { cout << "send error"; error = 1; break; }

                        ofstream file(nazwa_pliku);

                        while(1){ // dopoki sa jeszcze dane
                            message = przychodzace(client_socket); // ze musimy nazwe pliku podac

                            if(strcmp(message.c_str(), "END") == 0){
                                break; // wychodzimy, po wyjsciu wysyłamy ponownie zestaw poleceń jakie może user wykonać
                            }
                            file.write(message.c_str(), message.size());

                            status = wychodzace(client_socket, "1");
                            if (!status) { cerr << "send error"; error=1;}
                        }
                        file.close();
                        cout << "wyszlismy ez\n";
                    }
                }
                // tu mozna by wyciągnąćniektóre rzeczy wspólne, ale wolę nie, abym nie musiał grzebać jakbym coś dodawał "nietypowego"
                if (error) { break; } // jeśli jest błąd to wychodzimy z zalogowania

            }

            break; // zrywamy połączenie aby server mógł nadal czekac na polaczenie od innych
        }
        closesocket(client_socket);
    }

    std::cout << "Hello, World!" << std::endl; // nigdy, bo server ciagle bedzie stal, ewentualnie tu jakies prace na servezre

    WSACleanup(); // sprzątamy na koniec całego programu
    return 0;
}


