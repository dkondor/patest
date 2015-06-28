/*
 * idlist.h -- id-k tárolása, gyors keresés
 * 
 * Copyright 2013 Kondor Dániel <dani@thinkpad-r9r>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#ifndef IDLIST_H
#define IDLIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct ids_t {
	unsigned int N; //id-k száma
	unsigned int* ids; //id-k tárolása
	unsigned int* outlinks; //kimenő linkek száma (txedge-ben szereplő tranzakciók után egyel növeljük)
	unsigned int* inlinks; //bejövő linkek száma
	unsigned int* outtx; //kimenő tranzakciók száma (txin-beli tranzakciók szerint)
	unsigned int* intx; //bejövő tranzakciók száma (txout-beli tranzakciók szerint)
	int64_t* balance; //egyenleg -- megengedünk negatív egyenlegeket is: a tranzakciók nem szigorúan sorban vannak, így
		//viszont azt nem rontjuk el, ha egy cím egyenlege egzaktul 0 lesz (elköltötte az összes rajta levő pénzt)
} idlist;

//id-k felszabadítása
void ids_free(idlist* id);

//id-k beolvasása a megadott fájlból (maximum N darab -- N nem lehet 0, ezt használjuk a tömbök lefoglalásához)
idlist* ids_read(FILE* f, unsigned int N);

//id megkeresése: eredmény a tömbbeli hely, vagy l->N, ha nem találtuk
unsigned int ids_find(const idlist* l, unsigned int id);

static inline int ids_find2(const idlist* l, unsigned int id) {
	if(id && id <= l->N) if(l->ids[id-1] == id) return id-1;
	return ids_find(l,id);
}

#endif

