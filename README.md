# Zdalny dostęp do komputera

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
