/*
 * idlist.c -- id-k tárolása, gyors keresés
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

#include "idlist.h"
#include "edges.h"

static inline void swap(unsigned int* s, unsigned int i, unsigned int j) {
	unsigned int tmp = s[i];
	s[i] = s[j];
	s[j] = tmp;
}

static inline int cmp(unsigned int* s, unsigned int i, unsigned int j) {
	if(s[i] < s[j]) return -1;
	if(s[i] > s[j]) return 1;
	return 0;
}

static void quicksort(unsigned int* d, unsigned int s, unsigned int e) {
	if(s >= e) return;
	if(e-s == 1) return;
	if(e-s == 2) {
		if(cmp(d,s,s+1) > 0) swap(d,s,s+1);
		return;
	}
	
	unsigned int p = s + (e-s)/2;
	unsigned int i;
	swap(d,p,e-1);
	p = s;
	for(i=s;i<e-1;i++) {
		int a = cmp(d,e-1,i);
		if(a == 1 || (a == 0 && p<s+(e-s)/2)) {
			if(i>p) swap(d,i,p);
			p++;
		}
	}
	if(p < e-1) swap(d,p,e-1);
	quicksort(d,s,p);
	quicksort(d,p+1,e);
}


void ids_free(idlist* id) {
	if(id) {
		if(id->ids) free(id->ids);
		if(id->balance) free(id->balance);
		if(id->outlinks) free(id->outlinks);
		if(id->inlinks) free(id->inlinks);
		if(id->outtx) free(id->outtx);
		if(id->intx) free(id->intx);
		free(id);
	}
}

//id-k beolvasása a megadott fájlból (maximum N darab)
idlist* ids_read(FILE* f, unsigned int N) {
	if(! (f && N) ) return 0;
	idlist* ret = (idlist*)malloc(sizeof(idlist));
	if(!ret) return 0;

	
	ret->ids = 0;
	ret->balance = 0;
	ret->outlinks = 0;
	ret->inlinks = 0;
	ret->outtx = 0;
	ret->intx = 0;
	ret->N = 0;
	
	ret->ids = (unsigned int*)malloc(sizeof(unsigned int)*N);
	ret->outlinks = (unsigned int*)calloc(N,sizeof(unsigned int));
	ret->inlinks = (unsigned int*)calloc(N,sizeof(unsigned int));
	ret->outtx = (unsigned int*)calloc(N,sizeof(unsigned int));
	ret->intx = (unsigned int*)calloc(N,sizeof(unsigned int));
	ret->balance = (int64_t*)calloc(N,sizeof(uint64_t));
	
	if( ! (ret->ids && ret->outlinks && ret->inlinks && ret->outtx && ret->intx && ret->balance) ) {
		ids_free(ret);
		fclose(f);
		return 0;
	}
	
	//N darab számot próbálunk meg beolvasni, minden sorban egy szám
	unsigned int i = 0;
	unsigned int sor = 0;
	unsigned int hiba = 0;
	const unsigned int mh = 20;
	while(1) {
		sor++;
		int a = getc(f);
		while(a == ' ' || a == '\t') {
			a = getc(f);
		}
		if(a == EOF) break;
		if(a == '\n') continue;
		if(a == '#') goto ir_endl;
		
		ungetc(a,f);
		a = fscanf(f,"%u",ret->ids + i);
		if(a == 1) i++;
		else {
			if(hiba < mh) fprintf(stderr,"ids_read(): hibás adatok a bemenet %u. sorában!\n",sor);
			hiba++;
			if(hiba == mh) fprintf(stderr,"\t(a további hibákat nem írom ki)\n");
		}
		if(i == N) break;
		
ir_endl:
		do {
			a = getc(f);
		} while( ! (a == '\n' || a == EOF) );
		if(a == EOF) break;
	}
	
	if(hiba) fprintf(stderr,"ids_read(): összesen %u hibás sor\n",hiba);
	if(i == 0) {
		fprintf(stderr,"ids_read(): Nem találtam érvényes adatokat a bemeneten!\n");
		ids_free(ret);
		return 0;
	}
	N = i;
	ret->N = i;
	quicksort(ret->ids,0,N);
	fprintf(stderr,"ids_read(): %u adatot olvastam be\n",N);
	return ret;
}

//id megkeresése: eredmény a tömbbeli hely, vagy l->N, ha nem találtuk
unsigned int ids_find(const idlist* l, unsigned int id) {
	unsigned int i = 0; //tipp a helyre
	//~ if(id) i = id-1; //tipp: ID-k sorban, nincs kihagyva semmi
	//~ int di = 0; //előző lépésbeli elmozdulás
	unsigned int di = l->N; //elmozdulás mértéke
	
	while(1) {
		if(l->ids[i] == id) return i; //megtaláltuk
		if(l->ids[i] > id) { //i-t csökkenteni kell
			if(i==0) return l->N; //már a legelején vagyunk, nem lehet tovább csökkenteni
			di /= 2;
			if(di == 0) di = 1;
			if(di == 1) {
				if(l->ids[i-1] < id) return l->N; //i-1 és i között van, nem találtuk
				i--;
			}
			else {
				if(di > i) i = 0;
				else i -= di;
			}
		} // l->ids[i] > id -- i-t csökkenteni kell
		else { //i-t növelni kell
			if(i==l->N-1) return l->N; //a legvégén vagyunk
			di /= 2;
			if(di == 0) di = 1;
			if(di == 1) {
				if(l->ids[i+1] > id) return l->N; //i és i+1 között van, nem találtuk
				i++;
			}
			else {
				i += di;
				if(i >= l->N) i = l->N-1;
			}
		}
		
	} //while(1)
	return l->N; //ide elvileg nem juthatunk el, nincs break a ciklusban, csak return
}

/*
static inline void swap(uint32_t* d, unsigned int i, unsigned int j) {
	uint32_t tmp = d[i];
	d[i] = d[j];
	d[j] = tmp;
}

static inline int cmp(uint32_t* d, unsigned int i, unsigned int j) {
	if(d[i] < d[j]) return 1;
	if(d[i] > d[j]) return -1;
	return 0;
}

static void quicksort(uint32_t* d, unsigned int s, unsigned int e) {
	if(s >= e) return;
	if(s+1 == e) return;
	if(s+2 == e) {
		if(cmp(d,s,s+1) < 0) swap(d,s,s+1);
		return;
	}
	
	unsigned int p = s + (e-s)/2;
	swap(d,p,e-1);
	unsigned int i;
	p = s;
	for(i=s;i<e-1;i++) {
		int a = cmp(d,i,e-1);
		if(a > 0 || (a == 0 && i < s+(e-s)/2)) {
			if(i!=p) swap(d,i,p);
			p++;
		}
	}
	if(p < e-1) swap(d,p,e-1);
	
	quicksort(d,s,p);
	quicksort(d,p+1,e);
}*/

//ID-k generálása N db él alapján
idlist* ids_gen(const edge* e, unsigned int N) {
	if(!(N && e)) return 0;
	idlist* il = (idlist*)malloc(sizeof(idlist));
	if(!il) {
		fprintf(stderr,"ids_gen(): nem sikerült memóriát lefoglalni!\n");
		return 0;
	}
	il->N = 0; //id-k száma
	il->ids = 0; //id-k tárolása
	il->outlinks = 0; //kimenő linkek száma (txedge-ben szereplő tranzakciók után egyel növeljük)
	il->inlinks = 0; //bejövő linkek száma
	il->outtx = 0; //kimenő tranzakciók száma (txin-beli tranzakciók szerint)
	il->intx = 0; //bejövő tranzakciók száma (txout-beli tranzakciók szerint)
	il->balance = 0;
	
	uint32_t* ids = (uint32_t*)malloc(sizeof(uint32_t)*2*N);
	if(!ids) {
		fprintf(stderr,"ids_gen(): nem sikerült memóriát lefoglalni!\n");
		free(il);
		return 0;
	}
	
	unsigned int i;
	for(i=0;i<N;i++) {
		ids[2*i] = e[i].p1;
		ids[2*i+1] = e[i].p2;
	}
	
	quicksort(ids,0,2*N);
	unsigned int j=0;
	uint32_t last = ids[0];
	for(i=1;i<2*N;i++) {
		if(ids[i] != last) {
			j++;
			ids[j] = ids[i];
			last = ids[i];
		}
	}
	il->N = j+1;
	
	il->ids = (uint32_t*)malloc(sizeof(uint32_t)*(il->N));
	if( ! (il->ids) ) {
		fprintf(stderr,"ids_gen(): nem sikerült memóriát lefoglalni!\n");
		free(il);
		free(ids);
		return 0;
	}
	
	for(i=0;i<(il->N);i++) {
		il->ids[i] = ids[i];
	}
	free(ids);
	return il;
}

//N db élben az ID-k kicserélése a sorszámukra
int ids_replace(const idlist* il, edge* e, unsigned int N) {
	if(!(il && e)) return 1;
	if(!N) return 0;
	unsigned int i;
	for(i=0;i<N;i++) {
		uint32_t i1 = e[i].p1;
		uint32_t i11 = ids_find(il,i1);
		if(i11 >= il->N) {
			fprintf(stderr,"ids_replace(): a(z) %u ID-t nem találtam!\n",i1);
			return 2;
		}
		e[i].p1 = i11;
		
		uint32_t i2 = e[i].p2;
		uint32_t i21 = ids_find(il,i2);
		if(i21 >= il->N) {
			fprintf(stderr,"ids_replace(): a(z) %u ID-t nem találtam!\n",i2);
			return 2;
		}
		e[i].p2 = i21;
	}
	return 0;
}



