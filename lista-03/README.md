# Systemy operacyjne – lista 3 2019-10-29

Należy przygotować się do zajęć czytając następujące rozdziały książek:

* Tanenbaum (wydanie czwarte): 10.3, 11.4 • Stallings (wydanie dziewiąte): 4.6
* APUE (wydanie trzecie): 8, 10

UWAGA! W trakcie prezentacji należy być gotowym do zdefiniowania pojęć oznaczonych wytłuszczoną czcionką.
Podręcznikiem do zadań praktycznych jest „Advanced Programming in the UNIX Environment” (w skrócie APUE). Proszę przede wszystkim korzystać z podręcznika systemowego. Jeśli jego treść w Linuksie nie jest wystarczająco klarowna, to warto spojrzeć do odpowiednika z BSD – najlepiej z użyciem strony mdoc.su.
Zadania wymagające użycia rzutnika, oznaczenie (P), należy starannie przygotować w domu – najlepiej w postaci pliku tekstowego z listą poleceń do wykonania i komentarzami. Każde zadanie należy mieć właściwie przygotowane do prezentacji przed zajęciami. W przypadku zbędnego przeciągania czasu odpowiedzi ze względu na problemy techniczne prowadzący ma prawo skreślić zadanie i postawić jeden punkt ujemny.
UWAGA! Każdy student musi przygotować skrypt, który z użyciem programu xrandr1 ustawi rozdzielczość ekranu wbudowanego na 1024×768 i sklonuje go na zewnętrzne złącze VGA lub HDMI. Dla programu terminala należy wybrać dużą czcionkę (około 32 wierszy w trybie pełnoekranowym) i kontrastowe kolory. Starannie przetestuj swoją konfigurację przed zajęciami!

## Zadanie 1

Na podstawie rysunku 4.15 z §4.6 przedstaw stany procesu w systemie Linux. Podaj akcje albo zdarzenia wyzwalające zmianę stanów. Które przejścia mogą być rezultatem działań podejmowanych przez: jądro systemu operacyjnego, kod sterowników, proces użytkownika? Wyjaśnij różnice między snem przerywalnym i nieprzerywalnym. Czy proces może zablokować lub zignorować sygnał «`SIGKILL`»?

## Zadanie 2

Wyjaśnij różnice w tworzeniu procesów w systemie Linux (§10.3.3) i WinNT (§11.4.3). Naszkicuj przebieg akcji podejmowanych przez jądro w trakcie obsługi funkcji `fork(2)` i `execve(2)`. Wyjaśnij jak system uniksowy optymalizuje klonowanie procesów z użyciem kopiowania przy zapisie?

## Zadanie 3 (bonus)

W artykule [A fork() in the road](https://www.microsoft.com/en-us/research/publication/a-fork-in-the-road/) skrytykowano wywołanie `fork(2)`. Na podstawie sekcji 4 publikacji przedstaw pobieżnie argumentację autorów przeciwko «`fork`». Następnie opowiedz jak `vfork(2)` i posix_`spawn(3)` pomagają zniwelować niektóre z wymienionych wad.

## Zadanie 4 (P)

Uruchom program «`xeyes`» po czym użyj na nim polecenia «`kill`», «`pkill`» i «`xkill`». Który sygnał jest wysyłany domyślnie? Przy pomocy kombinacji klawiszy «CTRL+Z» wyślij «`xeyes`» sygnał «`SIGSTOP`», a następnie wznów jego wykonanie. Przeprowadź inspekcję pliku «/proc/pid/status» i wyświetl maskę sygnałów oczekujących na dostarczenie. Pokaż jak będzie się zmieniać, gdy będziemy wysyłać wstrzymanemu procesowi kolejno: «`SIGUSR1`», «`SIGUSR2`», «`SIGHUP`» i «`SIGINT`». Co opisują pozostałe pola pliku «`status`» dotyczące sygnałów? Który sygnał zostanie dostarczony jako pierwszy po wybudzeniu procesu?

Ściągnij ze strony przedmiotu archiwum «`so19_lista_3.tar.gz`», następnie rozpakuj i zapoznaj się z dostarczonymi plikami. UWAGA! Można modyfikować tylko te fragmenty programów, które zostały oznaczone w komentarzu napisem «`TODO`».

## Zadanie 5 (P)

Uzupełnij program [«`reaper.c`»](so19_lista_3/reaper.c) prezentujący powstawanie sierot. Proces główny przyjmuje rolę żniwiarza (ang. reaper) przy użyciu `prctl(2)`. Przy pomocy procedury «`spawn`» utwórz kolejno procesy syna i wnuka. Następnie osieroć wnuka kończąc działanie syna. Uruchom podproces wywołujący polecenie «`ps`», aby wskazać kto przygarnął sierotę – przykład poniżej (zwróć uwagę na numery grup procesów):

```
1 PID PPID PGRP STAT CMD
2 24886 24643 24886 S+ ./reaper (main)
3 24888 24886 24887 S ./reaper (grandchild)
4 24889 24886 24886 R+ /usr/bin/ps -o pid,ppid,pgrp,stat,cmd
```
Po udanym eksperymencie należy zabić wnuka sygnałem «`SIGINT`», a następnie po nim posprzątać drukując jego kod wyjścia. Zauważ, że proces główny nie zna numeru pid wnuka! W rozwiązaniu należy wykorzystać `setpgid(2)`, `pause(2)`, `waitpid(2)` i `kill(2)`.
UWAGA! Użycie funkcji `sleep(3)` lub podobnych do właściwego uszeregowania procesów jest zabronione!

## Zadanie 6 (P)

Uzupełnij program [«`cycle.c`»](so19_lista_3/cycle.c), w którym procesy grają w piłkę przy pomocy sygnału «`SIGUSR1`». Proces główny tworzy n dzieci. Każde z nich czeka na piłkę, a po jej odebraniu podaje ją do swojego starszego brata. Zauważ, że najstarszy brat nie zna swojego najmłodszego rodzeństwa, ale zna je ojciec – więc należy go wciągnąć do gry! Niech tata rozpocznie grę rzucając piłkę do najmłodszego dziecka. Kiedy znudzi Ci się obserwowanie procesów grających w piłkę możesz nacisnąć «CTRL+C» co wyśle «`SIGINT`» do całej rodziny. W rozwiązaniu należy wykorzystać `setprocmask(2)`, `sigsuspend(2)` i `kill(2)`.
UWAGA! Użycie funkcji `sleep(3)` lub podobnych do właściwego uszeregowania procesów jest zabronione!
Zadanie 7 (2, P). Uzupełnij program «`demand`» o procedurę obsługi sygnału «`SIGSEGV`». Program ma
za zadanie demonstrować przechwytywanie błędów stron, których nie było w stanie obsłużyć jądro SO.
Obsługujemy zakres adresów od «`VA_START`» do «`VA_END`». Pod losowo wybrane wirtualne strony z podanego przedziału zostanie podpięta pamięć wirtualna w trybie tylko do odczytu. Następnie program wygeneruje do zadanego przedziału adresów zapisy, które zakończą się naruszeniem ochrony pamięci.
Po przechwyceniu sygnału «`SIGSEGV`», korzystając z procedur «`mmap_page`» i «`mprotect_page`» odpowiednio zmapuj brakującą stronę (błąd «`SEGV_MAPERR`») i odblokuj zapis do strony (błąd «`SEGV_ACCERR`»). Dostęp do adresów spoza ustalonego zakresu powinien skutkować zakończeniem programu. Należy wtedy ustalić właściwy kod wyjścia tak, jakby proces został zabity sygnałem!

```
1 ...
2 Fault at 0x55cb50d54389 accessing 0x10003fc0! Make page at 0x10003000 writable. 3 Fault at 0x55cb50d54389 accessing 0x10007bb0! Map missing page at 0x10007000.
4 ...
5 Fault at 0x55cb50d5439c accessing 0x10010000! Address not mapped - terminating!
```

W procedurze obsługi sygnału można używać tylko procedur wielobieżnych (ang. reentrant) – sprawdź w podręczniku ich listę. Możesz wykorzystać procedurę «`safe_printf`», będącą okrojoną wersją «`printf`». Czemu można ją bezpiecznie wywołać w wnętrza «`sigsegv_handler`»?
Adres powodujący błąd strony i rodzaj błędu znajdziesz w argumencie «`sigsegv_handler`» o typie «`siginfo_t`», który opisano w podręczniku `sigaction(2)`. Wskaźnik instrukcji, która spowodowała błąd strony, można przeczytać ze struktury przechowującej kontekst procesora «`uc->uc_mcontext`». Odpowiednie definicje znajdują się w pliku nagłówkowym «`/usr/include/x86_64-linux-gnu/sys/ucontext.h`».
