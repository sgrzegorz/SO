# Zadania - zestaw 2
## Zadanie 1. Porównanie wydajności systemowych i bibliotecznych funkcji we/wy 

Program udostępnia operacje:

generate - tworzenie pliku z rekordami wypełnionego wygenerowaną losową zawartością (można wykorzystać wirtualny generator /dev/random lub w wersji uproszczonej funkcję rand)

sort - sortuje rekordy w pliku używając sortowania przez proste wstawianie. Kluczem do sortowania niech będzie wartość pierwszego bajtu rekordu (interpretowanego jako liczba bez znaku - unsigned char) 

copy - kopiuje plik1 do pliku2. Kopiowanie powinno odbywać się za pomocą bufora o zadanej wielkości rekordu.

## Zadanie 2. Operacje na strukturze katalogów. 

Program wyszukujący w drzewie katalogu (ścieżka do katalogu jest pierwszym argumentem programu), 
w zależności od wartości drugiego argumentu ('<', '>','=') , pliki zwykłe z datą modyfikacji wcześniejszą,
późniejszą lub równą dacie podanej jako trzeci argument programu.
