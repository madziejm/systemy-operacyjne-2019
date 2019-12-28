# Systemy operacyjne – lista 9 2019-12-10

*Należy przygotować się do zajęć czytając następujące rozdziały książek lub publikacji:*

* *[Dynamic Storage Allocation: A Survey and Critical Review](https://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.47.275): 1 – 3*

## Zadanie 1

*Algorytm przydziału pamięci udostępnia funkcje o sygnaturach «`alloc: words -> @id`» i «`free: @id -> void`» i ma do dyspozycji obszar 50 słów maszynowych. Implementacja używa dwukierunkowej listy wolnych bloków oraz boundary tags. Wyszukiwanie wolnych bloków działa zgodnie z polityką best-fit. Operacja zwalniania gorliwie złącza bloki i wstawia wolne bloki na początek listy. Posługując się diagramem z wykładu wykonaj krokową symulację algorytmu przydziału pamięci dla poniższego ciągu żądań. Należy wziąć pod uwagę miejsce zajmowane przez struktury danych algorytmu przydziału oraz nieużytki.
`alloc(5) alloc(12) alloc(15) alloc(8) free(@2) free(@1) free(@3) alloc(10)`
Uwaga: Funkcja «`alloc`» zwraca bloki o kolejnych identyfikatorach począwszy od `@1`. Adresy są wyrównane do długości słowa.*

Na kartce

## Zadanie 2 (bonus)

*Rozważmy algorytm przydziału pamięci z zadania pierwszego. Załóżmy, że implementujemy go na architekturze 64-bitowej – zatem słowo maszynowe ma 8 bajtów. Zarządzane areny będą nie większe niż 512KiB. Wskaźnik zwracany przez operację «`malloc`» będzie podzielny przez 16. Zaproponuj metodę efektywnego kodowania metadanych, aby zminimalizować narzut pamięciowy, oraz strategię wykrywania uszkodzenia danych algorytmu (tj. lista dwukierunkowa, boundary tags).  
Wskazówki: Rozważ właściwości wskaźników wynikające z rozmiaru areny.*

## Zadanie 3

*Rozważmy algorytm kubełkowy (§3.6) (ang. segregated-fit) przydziału pamięci z gorliwym złączaniem wolnych bloków. Jak przebiegają operacje «`malloc`» i «`free`»? Co robi «`malloc`», gdy na danej liście nie ma wolnego bloku żądanego rozmiaru? Jak poradzić sobie w trakcie złączania wolnych bloków w procedurze «`free`», jeśli chcemy usunąć ostatni element z listy? Rozważ zastosowanie leniwego złączania wolnych bloków w algorytmie kubełkowym przydziału pamięci – jakie problemy zauważasz?*

*Ściągnij ze strony przedmiotu archiwum «`so19_lista_9.tar.gz`», następnie rozpakuj je i zapoznaj się z dostarczonymi plikami.*

## Zadanie 4 (P)

*Programy «`bad-*.c`» posiadają kilka ukrytych błędów związanych z używaniem bloków pamięci przydzielonych dynamicznie. Zauważ, że kompilator nie zgłosił żadnych błędów ani ostrzeżeń, a uruchomienie tych programów nie kończy się błędem. Wytypuj błędy i zapisz je w swoich notatkach.
Następnie skompiluj programy z instrumentacją kodu dodaną przez [address sanitizer](https://en.wikipedia.org/wiki/AddressSanitizer) – wystarczy odkomentować odpowiednią linię w pliku «`Makefile`» i ponownie zbudować pliki wykonywalne. Po uruchomieniu programów otrzymasz komunikaty błędów – wyjaśnij je. Porównaj znalezione błędy z zanotowanymi kandydatami, a następnie popraw kod źródłowy.*

Zrobione. Najbardziej przebiegły okazał się ostatni kod, rozwinięte makro nie miało sensu.

## Zadanie 5 (bonus)

*Zapoznaj się z artykułem [AddressSanitizer: A Fast Address Sanity Checker](https://www.usenix.org/system/files/conference/atc12/atc12-final39.pdf). Jaki jest koszt używania tego narzędzia (§4)? Przedstaw zasadę działania tego narzędzia – w szczególności opowiedz o instrumentacji kodu (§3.1) i shadow map (§3.2). W poprzednim zadaniu nie zaprogramowano wszystkich błędów, z których wykryciem radzi sobie address sanitizer – podaj kilka przykładów.*

* Średnie uzyskane spowolnienie wyniosło 76%, największe około 2,5 raza.
* Przyrost użytej pamięci to przeciętnie 3,37 razu, i pochodzi on głównie z użycia `redzones`
* Zwiększenie rozmiaru stosu było pomijalne
* Rozmiar pliku wykonywalnego wzrósł od 1,5 do 3,2 razu, ze średnią 2,5 razu.
* Porównanie z innymi podobnymi narzędziami jest trudne

Pojęcie [instrumentation](https://stackoverflow.com/questions/8755211/what-is-meant-by-the-term-instrumentation) odnosi się do możliwości obserwowania lub mierzenia poziomu wydajności systemu, diagnozowania błędów i zapisywania śladów (trace) poprzez zmianę kodu wykonywalnego statycznie lub dynamicznie[1](https://en.wikipedia.org/wiki/Profiling_(computer_programming)#Instrumentation). Ma większy narzut, ale może być dokładniejsza.

Skoro pamięć zwracana przez malloc jest wyrównana do ośmiu bajtów (tak jest w dokumencie, ale naprawdę na sześćdziesięcioczterobitowych komputerach chyba jest do 16), to wybierając dowolny kawałek sterty aplikacji, pierwsze 0 – 8 bajtów kawałka jest adresowalnych, a pozostałe końcowe 8 - k bajtów – nie. Taka informacja o każdym kawałku może być potencjalnie zapisana w jednym bajcie shadow memory. Dlatego AddressSanitizer przydziela shadow memory 1/8 pamięci wirtualnej i używa mapowania adresów aplikacji na shadow memory. Przydział shadow memory następuje zanim aplikacja zacznie działać, więc nie dojdzie do interferencji. Mapowanie adresu aplikacji na shadow byte to `(Addr>>3)+Offset`

AddressSanitizer tłumaczy adres każdego zapisu na shadow address. Gdy dostęp jest do ośmiobajtowego kawałka pamięci, to sprawdza czy pod tym adresem znajduje się zero.
Jeśli nie, zapis był błędny. Jeśli do 1-, 2- lub 4-bajtowego, algorytm wyraża się jak niżej:

``` c
ShadowAddr = (Addr >> 3) + Offset;
k = *ShadowAddr; // k pierwszych bajtów jest adresowalnych
if (k != 0 && ((Addr & 7) + AccessSize > k)) // (Addr & 7) – offset względem początku ośmiobajtowego kawałka
    ReportAndCrash(Addr);
```

* zmienne na stercie  
 AddressSanitizer nadpisuje implementacje `malloc` oraz `free` takimi, które otaczają każde przydzielone miejsce nieadresowalnymi blokami redzone.

* zmienne globalne  
 Redzones wokół zmiennych globalnych są tworzone podczas kompilacji, a ich adresy przekazywane bibliotece przy starcie aplikacji, która oznacza (poison) te adresy w celu późniejszego zgłaszania błędów.

* zmienne na stosie  
 Obiekty przydzielane na stosie są otaczane redzones, a redzones oznaczone poison podczas czasu wykonywania/run-time.

> The free function poisons the entire memory regionand puts it intoquarantine, such that this region will notbe allocated bymallocany time soon. Currently, thequarantine is implemented as a FIFO queue which holdsa fixed amount of memory at any time.

Instrumentacja jest przygotowywana w ostatniej fazie kompilacji, w momencie końcowych faz optymalizacji. Dodaje w kodzie tylko jeden odczyt na jeden odczyt aplikacji.

* Use after free (dangling pointer dereference)
* Heap buffer overflow
* Stack buffer overflow
* Global buffer overflow
* Use after return
* Use after scope
* Initialization order bugs (multiple includes)
* Memory leaks
