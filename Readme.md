# Afinní šifra
## xmikul69
## KRY @ FIT VUT, 2023


## Implementace

Program je implementován v jazyce C. Implementace v souboru kry.c je rozdělena na několik hlavních částí, a to funkci main, funkce encrypt, decrypt a freq_analysis a pomocné funkce.

Na začátku je provedeno ošetření malých písmen, která jsou převedena na velká. Ostatní znaky, včetně mezer, číslic a čárek, jsou ponechány beze změny.


### Šifrování a dešifrování

Vstupy funkcí encrypt a decrypt jsou zašifrovaný řetězec, již alokovaný výstupní řetězec a čísla klíče 'a' a 'b'.
Šifrování je implementováno jako téměř přímý přepis formule ze zadání, které je provedeno nad každým velkým písmenem ze vstupu (ostatní znaky jsou ignorovány).

Při dešifrování je potřeba vypočítat hodnotu multiplikativní inverze, kterou poskytuje funkce 'mod_inv', převzatá z https://github.com/michel-leonard/C-MathSnip/blob/main/mod_inv.c. Kvůli možnosti podtečení při odečítání klíče b je celý výpočet "posunut" o 26 nahoru a nakonec vrácen zpět.


### Dešifrování bez znalosti klíčů

Logika nalezení nejpravděpodobnějšího klíče je implementována ve funkci 'freq_analysis'. Ta přijímá dva parametry, vstupní řetězec se zašifrovaným textem a alokovaný řetězec stejné délky, kam má být uložen dešifrovaný text.

Ve dvou zanořených smyčkách jsou postupně proiterovány všechny možné hodnoty 'a' a 'b' a pro každou je provedeno dešifrování, výpočet frekvencí a je spočítána funkce 'heuristic'. Ta vrací hodnotu v rozsahu 0 až UINT_MAX, která udává, nakolik se frekvenční charakteristika dešifrovného textu shoduje s českým jazykem. Výpočet probíhá tak, že četnost každého písmena v analyzovaném textu a v typickém českém textu jsou vynásobeny a součet těchto násobků udává vypočtenou heuristiku.

Hodnoty pro heuristickou funkci byly přejaty z https://algoritmy.net/article/40/Cetnost-znaku-CJ a upraveny tak, že četnosti verzí písmen s háčky a čárky byly sloučeny do daného písmena bez diakritiky.

V rámci funkce 'freq_analysis' jsou reprezentovány frekvence písmen polem 26 celých čísel. Jelikož funkce pracuje stále se stejným textem stejné délky, není potřeba přepočítávat výskyty na procentuální četnost, což by zpomalovalo výpočet Místo toho každá položka pole udává absolutní počet výskytů daného písmena v textu. Pro výpočet frekvencí slouží funkce 'compute_freqs'.

V rámci průchodu všech možných hodnot klíče si funkce uchovává nejlepší nalezenou hodnotu heuristiky spolu s odpovídajícími hodnotami klíče 'a' a 'b'.



## Kompilace a spuštění

Kompilaci lze provést příkazem 'make', který vytvoří binární spustitelný soubor 'kry'. Je použit kompilátor gcc s parametry -O3 a -fopenmp (knihovna OpenMP) pro zvýšení výkonnosti šifrování a dešifrování.

Skript testAll.sh lze použít pro testování funkcionality dešifrování bez znalosti klíčů. Rozdělí soubor ciphertext.csv po řádcích a následně nad každým spustí program kry.

