/*
 * edgeheap.cpp -- élek tárolása időben rendezetten
 * 
 * Copyright 2013 Kondor Dániel <kondor.dani@gmail.com>
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
 */
 
#include "edgeheap.h"


void edgeheap::clean() {
	if(heap != MAP_FAILED) {
		munmap(heap,hsize*sizeof(unsigned int));
		heap = (unsigned int*)MAP_FAILED;
	}
	hsize = 0;
	hn = 0;
}
		
int edgeheap::grow() {
	uint64_t map_size = grow0*sizeof(unsigned int);
	uint64_t old_size = hsize*sizeof(unsigned int);
	if(heap != MAP_FAILED) {
		if(hsize == 0) {
			fprintf(stderr,"edgeheap::grow(): hibás adatok!\n");
			return 1; //hiba
		}
		void* ptr2 = (void*)mremap(heap,old_size,old_size+map_size,MREMAP_MAYMOVE);
		if(ptr2 == MAP_FAILED) {
			fprintf(stderr,"edgeheap::grow(): nincs elég memória!\n");
			return 2;
		}
		heap = (unsigned int*)ptr2;
		hsize += grow0;
	}
	else {
		void* ptr2 = mmap(0,map_size,PROT_READ | PROT_WRITE,MAP_ANONYMOUS | MAP_PRIVATE,-1,0);
		if(ptr2 == MAP_FAILED) {
			fprintf(stderr,"edges_grow(): nincs elég memória!\n");
			return 3;
		}
		heap = (unsigned int*)ptr2;
		hsize = grow0;
	}
	return 0;
}

void edgeheap::heapup(unsigned int i) { // szokásos heapup, elem felvitele amíg lehet
	edge* e1 = e->e;
	unsigned int timestamp = e1[heap[i]].timestamp;
	while(i) {
		unsigned int parent = (i-1)/2;
		if(timestamp < e1[heap[parent]].timestamp) {
			unsigned int tmp = heap[i];
			heap[i] = heap[parent];
			heap[parent] = tmp;
			e1[heap[i]].offset = i;
			e1[heap[parent]].offset = parent;
			i = parent;
		}
		else break;
	}
}

void edgeheap::heapdown(unsigned int i) { // szokásos heapdown, elem elhelyezése a helyére
	edge* e1 = e->e;
	unsigned int timestamp = e1[heap[i]].timestamp;
	while(1) {
		unsigned int c1 = 2*i+1;
		unsigned int c2 = 2*i+2;
		if(c1 >= hn) break; //már az alján vagyunk
		unsigned int c;
		if(e1[heap[c1]].timestamp < timestamp) {
			if(c2 < hn) {
				if(e1[heap[c2]].timestamp < e1[heap[c1]].timestamp) {
					//c2 helyére kell betenni
					unsigned int tmp = heap[i];
					heap[i] = heap[c2];
					heap[c2] = tmp;
					e1[heap[i]].offset = i;
					e1[heap[c2]].offset = c2;
					i = c2;
					continue;
				}
			}
			//c1 helyére kell betenni
			unsigned int tmp = heap[i];
			heap[i] = heap[c1];
			heap[c1] = tmp;
			e1[heap[i]].offset = i;
			e1[heap[c1]].offset = c1;
			i = c1;
			continue;
		}
		else {
			if(c2 < hn) {
				if(e1[heap[c2]].timestamp < timestamp) {
					//c2 helyére kell betenni
					unsigned int tmp = heap[i];
					heap[i] = heap[c2];
					heap[c2] = tmp;
					e1[heap[i]].offset = i;
					e1[heap[c2]].offset = c2;
					i = c2;
					continue;
				}
			}
		}
		break; //ha ide jutottunk, akkor jó helyen van az elem
	}
}

void edgeheap::shiftup(unsigned int i) { // i-edik elem átrendezése legfelülre -- ez nem tartja meg a heap-tulajdonságot, de csak
		// az 0. elem lesz hibás, a két al-heap jó marad
		// ez után a 0. elemet törölni kell, és egy új elemet kell a helyére beilleszteni, majd levinni, ameddig lehet
	edge* e1 = e->e;
	unsigned int hi = heap[i]; //hi legfelülre, a többi rekurzívan lefele
		//itt nem kell a timestamp-okat ellenőrizni, azok jók maradnak
	while(i) {
		unsigned int p = (i-1)/2;
		heap[i] = heap[p];
		e1[heap[i]].offset = i;
		i = p;
	}
	heap[0] = hi;
	e1[heap[0]].offset = 0;
}

int edgeheap::add(unsigned int n) { //edges[n] hozzáadása
	edge* e1 = e->e;
	if(hn == hsize) {
		int r = grow();
		if(r) return r;
	}
	heap[hn] = n;
	e1[n].offset = hn;
	hn++;
	heapup(hn-1);
	return 0;
}

int edgeheap::del(unsigned int n) { //edges[n] törlése
	edge* e1 = e->e;
	unsigned int i = e1[n].offset;
	if(i >= hn) {
		fprintf(stderr,"edgeheap::del(%u): a törölni kívánt elem nem szerepel a heap-ben!\n",n);
		return 1;
	}
	if(hn == 1) { hn = 0; return 0; }
	if(hn == 2) {
		if(i == 0) heap[0] = heap[1];
		hn--;
		return 0;
	}
	shiftup(i);
	hn--;
	heap[0] = heap[hn];
	e1[heap[0]].offset = 0;
	heapdown(0);
	return 0;
}

//legfelső elem (legkisebb timestamp) törlése
void edgeheap::del0() {
	if(!hn) return;
	if(hn == 1) { hn = 0; return; }
	if(hn == 2) {
		heap[0] = heap[1];
		hn--;
		return;
	}
	edge* e1 = e->e;
	hn--;
	heap[0] = heap[hn];
	e1[heap[0]].offset = 0;
	heapdown(0);
}

