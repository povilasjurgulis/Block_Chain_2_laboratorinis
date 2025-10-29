# Povilo Jurgulio Blokų Grandinių Technologijų 2 laboratorinis darbas (v0.1)

Trumpas aprašymas

Šis laboratorinis darbas yra supaprastintos blokų grandinės realizacija. Naudojau C++ kalbą. ChatGPT padėjo sugalvoti struktūrą ir ko reikia programoje; pats žiūrėjau labai daug Youtube video ir bandžiau suprasti kaip veikia Bitcoin ir tiesiog blockchain, ir tai man padėjo pačiam kažkiek realizuoti funkcijų, bet dažnai, kai darydavau funkcijas ar metodus, reikėdavo paklausti chatgpt, kad patvarkytų, nes nebūdavo pilnai gerai, kai pats dariau. Dar normaliai idėjų gavau tyrinėdamas Google.
Naudojau savo hash_function iš praeito laboratorinio darbo ir nieko nekeičiau.

## Mano programa atlieka šiuos veiksmus:

- Generuoja nustatytą skaičių vartotojų su atsitiktiniais balansais.
- Sugeneruoja daug (pvz., 10 000) transakcijų tarp šių vartotojų.
- Grupuodama transakcijas formuoja blokus (iki nustatyto transakcijų skaičiaus viename bloke).
- Atlieka „proof-of-work“ kiekvienam blokui, randa nonce, kad blokų hashas atitiktų sunkumą.
- Atnaujina vartotojų balansus pagal įtrauktas transakcijas ir atspausdina santrauką (mempool dydis, atnaujinti balanso įrašai ir panašiai).

## Files santrauka:

- `all_classes.h` / `all_classes.cpp` — sudėtinės klasių deklaracijos ir įgyvendinimai (Transaction, User, Generator, MerkleTree ir kt.).
- `Block.h` / `Block.cpp`, `Blockchain.h` / `Blockchain.cpp` — blokų ir grandinės logika bei "mining" eiga.
- `hash_function.cpp` — paprasta maišos funkcija naudojama ID/hash generavimui (iš praeito laboratorinio darbo).
- `main.cpp` —  pagrindinis file, kuris paleidžia generavimą, kalnakasybą ir ataskaitas.

### Svarbu:
 "Mining" gali užtrukti priklausomai nuo nustatyto sunkumo (difficulty). Testams galite laikinai sumažinti sunkumą ar sumažinti transakcijų skaičių greitam paleidimui.

