# Systemy operacyjne – lista 12 (2020-01-21)

Należy przygotować się do zajęć czytając następujące rozdziały książek:

* Tanenbaum (wydanie czwarte): 2.3, 2.5, 6.2, 6.6
* Arpaci-Dusseau: 28 ([Locks](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks.pdf)), 31 ([Semaphores](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf)), 32 ([Common Concurrency Problems](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-bugs.pdf))

UWAGA! W trakcie prezentacji należy być gotowym do zdefiniowania pojęć oznaczonych wytłuszczoną czcionką.

## Zadanie 1

*Podaj cztery warunki konieczne do zaistnienia zakleszczenia. Przeczytaj §6.6 oraz §32.3, a następnie wyjaśnij w jaki sposób można **przeciwdziałać zakleszczeniom** (ang. deadlock prevention)? Które z proponowanych rozwiązań stosuje się w praktyce i dlaczego? Czemu pozostałe są zbyt kłopotliwe?*

Warunki to:

1. Mutual exclusion condition – każdy zasób jest albo dostępny, albo przypisany do dokładnie jednego procesu.
2. Hold-and-wait condition – procesy przetrzymujące otrzymane wcześniej zasoby mogą prosić o następne.
3. No-preemption condition – zasoby przydzielone procesowi nie mogą zostać mu zabrane, a musi je on oddać sam.
4. Circular wait condition – zaistniał cykl procesów, w którym każdy czeka na następny.

Rozwiązania:

1. Ad 1
   * unikanie przydzielanie zasobów na wyłączność, np. printer daemon (wirtualizacja drukarki?) – naprawdę używane
2. Ad 2
   * podawanie przez proces z góry wszystkich zasobów, które będzie używał, i blokowanie ich na czas jego wykonania
   * wymaganie tymczasowego zwolnienia wszystkich poprzednio posiadanych zasobów przed próbą zażądania zasobu
3. Ad 3
   * zabranie zasobów procesowi – trudne albo nawet niemożliwe
   * wirtualizacja zasobu
4. Ad 4
   * pozwalanie procesowi na posiadanie tylko jednego zasobu w danej chwili – średnio akceptowalne
   * ponumerowanie zasobów i pozwalanie tylko na pobieranie zasobów w kolejności rosnących (malejących) numerów; w ten sposób nigdy nie powstanie cykl (grafu przydziału) – praktyczne

## Zadanie 2 (P)

*Podaj w pseudokodzie semantykę **instrukcji atomowej** `compare-and-swap` i z jej użyciem zaimplementuj **blokadę wirującą** (ang. spin lock) (§28.7). Zakładając, że typ «`spin_t`» jest równoważny «`int`», podaj ciało procedur «`void lock(spin_t *)`» i «`void unlock(spin_t *)`». Dlaczego użycie blokad wirujących ma sens tylko w oprogramowaniu uruchamianym na maszynach wieloprocesorowych? Podaj główne różnice między blokadami wirującymi, a **blokadami usypiającymi** (§28.14).*

``` c
#define LOCKED 0
#define UNLOCKED 1
typedef int spin_t

// returns old_value
extern int compare_and_swap
    (int *addr, int expected_value, int target_value);

spin_t lock(spin_t* l)
{
    while(compare_and_swap(&l, UNLOCKED, LOCKED) == LOCKED)
    { /* nothing */ }
}

void unlock(spin_t* lock)
{
    assert(compare_and_swap(l, LOCKED, UNLOCKED) == LOCKED);
}
```

Blokada wirująca polega na ciągłym sprawdzaniu przez proces czy zasób jest dostępny. Blokada usypiająca zaś polega na uśpieniu wątku czekającego na zasób i wybudzeniu go, gdy zasób zostanie zwrócony.

|            blokada wirująca             |           blokada usypiająca           |
| :-------------------------------------: | :------------------------------------: |
|     czekający zużywa czas procesora     |       czekający nie zużywa czasu       |
|         nie wymaga wsparcia SO          |           wymaga wsparcia SO           |
| lepsza gdy czas oczekiwania jest krótki | lepsza gdy czas oczekiwania jest długi |

Blokada wirująca wiąże się z marnowaniem czasu procesora. Jeśli proces posiadający i czekający na zasób byłyby uruchomione na jednym procesorze, czekanie spowalniałoby pracę procesu posiadającego zasób.

## Zadanie 3

*Przeanalizuj poniższy pseudokod wadliwego rozwiązania problemu producent-konsument. Zakładamy, że kolejka «`queue`» przechowuje do $n$ elementów. Wszystkie operacje na kolejce są **atomowe** (ang. atomic). Startujemy po jednym wątku wykonującym kod procedury «`producer`» i «`consumer`». Procedura «`sleep`» usypia wołający wątek, a «`wakeup`» budzi wątek wykonujący daną procedurę. Wskaż przeplot instrukcji, który doprowadzi do  
(a) błędu wykonania w linii `6` i `13`  
(b) zakleszczenia w liniach `5` i `12`.*

``` python
 1 def producer():
 2   while True:
 3     item = produce()
 4     if queue.full():
 5       sleep()
 6     queue.push(item)
 7     if not queue.empty():
 8       wakeup(consumer)
 9 def consumer():
10   while True:
11     if queue.empty():
12       sleep()
13     item = queue.pop()
14     if not queue.full():
15       wakeup(producer)
16     consume(item)
```

*Wskazówka: Jedna z usterek na którą się natkniesz jest znana jako problem zagubionej pobudki (ang. lost wake-up problem).*

*Lost wake-up problem* – sytuacja w której proces powiadomienie wybudzające, zanim zaszedł w sen, z którego miał zostać wybudzony.

Problem wynika z tego, że pary linijek 4 – 5, 11 – 12, a być może też 7 – 8 oraz 14 – 15, są nieatomowe. Wystąpi kiedy jeden proces będzie spał, bo kolejka jest pusta, albo drugi, bo jest pełna.

1. Błąd wykonania w linijce 6
   po linijce 5 nastąpiło przełączenie, a po powrocie warunek z linijki 4 okazał się być prawdziwy
2. Błąd wykonania w linijce 13
   po linijce 15 nastąpiło przełączenie, a po powrocie warunek z linijki 14 nie był już prawdziwy
3. Zakleszczenie w linii 5
   po linijce 4 nastąpiło przełączenie, a po powrocie warunek z linijki 4 nie był już prawdziwy
4. Zakleszczenie w linii 12
   po linijce 11 nastąpiło przełączenie, a po powrocie warunek z linijki 11 nie był już prawdziwy



## Zadanie 4

*Poniżej podano błędną implementację semafora zliczającego z użyciem semaforów binarnych. Dopuszczamy, żeby «`count`» był liczbą ujemną, w takim przypadku jej wartość bezwzględna oznacza liczbę uśpionych procesów. Znajdź kontrprzykład i zaprezentuj wszystkie warunki niezbędne do jego odtworzenia.*

``` c
 0 // counting semaphore
 1 struct csem {
 2   bsem mutex;
 3   bsem delay;
 4   int count;
 5 };
 7 void csem::csem(int v) { // v – liczba zasobów
 8   mutex = 1;
 9   delay = 0;
10   count = v;
11 }
```

``` c
12 // proberen – opuszczanie semafora (zmniejszenie licznika/zabranie zasobu)
13 void csem::P() {
14   P(mutex);
15   count--;
16   if(count < 0) { // ktoś czeka
17     V(mutex);
18     P(delay); // blok delay
19   } else {
20     V(mutex);
21   }
22 }
```

```c
// verhogen – podnoszenie semafora (zwiększenie licznika/zwolnienie zasobu)
23 void csem::V() {
24   P(mutex);
25   count++;
26   if (count <= 0) // nie ma nic wolnego
27     V(delay); // odblok delay
28   V(mutex);
29 }
```

W tym kodzie semafor binarny opuszczony, czyli zajęty, ma wartość 0, zaś podniesiony 1.

Zamierzeniem było zapewnienie niepodzielności zmian licznika semaforem o nazwie `mutex` oraz użycie semafora `delay`, by w przypadku braku zasobów zablokować ich pobieranie do momentu, gdy jakieś zostają zwrócone. Jednak nieatomowa jest para instrukcji 17 – 18. Załóżmy że ktoś podnosi semafor delay przy zwracaniu, ale nikt go nie opuścił, bo wcześniej został wywłaszczony. Błąd jest indukowany przez poniższy przeplot:

Niech liczba zasobów: `0`, wait: `locked`, mutex: `unlocked`

| A                                                    | B                                                      | C                           |
| ---------------------------------------------------- | ------------------------------------------------------ | --------------------------- |
| `{ counter = 0, wait = locked, mutex = unlocked }`   |
| `P()`: $12 – 17$                                     |                                                        |                             |
| `{ counter = -1, wait = locked, mutex = unlocked }`  |
|                                                      | `P()`: $12 – 17$                                       |                             |
| `{ counter = -2, wait = locked, mutex = unlocked }`  |
|                                                      |                                                        | `V()` $\checkmark$ (koniec) |
|                                                      |                                                        | `V()` $\checkmark$ (koniec) |
| `{ counter = 0, wait = unlocked, mutex = unlocked }` |
|                                                      | `P()` = $18 – 22$ $\checkmark$ (koniec) – blokuje wait |                             |
| `{ counter = 0, wait = locked, mutex = unlocked }`   |
| `P()`: $18$ i wciąż czeka, bo `wait = locked`        |                                                        |                             |

Jeśli ktoś zwolni zasób, nie odblokuje `delay`, bo $counter \ge 0$, więc nie wybudzi wątku A.

## Zadanie 5

*Poniżej podano jedno z rozwiązań **problemu ucztujących filozofów**. Zakładamy, że istnieją tylko leworęczni i praworęczni filozofowie, którzy podnoszą odpowiednio lewą i prawą pałeczkę jako pierwszą. Pałeczki są ponumerowane zgodnie z ruchem wskazówek zegara. Udowodnij, że jakikolwiek układ $n ≥ 5$ ucztujących filozofów z co najmniej jednym leworęcznym i praworęcznym zapobiega zakleszczeniom i głodzeniu.*

``` c
semaphore fork[N] = {1, 1, 1, 1, 1, ...};
```

``` c
 1 void righthanded (int i) {
 2   while (true) {
 3     think();
 4     P(fork[(i+1) mod N]);
 5     P(fork[i]);
 6     eat();
 7     V(fork[i]);
 8     V(fork[(i+1) mod N]);
 9   }
10 }
```

``` c
13 void lefthanded (int i) {
14   while (true) {
15     think();
16     P(fork[i]);
17     P(fork[(i+1) mod N]);
18     eat();
19     V(fork[(i+1) mod N]);
20     V(fork[i]);
21   }
22 }
```

**1. W żadnym stanie nie dojdzie do zakleszczenia:**  
   Załóżmy nie wprost, że istnieje pewna liczba zakleszczonych filozofów, w tym filozof $F_i$B, który bez straty niech będzie leworęczny

   1. $F_i$ nie ma dostępu do żadnego widelca.  
      Po jego lewej jest zakleszczony filozof $F_{i-1}$, bo inaczej $F_i$ nie byłby zakleszczony. Skoro tak, to $F_{i-1}$ jest praworęczny. Iterując okaże się, że $F_i$ jest praworęczny, a z założenia miał być lewo–.
    2. $F_i$ ma widelec, i to lewy, bo akurat zakładamy jego leworęczność.  
       Blokuje go brak prawego widelca. $F_{i + 1}$ też jest zakleszczony, bo inaczej oddałby widelec. $F_{i + 1}$ jest więc leworęczny. Iterując, wszyscy, sprzecznie z założeniem, są leworęczni.

<!-- Załóżmy, że istnieje pewna liczba filozofów $F_i$, w której każdy czeka na widelec swojego sąsiada i trzyma już jeden widelec (dlaczego możemy założyć, że trzymają już jeden?).
Bez straty ogólności rozumowania załóżmy, że $F_j$ jest leworęczny. Skoro $F_j$ trzyma lewy widelec, a nie może wziąć prawego, oznacza to, że jego prawy sąsiad też jest leworęczny. Iterując dojdziemy do wniosku, że wszyscy filozofowie są leworęczni, a to byłoby sprzeczne z założeniem. -->

**2. Nigdy nie dojdzie do zagłodzenia:**  
Załóżmy, że leworęczny filozof $F_j$ nigdy nie może zacząć jeść.  

   1. Załóżmy, że $F_j$ nie trzyma żadnego widelca.  
   Wtedy jego lewy sąsiad $F_i$, który trzyma swój prawy widelec, nigdy nie może zakończyć jedzenia. Skoro tak, $F_i$ nigdy nie dostaje swojego lewego widelca, więc jego lewy sąsiad też musi być praworęczny. Iterując rozumowanie, kolejni lewi sąsiedzi będą musieli być praworęczni i zagłodzeni, ale w końcu dojdziemy do wyjściowego filozofa, który jest leworęczny. Sprzeczność.
   2. Załóżmy, że $F_j$ trzyma widelec (lewy, bo jest leworęczny).  
   Skoro jest zagłodzony, to na pewno nie ma prawego widelca. Prawy sąsiad też ma jeden widelec, więc jest leworęczny (bo gdyby miał oba to w końcu by je oddał). Iterujemy dalej i pociąg się wykoleił – wszyscy są leworęczni, a nie takie było założenie.
