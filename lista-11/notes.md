# a

## Zadanie 1

Zakleszczenie (deadlock) – sytuacja, w której w grupie programów każdy (explicite lub nie) czeka na podjęcie akcji przez któryś program z tej grupy. Do skrzyżowania bez ustalonego pierwszeństwa podjeżdżają w tej samej chwili cztery pojazdy chcące jechać prosto.

Uwięzienie (livelock) – sytuacja, w której odpowiedzią na konfliktujący stan dwóch lub więcej programów jest odpowiedź prowadząca do zapętlenia stanów. Uzupełnijmy poprzednią sytuację o następujące reagowanie na zakleszczenie: gdy wykryto zakleszczenie i da się cofnąć, cofnij się i spróbuj znowu przejechać skrzyżowanie. Jeśli pojazdy będą przyjeżdżać i odjeżdżać od skrzyżowania w tym samym czasie, ciągłe próby nie będą prowadziły do postępu.

Głodzenie (starvation) – sytuacja, kiedy programowi odmawiany jest dostęp do zasobów, które są niezbędne do jego działania. Jeśli po drodze z pierwszeństwem porusza się nieprzerwany ciąg pojazdów, pojazdy z drogi bez pierwszeństwa przejazdu będą oczekiwały bez końca.

Wykrywanie zakleszczeń – można stworzyć graf zależności (wychodzące krawędzie oznaczają zależność od zasobu, a wchodzące oznacza zajęcie zasobu) i przeszukać w poszukiwaniu cyklu.
Jeśli jest jest wiele zasobów jednego rodzaju, tworzymy macierz.

Zapobieganie zakleszczeń – zakleszczenia powstają z wspólnego wystąpienia czterech warunków. Wystarczy złamać jeden z nich:

1. Mutual Exclusion – unikamy przydzielania wyłączności na korzystanie z zasobów
2. Hold-and-Wait Condition – jeśli czekasz na zasób, wycofaj się
3. No-Preemption Condition – wirtualizacja zasobów, na przykład printer daemon
4. Circular Wait Condition – numerujemy zasoby, pozwalamy czekać tylko na zasób o wyższym numerze, w przeciwnym numerze wycofujemy się.
  albo pozwalamy na wyłączność tylko dla jednego zasobu na proces (niepraktyczne)

Usuwanie zakleszczeń – po wykryciu:

1. zabicie procesu, który bez problemu wykona się od początku
2. wywłaszczenie procesu i oddanie zasobów
3. rollback to checkpoint

## Zadanie 2

Sytuacja wyścigu: wynik zależy od kolejności.

$2$ wątki:
Maksymalna wartość – 100, kiedy pary odczyt–zapis będą niepodzielne.
Minimalna wartość:
A: wczytuje 0 ze zmiennej do rejestru
B: wczytuje 0 ze zmiennej do rejestru
A: kontynuuje aż do wpisania 49 do zmiennej, wpisuje 49 do zmiennej
B: zwiększa 0 w rejestrze, wpisuje 1 do zmiennej
A: wczytuje 1 ze zmiennej do rejestru
B: wykonuje się 50 razy i zakańcza wpisując 50 do zmiennej
A: inkrementuje 1 i wpisuje 2 do zmiennej
wartość to 2.

$k$ wątków:
Maksymalna wartość: $k * 50$: oczekiwanie.
Minimalna wartość:
A: wczytuje 0 ze zmiennej do rejestru
B: wczytuje 0 ze zmiennej do rejestru

C – K: wczytują 0 ze zmiennej do rejestru

A: kontynuuje aż do wpisania 49 do zmiennej, wpisuje 49 do zmiennej
B: zwiększa 0 w rejestrze, wpisuje 1 do zmiennej
A: wczytuje 1 ze zmiennej do rejestru
B: wykonuje się 50 razy i zakańcza wpisując 50 do zmiennej

C – K: wykonują się 50 razy
zmienna zostaje zwiększona o $(k - 1) * 50$

A: inkrementuje 1 i wpisuje 2 do zmiennej

wartość to 2.

## Zadanie 3

Chcemy razem 7 – 9

proces | proces
---
0|1
 | 4 – 7
4 – 12 |
 | 7 – 10!
 | 11 – 12
oba są w sekcji krytycznej

## Zadanie 4

## Zadanie 5

## Zadanie 6

Nie ma takiego wywołania funkcji, które poczekałoby na dowolny wątek.
Zrobione.

## Zadanie 7

Zrobione

## Zadanie 8

'bug-1.c`

```console
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 1000
OK cnt=2000
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 100000000
BOOM! cnt=116101497
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 100000000
BOOM! cnt=110652684
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 10000000 
BOOM! cnt=11867494
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 1000000 
BOOM! cnt=1000000
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 2000000
BOOM! cnt=2915980
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 200000 
OK cnt=400000
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 150000
OK cnt=300000
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 155000
OK cnt=310000
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 155500
OK cnt=311000
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 155555 
OK cnt=311110
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 185555
OK cnt=371110
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 188555
OK cnt=377110
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 198555
OK cnt=397110
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 198599
OK cnt=397198
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 199999
BOOM! cnt=199999
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 199999
OK cnt=399998
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 199999
OK cnt=399998
 Deb  ~/v/g/systemy-operacyjne-2019/so19_lista_11  on master ⇡2 *3 !8 ?25  ./bug-1 1999
```

Czy odpowiedź to "dużych"? :v
