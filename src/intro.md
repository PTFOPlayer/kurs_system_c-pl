# Wprowadzenie

## Dla kogo jest ten kurs?

Jest on przeznaczony głównie dla osób które chcą się pobawić, nauczyć programowania niskopoziomowego, rozwinąć sœój zasób wiedzy na temat bardzo podstawowych procesów zachodzących w komputerze lub po prostu chcących mieć jeden projekt więcej na githubie który jest czymś więcej niż kolejną wersją listy `todo`. Kurs jest ciężki, może wymagać minimalnej wiedzy na temat działania procesora, języka ASM oraz języka C. 

Będę się starał w tym kursie jak najlepiej wyjaśnić wszystkie pojęcia, ale mogę co nieco pominąć, w związku z tym zapraszam do tworzenia `Issue` i `PR` na [githubie](https://github.com/PTFOPlayer/kurs_system_c-pl/).

Kurs definitywnie nie jest dla osób które nie znają podstaw C. Większośc kodu obecnego w tym kursie jest kodem niskopoziomowym który działa bezposrednio na surowych adresach i rejestrach, są w nim wstawki z ASM oraz nie posiadamy żadnego stopnia abstrakcji poza tym zapewnianym przez minimalną specyfikację języka C99. Znajduje się tutaj równierz dużo pojęć dotyczących struktury działania procesora i pamięci które mogą być ciężkie do zrozumienia dla osób które nie miały doczynienia z programowaniem niskopoziomowym.

## Źródła wiedzy
Warto znać dobre źródła wiedzy zabierając się za tak ciężki temat jak pisanie surowego systemmu operacyjnego.

### wiki OSdev - [wiki.osdev.org](https://wiki.osdev.org)
Jest to największe źródło wiedzy jeśli chodzi o tematykę pisania amatorskiego systemu operacyjnego

### Dokumentacja Intela - [intel-sdm](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
Nie mam bladego pojęcia jakim cudem dałem radę to przeczytać ale znajomość tego jak działają procesory i co jest na nich możliwe bardzo pomaga w projektach z działu embedded.

### Dokumentacja AMD - [amd/developer](https://www.amd.com/en/developer.html)
Pomimo tego że mniej popularna od intelowskiej, jest ona równie dobrym źródłem wiedzy na temat procesorów x86.

### Philipp Oppermann, Blog, Edycja 1 - [phil-opp](https://os.phil-opp.com/edition-1/)
To jest akurat kurs pisania systemu operacyjnego z użyciem języka RUST. Nastomiast wiele rzeczy z paru pierwszych działów jego bloga i tego kursu jest bardzo podobnych.