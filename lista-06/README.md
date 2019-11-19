# Systemy operacyjne – lista 6 2019-11-19

Należy przygotować się do zajęć czytając następujące rozdziały książek:

* APUE (wydanie trzecie): 3.13, 4.2-4.5, 4.16-4.17. 4.21, 4.24-4.24

## Zadanie 1 (P)

*Przeczytaj krytykę kluczowej idei systemu Unix, tj. [A Unix File Is Just a Big Bag of Bytes](http://www.catb.org/~esr/writings/taoup/html/ch20s03.html#id3015538). Na podstawie [Resource Fork](https://en.wikipedia.org/wiki/Resource_fork) wyjaśnij czym były dodatkowe zasoby pliku w historycznych systemach MacOS. Jaką  postać  mają rozszerzone atrybuty pliku `xattr(7)`?  Gdzie  są  one  składowane  w  systemie  plików? Poleceniem `wget(1)` z opcją «`--xattr`» pobierz z Internetu plik, po czym wyświetl jego rozszerzone atrybuty przy pomocy polecenia `getfattr(1)`. Następnie policz sumę md5 wybranego pliku i przypisz ją do atrybutu «`user.md5sum`» poleceniem `setfattr(1)`, po czym sprawdź czy operacja się powiodła.*

*Resource fork* to część pliku zawierająca jego zasoby, których struktura może być przeczytana przez sam system operacyjny. Miejsce przechowywania rozszerzonych atrybutów pliku jest zależne od systemu plików.

[ext4 Data Structures and Algorithms](https://www.kernel.org/doc/html/latest/filesystems/ext4/dynamic.html#extended-attributes):

> Extended attributes (xattrs) are typically stored in a separate data block on the disk and referenced from inodes via inode.i_file_acl.  
> There are two places where extended attributes can be found. The first place is between the end of each inode entry and the beginning of the next inode entry. For example, if inode.i_extra_isize = 28 and sb.inode_size = 256, then there are 256 - (128 + 28) = 100 bytes available for in-inode extended attribute storage. The second place where extended attributes can be found is in the block pointed to by inode.i_file_acl.

``` console
user@d3b14n :~$ wget --xattr http://ftp.pl.debian.org/debian/pool/main/h/hello/hello_2.9-2+deb8u1_amd64.deb
[wget output]
user@d3b14n :~$ getfattr hello_2.9-2+deb8u1_amd64.deb
# file: hello_2.9-2+deb8u1_amd64.deb
user.xdg.origin.url
user@d3b14n :~$ getfattr -n user.xdg.origin.url hello_2.9-2+deb8u1_amd64.deb
# file: hello_2.9-2+deb8u1_amd64.deb
user.xdg.origin.url="http://ftp.pl.debian.org/debian/pool/main/h/hello/hello_2.9-2+deb8u1_amd64.deb"
user@d3b14n :~$ md5sum hello_2.9-2+deb8u1_amd64.deb
64a92a50c745f79aa17de37f9e7a97ff  hello_2.9-2+deb8u1_amd64.deb
user@d3b14n :~$ setfattr -n user.md5sum -v 64a92a50c745f79aa17de37f9e7a97ff hello_2.9-2+deb8u1_amd64.deb
user@d3b14n :~$ getfattr -n user.md5sum hello_2.9-2+deb8u1_amd64.deb
# file: hello_2.9-2+deb8u1_amd64.deb
user.md5sum="64a92a50c745f79aa17de37f9e7a97ff"
```

*Ściągnij ze strony przedmiotu archiwum «`so19_lista_6.tar.gz`», następnie rozpakuj i zapoznaj się z dostarczonymi plikami.  
UWAGA! Można modyfikować tylko te fragmenty programów, które zostały oznaczone w komentarzu napisem «`TODO`».*

## Zadanie 2 (2pkt, P)

*Program  «`writeperf`»  służy  do  testowania  wydajności  operacji  zapisu  do  pliku. Nasz [microbenchmark](https://en.wikipedia.org/wiki/Benchmark_(computing)) wczytuje z linii poleceń opcje i argumenty opisane dalej. Na standardowe wyjście drukuje `t` trójkątów (opcja «`-t`») prostokątnych o boku złożonym z `l` znaków gwiazdki «``*``» (opcja «``-l``»). Jeśli standardowe wyjście zostało przekierowane do pliku oraz została podana opcja «`-s`», to przed zakończeniem programu bufory pliku zostaną zsynchronizowane z dyskiem wywołaniem `fsync(2)`. Program realizuje pięć wariantów zapisu do pliku:*

* *Każdą linię trójkąta zapisuje osobno wywołaniem `write(2)` (argument «`write`»).*
* *Używa strumienia biblioteki stdio bez buforowania (argument «`fwrite`»), z buforowaniem liniami (argument «`fwrite-line`») i buforowaniem pełnym (argument «`fwrite-full`»).*
* *Wykorzystuje wywołanie systemowe `writev(2)` do zapisania do «`IOV_MAX`» linii na raz.*

*Twoim  zadaniem  jest  odpowiednie  skonfigurowanie  bufora  strumienia  «`stdout`»  z  użyciem  procedury `setvbuf(3)` oraz zaimplementowanie metody zapisu z użyciem «`writev`».  
Przy pomocy skryptu powłoki «`writeperf.sh`» porównaj wydajność wymienionych wcześniej metod zapisu. Uzasadnij przedstawione wyniki. Miej na uwadze liczbę wywołań systemowych (należy to zbadać posługującsię narzędziem `strace(1)` z opcją «`-c`») oraz liczbę kopii danych wykonanych celem przesłania zawartości linii do buforów dysku.*

Zrobione.

``` console
user@d3b14n :~$ ./writeperf.sh
Method: write

real    1m34.208s
user    0m0.300s
sys     1m7.194s
594c417685170dd3eb60286c0f634dc9  test

Method: fwrite

real    1m32.737s
user    0m0.871s
sys     1m7.143s
594c417685170dd3eb60286c0f634dc9  test

Method: fwrite-line

real    1m33.120s
user    0m1.256s
sys     1m6.604s
594c417685170dd3eb60286c0f634dc9  test

Method: fwrite-full

real    0m17.783s
user    0m0.048s
sys     0m6.878s
594c417685170dd3eb60286c0f634dc9  test

Method: writev

real    1m56.524s
user    0m0.000s
sys     0m50.562s
594c417685170dd3eb60286c0f634dc9  test
```

``` console
user@d3b14n :~$ strace -c ./writeperf -t 1000 -l 1000 -s write 1>/dev/null
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
100.00   10.750273          10   1000000           write
  0.00    0.000016          16         1         1 ioctl
  0.00    0.000014          14         1         1 fsync
  0.00    0.000000           0         1           read
  0.00    0.000000           0         2           close
  0.00    0.000000           0         2           fstat
  0.00    0.000000           0         7           mmap
  0.00    0.000000           0         4           mprotect
  0.00    0.000000           0         1           munmap
  0.00    0.000000           0         3           brk
  0.00    0.000000           0         1         1 access
  0.00    0.000000           0         1           execve
  0.00    0.000000           0         1           arch_prctl
  0.00    0.000000           0         2           openat
------ ----------- ----------- --------- --------- ----------------
100.00   10.750303               1000027         3 total
```

``` console
user@d3b14n :~$ strace -c ./writeperf -t 1000 -l 1000 -s fwrite 1>/dev/null
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
100.00    9.712961           9   1000000           write
  0.00    0.000093          13         7           mmap
  0.00    0.000082          20         4           mprotect
  0.00    0.000044          44         1           munmap
  0.00    0.000037          12         3           brk
  0.00    0.000018           9         2           close
  0.00    0.000016          16         1           arch_prctl
  0.00    0.000005           5         1         1 ioctl
  0.00    0.000003           3         1         1 fsync
  0.00    0.000000           0         1           read
  0.00    0.000000           0         2           fstat
  0.00    0.000000           0         1         1 access
  0.00    0.000000           0         1           execve
  0.00    0.000000           0         2           openat
------ ----------- ----------- --------- --------- ----------------
100.00    9.713259               1000027         3 total
```

``` console
user@d3b14n :~$ strace -c ./writeperf -t 1000 -l 1000 -s fwrite-line 1>/dev/null
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 99.97    8.730095           8   1000000           write
  0.01    0.001298         185         7           mmap
  0.00    0.000347         347         1           read
  0.00    0.000270         135         2           close
  0.00    0.000261         261         1           munmap
  0.00    0.000248          62         4           mprotect
  0.00    0.000088          44         2           fstat
  0.00    0.000065          32         2           openat
  0.00    0.000041          13         3           brk
  0.00    0.000009           9         1         1 ioctl
  0.00    0.000004           4         1         1 fsync
  0.00    0.000000           0         1         1 access
  0.00    0.000000           0         1           execve
  0.00    0.000000           0         1           arch_prctl
------ ----------- ----------- --------- --------- ----------------
100.00    8.732726               1000027         3 total
```

``` console
user@d3b14n :~$ strace -c ./writeperf -t 1000 -l 1000 -s fwrite-full 1>/dev/null
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 99.78    1.448674          11    122437           write
  0.19    0.002788         929         3           brk
  0.01    0.000174          24         7           mmap
  0.01    0.000104          26         4           mprotect
  0.00    0.000046          46         1           munmap
  0.00    0.000032          16         2           openat
  0.00    0.000020          20         1           read
  0.00    0.000017           8         2           fstat
  0.00    0.000016           8         2           close
  0.00    0.000014          14         1           arch_prctl
  0.00    0.000000           0         1         1 ioctl
  0.00    0.000000           0         1         1 access
  0.00    0.000000           0         1           execve
  0.00    0.000000           0         1         1 fsync
------ ----------- ----------- --------- --------- ----------------
100.00    1.451885                122464         3 total
```

``` console
user@d3b14n :~$ strace -c ./writeperf -t 1000 -l 1000 -s writev 1>/dev/null
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 99.90    0.024028          24       977           writev
  0.06    0.000014          14         1         1 ioctl
  0.05    0.000011          11         1         1 fsync
  0.00    0.000000           0         1           read
  0.00    0.000000           0         2           close
  0.00    0.000000           0         2           fstat
  0.00    0.000000           0         7           mmap
  0.00    0.000000           0         4           mprotect
  0.00    0.000000           0         1           munmap
  0.00    0.000000           0         3           brk
  0.00    0.000000           0         1         1 access
  0.00    0.000000           0         1           execve
  0.00    0.000000           0         1           arch_prctl
  0.00    0.000000           0         2           openat
------ ----------- ----------- --------- --------- ----------------
100.00    0.024053                  1004         3 total
```

Zasadniczo wyniki nie powinny być traktowane jako wiarygodne, bo pochodzą z uruchomienia w maszynie wirtualnej.

## Zadanie 3 (P)

*Program «`id`» drukuje na standardowe wyjście tożsamość, z którą został utworzony, np.:*

``` console
$ id
uid=1000(cahir) gid=1000(cahir) groups=1000(cahir),20(dialout),24(cdrom),25(floppy),
27(sudo),29(audio),30(dip),44(video),46(plugdev),108(netdev),123(vboxusers),999(docker)
```

*Uzupełnij  procedurę  «`getid`»  tak  by  zwracała  identyfikator  użytkownika `getuid(2)`,  identyfikator  grupy `getgid(2)` oraz tablicę identyfikatorów i liczbę grup dodatkowych `getgroups(2)`. Nie możesz z góry założyć liczby  grup,  do  których  należy  użytkownik.  Dlatego  należy  stopniowo  zwiększać  rozmiar  tablicy  «`gids`» przy pomocy `realloc(3)`, aż pomieści rezultat wywołania «`getgroups`». Należy również uzupełnić ciało procedur «`uidname`» i «`gidname`» korzystając odpowiednio z `getpwuid(3)` i `getgrgid(3)`.*

Chyba miało być napisane «otworzony».  
Zrobione.

## Zadanie 4 (P)

*Program «`listdir`» drukuje zawartość katalogu w formacie podobnym do wyjścia polecenia «`ls -l`». Poniżej można znaleźć przykładowy wydruk, na którym widnieją odpowiednio: plik zwykły, dowiązanie symboliczne, urządzenie znakowe, plik wykonywalny z bitem set-uid, jeden katalog z ustawionym bitem set-gid i drugi z bitem sticky.*

``` console
-rw-r--r--   1  cahir  cahir   2964  Fri Nov 15 14:36:59 2019 listdir.c
lrwxrwxrwx   1  cahir  cahir     17  Mon Nov  4 11:14:49 2019  libcsapp -> ../csapp/libcsapp
crw--w----   1  cahir    tty  4,  2  Tue Nov 12 08:42:33 2019  tty2
-rwsr-xr-x   1   root   root  63736  Fri Jul 27 10:07:37 2018  passwd
drwxrwsr-x  10   root  staff   4096  Mon Jan  9 13:49:40 2017  local
drwxrwxrwt  23   root   root  12288  Fri Nov 15 16:01:16 2019  tmp
```

*Uzupełnij kod programu według wskazówek zawartych w komentarzach w kodzie źródłowym. Należy użyć:*

* *`fstatat(2)` do przeczytania metadanych pliku,*
* *`major(3)` i `minor(3)` do zdekodowania numeru urządzenia,*
* *`readlinkat(2)` to przeczytania ścieżki zawartej w dowiązaniu symbolicznym.*

*Implementacja iterowania zawartości katalogu będzie wymagała zapoznania się ze strukturą «`linux_dirent`» opisaną  w  podręczniku `getdents(2)`.  Wywołanie  systemowe  «`getdents`»  nie  jest  eksportowane  przez bibliotekę standardową, zatem należało je wywołać pośrednio – zobacz plik «`libcsapp/Getdents.c`».*

Jak wyciągnąć typ pliku z pola `stat.st_mode` `struct stat` jest napisane w manualu `man inode`. Więcej informacji w `man 2 stat`.  
Zrobione.

## Zadanie 5 (2pkt, P)

*(Pomysłodawcą zadania jest Tomasz Wierzbicki.)  
Program  «`mergesort`»  odczytuje  ze  standardowego  wejście  liczbę  naturalną n,  po  czym  czyta n liczb całkowitych. Program realizuje algorytm sortowania przez scalanie. Proces główny zajmuje się wczytywaniem danych  wejściowych  i  drukowaniem  posortowanego  ciągu.  Żeby  posortować  liczby,  program  uruchamia podproces,  który  wykonuje  procedurę  «`Sort`».  Rozmawia  z  nim  przy  pomocy  gniazda  domeny  uniksowej `unix(7)`,  które  tworzy  z  użyciem `socketpair(2)`,  czyli lokalnej dwukierunkowej metody  komunikacji międzyprocesowej. Jeśli proces sortujący otrzyma od rodzica pojedynczą liczbę, to natychmiast odsyła ją swojemu  rodzicowi  i  kończy  działanie.  Jeśli  dostanie  więcej  liczb,  to  startuje  odpowiednio  lewe  i  prawe dziecko, po czym za pomocą procedury «`SendElem`» przesyła im liczby do posortowania. Następnie wywołuje procedurę «`Merge`», która odbiera od potomków posortowane ciągi, scala je i wysyła do procesu nadrzędnego. Twoim zadaniem jest uzupełnienie procedury «`Sort`» tak by wystartowała procesy potomne i uruchomiłaprocedury «`SendElem`» i «`Merge`». Należy odpowiednio połączyć procesy z użyciem gniazd oraz zamknąć niepotrzebne gniazda w poszczególnych procesach. Posługując się rysunkiem wyjaśnij strukturę programu. Kiedy tworzysz podprocesy i gniazda? Kiedy zamykasz niepotrzebne gniazda? Jak wygląda przepływ danych? Skrypt «`gen-nums.py`» przyjmuje w linii poleceńn, czyli liczbę elementów do wygenerowania. Po uruchomieniu drukuje `n` na standardowe wyjście, po czym drukuje `n` losowych liczb całkowitych. Produkowane dane są w odpowiednim formacie do wprowadzenia do programu «`mergesort`».  
UWAGA! Wszystkie procesy muszą działać w stałej pamięci!*

Zrobione.
