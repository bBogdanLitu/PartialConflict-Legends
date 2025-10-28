# Partial Conflict: Legends

### Description
<p align="justify">Command your All-Star army to victory in a turn-based strategy game heavily inspired by my passion for the 'Total War' series by Creative Assembly. Begin as a Contender, with one starting general from a (stat-wise) slightly underwhelming pool. Fight your neighbouring Warlords, loyal to the Emperor, and take over their Settlements to increase your power and influence, but make sure to setup your defence! Periodic attacks from all Warlords who can directly (?) reach one of your controlled Settlements are meant to test your grasp over the realm. Master their routines and exploit their weaknesses in order to progress towards the Emperor himself! If defeating his underlings wasn't a hard task, you might find taking him on to be a real challenge. Upgrade your generals, hire new ones and make sure your army is ready for any situation. Other bot Contenders will follow their path to glory (decided by me) until prevented from doing so, you don't have unlimited time. Defeat the Emperor and take control of his Settlements to claim victory.

<p align="justify">You don't like my take on the stats and/or names of the generals? Edit them yourself by modifying the configuration file, following the given format (generals.json). Give yourself an easier start by creating the most overpowered unit possible, make every enemy as strong as a dying snail or sabotage the other contenders by giving them absolutely nothing to work with (might fire back (?) ).

Why the name 'Partial Conflict'? Because it can't be compared to its 'inspiration' - [Total War: THREE KINGDOMS](https://www.totalwar.com/total-war/total-war-three-kingdoms) - [Gameplay (my own)](https://www.youtube.com/watch?v=EjgtD9rqzyU).

Why 'Legends'? Because you can field Sachil Onil to do your dirty work.

### Descriere
<p align="justify">Condu-ți armata de vedete către victorie în jocul meu de strategie pe ture (sună ciudat) inspirat din seria de jocuri „Total War” a celor de la Creative Assembly. Începe ca un Necunoscut, cu un general dintr-o listă slabă (din punct de vedere al puterii). Luptă-te cu Războinicii, cei loiali Împăratului, și preia controlul Orașelor pentru a-ți crește puterea și influența, dar asigură-te că le și aperi! Atacurile periodice de la toți Războinicii care au rută directă (?) spre unul dintre Orașele tale sunt menite să te pună la încercare. Învață-le obiceiurile și exploatează-le slăbiciunile (au) pentru a progresa către Împărat! Dacă, până acum, nu ți s-a părut mare lucru, s-ar putea ca acesta să reprezinte o încercare mai grea. Îmbunătățește-ți generalii și angajează alții, doar asigură-te că armata ta poate trece peste orice situație. Alți boți Necunoscuți își vor urma drumul spre victorie (codat de mine) până când nu mai pot face pași, așa că timpul nu este nelimitat. Doboară Împăratul și stăpânește-i orașele pentru a obține gloria supremă (e doar un joc, totuși).

<p align="justify">Dacă nu-ți convin stats-urile și numele puse de mine (nu te învinuiesc), poți să le schimbi personal prin editarea fișierului de configurație (generals.json). Fă-ți cel mai puternic general de start posibil, dă-le inamicilor cadavre ambulante pe post de unități și/sau sabotează-i pe ceilalți Necunoscuți prin „oferirea” unor melci umani (ar putea avea consecințe (?) ).

Numele se „justifică” pentru că nu îmi imaginez că acesta va fi măcar comparabil cu jocul [Total War: THREE KINGDOMS](https://www.totalwar.com/total-war/total-war-three-kingdoms) - [Gameplay (al meu)](https://www.youtube.com/watch?v=EjgtD9rqzyU).

Mai mult, poți pune la treabă niște generali „interesanți” (exemplu în engleză).

### Important!

Aveți voie cu cod generat de modele de limbaj la care nu ați contribuit semnificativ doar dacă documentați riguros acest proces.
Codul generat pus "ca să fie"/pe care nu îl înțelegeți se punctează doar pentru puncte bonus, doar în contextul
în care oferă funcționalități ajutătoare și doar dacă are sens.

Codul din proiect trebuie să poată fi ușor de înțeles și de modificat de către altcineva. Pentru detalii, veniți la ore.

O cerință nu se consideră îndeplinită dacă este realizată doar prin cod generat.

- **Fără cod de umplutură/fără sens!**
- **Fără copy-paste!**
- **Fără variabile globale!**
- **Fără atribute publice!**
- **Pentru T2 și T3, fără date în cod!** Datele vor fi citite din fișier, aveți exemple destule.
- **Obligatoriu** fișiere cu date mai multe din care să citiți, obligatoriu cu biblioteci externe: fișiere (local sau server) sau baze de date
- obligatoriu (TBD) să integrați cel puțin două biblioteci externe pe lângă cele pentru stocare

### Tema 0

- [X] Nume proiect (poate fi schimbat ulterior)
- [X] Scurtă descriere a temei alese, ce v-ați propus să implementați

## Tema 1

#### Cerințe

- [X] definirea a minim **3-4 clase** folosind compunere cu clasele definite de voi; moștenirile nu se iau în
  considerare aici
- [X] constructori de inițializare cu parametri pentru fiecare clasă
- [X] pentru o aceeași (singură) clasă: constructor de copiere, `operator=` de copiere, destructor
<!-- - [ ] pentru o altă clasă: constructor de mutare, `operator=` de mutare, destructor -->
<!-- - [ ] pentru o altă clasă: toate cele 5 funcții membru speciale -->

- [X] `operator<<` pentru **toate** clasele pentru afișare (`std::ostream`) folosind compunere de apeluri cu
  `operator<<`
- [X] cât mai multe `const` (unde este cazul) și funcții `private`
- [X] implementarea a minim 3 funcții membru publice pentru funcționalități netriviale specifice temei alese, dintre
  care cel puțin 1-2 funcții mai complexe
  - nu doar citiri/afișări sau adăugat/șters elemente într-un/dintr-un vector
- [X] scenariu de utilizare **cu sens** a claselor definite:
  - crearea de obiecte și apelarea tuturor funcțiilor membru publice în main
  - vor fi adăugate în fișierul `tastatura.txt` DOAR exemple de date de intrare de la tastatură (dacă există); dacă aveți nevoie de date din fișiere, creați alte fișiere separat
- [X] minim 50-55% din codul propriu să fie C++, `.gitattributes` configurat corect
- [X] tag de `git`: de exemplu `v0.1`
- [X] serviciu de integrare continuă (CI) cu **toate bifele**; exemplu: GitHub Actions
- [ ] code review #1 2 proiecte

#### Chestii specifice

- [X] generals.json, citire, distribuire;
- [X] settlements.json, citire, distribuire (exemplu minim);
- [X] verificare input utilizator pentru a preveni unele probleme (mai trebuie investigat);
- [X] alegere general de start, intrare intr-un tutorial simplut;
- [X] prima versiune a sistemului si logicii de lupta dintre armate;
- [ ] output intuitiv si usor de urmat (ftxui / macar text colorat si decorat).

#### Posibile idei pentru Tema 2

- Schimbare logica Settlements.h / ControlPoint.h pentru a permite mersul inainte si inapoi, nu doar inainte;
- Extindere tutorial la o zona mai mare, nu neaparat toata harta, dar jucabila (putem misca, modifica armata);
- Implementare cost pentru fiecare general (calculat in mod dinamic prin comparatie cu ce mai exista prin minunatul
  generals.json, nu poate fi editat direct de jucator pentru ca nu vreau eu, d-aia);
- LocalLeader - pentru mostenire (e un fel de General), care sta intr-un settlement si contribuie la apararea sa, plus
  alte beneficii (?);
- Captain - tot pentru mostenire. Este un general mai slab, usor accesibil dpdv al costului, fara nume foarte smecher,
  care poate ocupa un loc dintr-o armata;
- Emperor - oare pentru ce? Va fi marele *REVEAL* al imparatului din acest joc. Probabil va fi pus la intimidare
  printr-un settlement aparent inofensiv, parte din povestea jocului (insane lore);
- Librarie pentru sunete (vor contribui la *aura* imparatului suprem).

## Tema 2

#### Cerințe
- [ ] separarea codului din clase în `.h` (sau `.hpp`) și `.cpp`
- [ ] moșteniri:
  - minim o clasă de bază și **3 clase derivate** din aceeași ierarhie
  - ierarhia trebuie să fie cu bază proprie, nu derivată dintr-o clasă predefinită
  - [ ] funcții virtuale (pure) apelate prin pointeri de bază din clasa care conține atributul de tip pointer de bază
    - minim o funcție virtuală va fi **specifică temei** (i.e. nu simple citiri/afișări sau preluate din biblioteci i.e. draw/update/render)
    - constructori virtuali (clone): sunt necesari, dar nu se consideră funcții specifice temei
    - afișare virtuală, interfață non-virtuală
  - [ ] apelarea constructorului din clasa de bază din constructori din derivate
  - [ ] clasă cu atribut de tip pointer la o clasă de bază cu derivate; aici apelați funcțiile virtuale prin pointer de bază, eventual prin interfața non-virtuală din bază
    - [ ] suprascris cc/op= pentru copieri/atribuiri corecte, copy and swap
    - [ ] `dynamic_cast`/`std::dynamic_pointer_cast` pentru downcast cu sens
    - [ ] smart pointers (recomandat, opțional)
- [ ] excepții
  - [ ] ierarhie proprie cu baza `std::exception` sau derivată din `std::exception`; minim **3** clase pentru erori specifice distincte
    - clasele de excepții trebuie să trateze categorii de erori distincte (exemplu de erori echivalente: citire fișiere cu diverse extensii)
  - [ ] utilizare cu sens: de exemplu, `throw` în constructor (sau funcție care întoarce un obiect), `try`/`catch` în `main`
  - această ierarhie va fi complet independentă de ierarhia cu funcții virtuale
- [ ] funcții și atribute `static`
- [ ] STL
- [ ] cât mai multe `const`
- [ ] funcții *de nivel înalt*, de eliminat cât mai mulți getters/setters/funcții low-level
- [ ] minim 75-80% din codul propriu să fie C++
- [ ] la sfârșit: commit separat cu adăugarea unei noi clase derivate fără a modifica restul codului, **pe lângă cele 3 derivate deja adăugate** din aceeași ierarhie
  - noua derivată nu poate fi una existentă care a fost ștearsă și adăugată din nou
  - noua derivată va fi integrată în codul existent (adică va fi folosită, nu adăugată doar ca să fie)
- [ ] tag de `git` pe commit cu **toate bifele**: de exemplu `v0.2`
- [ ] code review #2 2 proiecte

## Tema 3

#### Cerințe
- [ ] 2 șabloane de proiectare (design patterns)
- [ ] o clasă șablon cu sens; minim **2 instanțieri**
  - [ ] preferabil și o funcție șablon (template) cu sens; minim 2 instanțieri
- [ ] minim 85% din codul propriu să fie C++
<!-- - [ ] o specializare pe funcție/clasă șablon -->
- [ ] tag de `git` pe commit cu **toate bifele**: de exemplu `v0.3` sau `v1.0`
- [ ] code review #3 2 proiecte

## Instrucțiuni de compilare

Proiectul este configurat cu CMake.

Instrucțiuni pentru terminal:

1. Pasul de configurare
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
# sau ./scripts/cmake.sh configure
```

Sau pe Windows cu GCC folosind Git Bash:
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G Ninja
# sau ./scripts/cmake.sh configure -g Ninja
```

Pentru a configura cu ASan, avem opțiunea `-DUSE_ASAN=ON` (nu merge pe Windows cu GCC):
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON
# sau ./scripts/cmake.sh configure -e "-DUSE_ASAN=ON"
```


La acest pas putem cere să generăm fișiere de proiect pentru diverse medii de lucru.


2. Pasul de compilare
```sh
cmake --build build --config Debug --parallel 6
# sau ./scripts/cmake.sh build
```

Cu opțiunea `parallel` specificăm numărul de fișiere compilate în paralel.


3. Pasul de instalare (opțional)
```sh
cmake --install build --config Debug --prefix install_dir
# sau ./scripts/cmake.sh install
```

Vezi și [`scripts/cmake.sh`](scripts/cmake.sh).

Observație: folderele `build/` și `install_dir/` sunt adăugate în fișierul `.gitignore` deoarece
conțin fișiere generate și nu ne ajută să le versionăm.


## Instrucțiuni pentru a rula executabilul

Există mai multe variante:

1. Din directorul de build (implicit `build`). Executabilul se află la locația `./build/oop` după ce a fost rulat pasul de compilare al proiectului (`./scripts/cmake.sh build` - pasul 2 de mai sus).

```sh
./build/oop
```

2. Din directorul `install_dir`. Executabilul se află la locația `./install_dir/bin/oop` după ce a fost rulat pasul de instalare (`./scripts/cmake.sh install` - pasul 3 de mai sus).

```sh
./install_dir/bin/oop
```

3. Rularea programului folosind Valgrind se poate face executând script-ul `./scripts/run_valgrind.sh` din rădăcina proiectului. Pe Windows acest script se poate rula folosind WSL (Windows Subsystem for Linux). Valgrind se poate rula în modul interactiv folosind: `RUN_INTERACTIVE=true ./scripts/run_valgrind.sh`

Implicit, nu se rulează interactiv, iar datele pentru `std::cin` sunt preluate din fișierul `tastatura.txt`.

```sh
RUN_INTERACTIVE=true ./scripts/run_valgrind.sh
# sau
./scripts/run_valgrind.sh
```

4. Pentru a rula executabilul folosind ASan, este nevoie ca la pasul de configurare (vezi mai sus) să fie activat acest sanitizer. Ar trebui să meargă pe macOS și Linux. Pentru Windows, ar merge doar cu MSVC (nerecomandat).

Comanda este aceeași ca la pasul 1 sau 2. Nu merge combinat cu Valgrind.

```sh
./build/oop
# sau
./install_dir/bin/oop
```
## External Libraries

- [Nlohmann Json](https://github.com/nlohmann/json) for reading user-configurable data. Used without any modifications;
- [FTXUI](https://github.com/ArthurSonzogni/FTXUI) for terminal beautification.

## Resurse

- [Nlohmann Json](https://github.com/nlohmann/json) pentru citirea unor date ce pot fi schimbate după preferințele fiecăruia. Nu am efectuat nicio modificare proprie;
- [FTXUI](https://github.com/ArthurSonzogni/FTXUI) pentru oportunitati de infrumusetare a terminalului.
- adăugați trimiteri **detaliate** către resursele externe care v-au ajutat sau pe care le-ați folosit
