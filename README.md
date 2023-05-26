# Project for the subject fundamentals of programming (Cpp)
Computer Science and Intelligent Systems 1 year 2 semester. 

## Description
Software providing 'not very secure' remote access to a computer. The project includes files for the client and the server.
The server is in standby mode, 1 person at a time can have access. Logging in is done by entering the appropriate Login Password pair. It will be possible to perform several operations on the remote computer. The programme is written for a friendly user. It does not have many security features. 
Jpg files show how the programme works

## Functionality / options
- [ ] Direct server login, password hashed
Separate feedback messages for:
    - non-existent login
    - wrong password
    - correct login
- [ ] Limitation of password attempts to a preset number. If exceeded, no login is possible. Server shuts down, a restart of the program / server is necessary to return.
- [ ] Display of "menu"

    - display the contents of a directory/folder
    - move to folder
    - create folder
    - download file
    - send file

## Future expansion/improvement
If there is enough time, desire and life then: 
- [ ] Clearing the code of unnecessary test lines and overly extensive comments.
- [x] Replacing information handling with switch/cashes
- [x] Adding a Password Hashing Function (at least a bit more security).

- [x] Adding functions to the menu, for example:
    - copy the specified file,
    - uploading the specified file,
    - displaying files in a 'shared' directory
    - Adding security against deletion of existing files ( cybersecurity )
    - (file deletion)
- [ ] Addition of a simple graphical interface on the client side
- [ ] Modification of the way logins and passwords are retrieved - reading from a file this entails:
- [ ] Adding users / restricting certain options depending on the user. 
- [ ] Adding a log file of what is happening on the server + an independent program which periodically makes a copy of this file to a completely different location (in case of an attack and removal of traces from this file).
- [ ] Adding a field with the time since the last login attempt. allow further logins depending on the time since the last attempt and the number of failed logins. e.g. 2 invalid logins - login after 5 minutes, 3 logins 15 minutes, 5 logins 2 hours
- [ ] Replacing the hash with asymmetric keys
- [ ] Adding "some" security
- [ ] Adding to the field
- [ ] Transferring ideas from the code here to the README

# PL:
## Opis
Oprogramowanie zapewniające "niezbyt bezpieczny" zdalny dostęp do komputera. Projekt zawiera pliki dla klienta oraz servera.
Server jest w stanie czuwania, 1 osoba jednocześnie może mieć dostęp. Logowanie odbywa się poprzez podanie odpowiedniej pary Login Hasło. Możliwe będzie wykonanie kilku operacji na zdalnym komputerze. Program jest pisany pod przyjaznego użytkownika. Nie posiada wielu zabezpieczeń. 

## Funkcjonalności / opcje

- [ ] Logowanie bezpośrednie do servera, hasło zahashowane
Oddzielne komunikaty zwrotne dla:
    - nieistniejącego loginu
    - błędnego hasła
    - prawidłowego zalogowania
- [ ] Ograniczenie prób podania hasła do zadanej ilości. Po przekroczeniu brak możliwości zalogowania się. Server się wyłącza, aby powrócić konieczne jest ponowne uruchomienie programu / servera.
- [ ] Wyświetlanie "menu"

    - wyswietlenie zawartosci katalogu / folderu
    - przejście do folderu
    - stworzenie folderu
    - pobierz plik
    - wyslij plik

## Przyszła rozbudowa / ulepszenie
Jeśli starczy czasu, chęci i życia to: 
- [ ] Wyczyszczenie kodu ze zbędnych lini testowych oraz zbyt obszernych komentarzy.
- [x] Zamana obsługi informacji na switch/cashe
- [x] Dodanie Funkcji Hashującej hasło (choć trochę zwiększenie bezpieczeństwa)

- [x] Dodanie funkcji do menu, na przykład:
    - skopiowanie podanego pliku,
    - wysłanie podanego pliku,
    - wyświetlenie plików w katalogu "udostępnionym"
    - dodanie zabezpieczeń przed usunięciem istniejących plików ( cybersecurity )
    - (usunięcie pliku)
- [ ] Dodanie prostego Interfejsu Graficznego po stronie klienta
- [ ] Modyfikacja sposobu pobierania loginów i haseł - czytanie z pliku to pociąga za sobą możliwość:
- [ ] Dodawanie użytkowników / ograniczenie pewnych opcji zależnie od użytkownika. 
- [ ] Dodanie Pliku z logami tego co się dzieje na serverze + niezależny program który cyklicznie wykonuje kopię tego pliku do zypełnie innej lokalizacji (w przypadku ataku i usunięcia śladów z tego pliku)
- [ ] Zamiana hashu na klucze asymetryczne
- [ ] Dodanie "jakiś" zabezpieczeń
- [ ] Dodanie do pola
- [ ] Przeniesienie pomysłów z kodu tutaj do README



## Wnioski
Projekt sam w sobie jest stosunkowo prosty, lecz przy obecnej cyfryzacji, umiejętności związane z samodzielnym zarządzaniem ruchem w sieci jest wręcz niezbędna dla każdego "informatyka"

## Zrodzone Pomysły W trakcje realizacji tego projektu
- [ ] Skaner otwartych portów na zadanym adresie IP ( są różne skanowania) najprościej byłoby patrzeć na odpowiedź jaką zwraca server.

- [ ] Stworzenie oraz rozwój własnych serverów w grach typu Minecraft

### mniej legalne
- [ ] Aplikacja / Strona "wymuszająca" połączenie na server (np jakaś wyskakująca reklama) w celu "uzyskania" danych o użytkowniku typu adres IP, marka urządzenia, inne dane. (choć i tak użytkownik musi wykonać ruch, np klikając w reklamę)
- [ ] Backdor / Zdalny dostęp - komputery zoombie, keyloggery - wysyłanie na zadany server tego co się dzieje na "kliencie" (zainfekowanym)
- [ ] Program (razem z jakimś skryptem bashowym)szukający w plikach jakiejś kombinacji typu login: / hasło: / adresów email (zawierające @) numerów telefonów, numery kont bankowych, plików excel etc, i wysyłanie ich zawartości / modyfikowanie na zadane wartości. 
