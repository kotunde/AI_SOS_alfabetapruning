# SOS game

### Feladat leírása
- Kezdetben egy minimum 3x3-as tábla (a programban ```#define N 5```)
- A játékosok körönként vagy egy "s"-t, vagy egy "o"-t tesznek bármely szabad cellába
- A cél mindkét játékos számára az s-o-s szekvencia létrehozása(vízszintesen, függőlegesen, vagy átlósan), ebből minél többet
- Ha az egyik játékosnak sikerül egy s-o-s szekvenciát létrehozni, újból ő következik karaktert tenni
- Számoljuk, hogy ki hány s-o-s szekvenciát hozott létre
- A játéknak vége, ha a tábla betelik


### Heurisztikus kiértékelő függvény

A heurisztika függvény ```heuristics()``` megkapja paraméterként a tábla jelenlegi helyzetéből elérhető i-dik állapotot. Tehát az állapot tábla egy karakterben különbözik a jelenlegi táblától, az az egy karakter a gép lehetséges lépése. 
Itt három eset van a gépre nézve:
- a ```search_sos``` függvény 0/1-et térít vissza annak függvényében, hogy ebben az állapotban a jelenlegi táblához képest lett-e kihozva s-o-s szekvencia, értéke 1 pont

- a ```search_sxs``` függvény 0/1-et térít vissza annak függvényében, hogy ebben az állapotban a jelenlegi táblához képest lett-e kihozva s-x-s szekvencia, ami következő lépésben az ellenfélnek kedvezne,értéke -1 pont

- a ```search_so``` függvény 0/1-et térít vissza annak függvényében, hogy ebben az állapotban a jelenlegi táblához képest lett-e kihozva s-x-s szekvencia, ami következő lépésben az ellenfélnek kedvezne,értéke -1 pont

- az ezektől külőnböző eseteket semlegesnek tekintjük, értéke 0

