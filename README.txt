===============================================================================
||				Mapper-Reducer				     ||
===============================================================================

Bariere si Mutex

Din notiunile de APD, am folosit o bariera in doua locuri si mutex in doua
locuri. Una dintre bariere se afla la finalul threadurilor de mapare, in timp
ce a doua bariera este pusa la inceputul functiei de reduce pentru a se asigura
ca nu incep threadurile de reducere pana se termina cele de mapare.

De asemenea, am inconjurat cu un mutex adaugarea de noi elemente in mapa pentru
a ma asigura ca nu voi pierde niciun numar. Pe langa mapa, am folosit mutex si
atunci cand schimb valoarea "globala" a numarului fisierului in care ma aflu,
pentru a nu avea mai multe threaduri pe acelasi fisier.

Structuri

Pentru a pastra toate valorile "globale" de care am nevoie, am initializat doua
structuri. In vectorul "arguments" de tip "ids" pastrez numarul threadului
curent si un pointer la structura "Args", unde am toate variabilele importante,
inclusiv un pointer catre mapa care contine toate puterile perfecte.

Algoritm de ridicare la putere / radical

Am folosit aproximarea lui Newton pentru a gasi radacina unui numar in mod
eficient, utilizand si functia fast_power care calculeaza puterea mai rapid
unui numar.

"Curatare" de mapa

Pentru a curata mapa, ma folosesc de doua functii din biblioteca map, "sort"
pentru a ordona fiecare vector din map, si map[id].erase pentru a sterge
dublurile mai usor.
