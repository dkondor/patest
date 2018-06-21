/*
 * patest6r3.cpp -- preferential attachment tesztelése, Pósfai Marci által javasolt verzió:
 * 	minden él csatlakozásánál a fokszám visszatranszformálása az aktuális CDF-nek megfelelően,
 * 	ahol a CDF az adott fokszámú pontok választásának a valószínűségéből jön,
 * 	ezt d^a-nak vesszük, ahol az a paraméter állítható
 * ezzel egy d fokszámú pont választásának a valószínűsége: d^a * n_d, ahol n_d a d fokszámú pontok száma
 * 
 * új változat, az egyenlegek számításánál is használt red-black tree implementációt használva, az ottani működés tesztelésére
 * 2015-04-04: számolás egyszerre több exponensre is (red_black_tree2.[ch])
 * 
 * 2015-04-09: popen() helyett popen_noshell használata, a fork() elkerülésére
 * 
 * Preferential attachment megvalósulásának a tesztelése a Bitcoin hálózatra
 * minden új linkre (két cím / felhasználó közötti első kapcsolatra) megnézzük, hogy abban a pillanatban
 * a felhasználóknak mik a tulajdonságai: fokszám, korábbi tranzakciók száma (esetleg csak egy időablakban),
 * egyenleg, esetleg továbbiak is
 * 
 * Copyright 2015 Kondor Dániel <kondor.dani@gmail.com>
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include "idlist.h"
#include "edges.h"
#include "red_black_tree2.h"
#include "edgeheap.h"
#include "popen_noshell.h"
#include "read_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>

typedef struct edgerecord_t { //egy bejegyzés az eléket tartalmazó fájlban
	unsigned int in;
	unsigned int out;
	unsigned int timestamp;
} erecord;

//egy új bejegyzés beolvasása (ha van még a fájlban)
int erecord_read(read_table* f, erecord* r, int ignore_invalid) {
	while(1) {
		if(read_table_line(f)) return -1;
		
		unsigned int in,out,timestamp;
		if(read_table_uint32(f,&in) || read_table_uint32(f,&out)) {
			if(ignore_invalid && read_table_get_last_error(f) == T_OVERFLOW) continue;
			else return -1;
		}
		if(read_table_uint32(f,&timestamp)) return -1;
		r->in = in;
		r->out = out;
		r->timestamp = timestamp;
		return 0;
	}
}

static char gzip0[] = "/usr/bin/zip";
static char zcat[] = "/bin/zcat";


int ouf_open(FILE** out1, FILE** outn, unsigned int na, char* gzip, int foutn, char* fout2, double* aa, char** buf, int seq, popen_noshell_pass_to_pclose* p1, popen_noshell_pass_to_pclose* pn) {
	unsigned int j;
	for(j=0;j<na;j++) { out1[j] = 0; outn[j] = 0; }
	int hiba = 0;
	for(j=0;j<na;j++) {
		if(gzip) {
			//tömörített kimenet
			sprintf(buf[0],"%s",gzip);
			buf[2][0] = '-';
			buf[2][1] = 0;
			if(foutn) {
				//két kimeneti fájl
				sprintf(buf[1],"%s-%.2f-o-%d.dat.zip",fout2,aa[j],seq);
				out1[j] = popen_noshell(gzip,buf,"w",p1+j,0);
				if(!out1[j]) { hiba = 1; break; }
				sprintf(buf[1],"%s-%.2f-n-%d.dat.zip",fout2,aa[j],seq);
				outn[j] = popen_noshell(gzip,buf,"w",pn+j,0);
				if(!outn[j]) { hiba = 1; break; }
			}
			else {
				//csak egy kimenet
				sprintf(buf[1],"%s-%.2f-0-%d.dat.zip",fout2,aa[j],seq);
				out1[j] = popen_noshell(gzip,buf,"w",p1+j,0);
				if(!out1[j]) { hiba = 1; break; }
				outn[j] = out1[j];
			}
		}
		else {
			//fájlok normál megnyitása
			if(foutn) {
				//két kimeneti fájl
				sprintf(buf[1],"%s-%.2f-o-%d.dat",fout2,aa[j],seq);
				out1[j] = fopen(buf[1],"w");
				if(!out1[j]) { hiba = 1; break; }
				sprintf(buf[1],"%s-%.2f-n-%d.dat",fout2,aa[j],seq);
				outn[j] = fopen(buf[1],"w");
				if(!outn[j]) { hiba = 1; break; }
			}
			else {
				//csak egy kimenet
				sprintf(buf[1],"%s-%.2f-0-%d.dat",fout2,aa[j],seq);
				out1[j] = fopen(buf[1],"w");
				if(!out1[j]) { hiba = 1; break; }
				outn[j] = out1[j];
			}
		}
	} //for(j) -- kimeneti fájlok megnyitása
	return hiba;
}

//kimeneti fájlok bezárása
void outf_close(FILE** out1, FILE** outn, unsigned int na, char* gzip, int foutn, popen_noshell_pass_to_pclose* p1, popen_noshell_pass_to_pclose* pn) {
	unsigned int j;
	for(j=0;j<na;j++) {
		if(out1[j]) {
			if(gzip) {
				pclose_noshell(p1 + j);
			}
			else fclose(out1[j]);
		}
		if(foutn && outn[j]) {
			if(gzip) {
				pclose_noshell(pn + j);
			}
			else fclose(outn[j]);
		}
		out1[j] = 0;
		outn[j] = 0;
	}
}

int strtodint(char* a,unsigned int* delay) {
	char* a1 = 0;
	unsigned int delay2 = strtoul(a,&a1,10);
	if(a1 == a) {
		return 1;
		
	}
	if(*a1) { switch(*a1) {
		case 'h':
			*delay = delay2*3600;
			break;
		case 'd':
			*delay = delay2*86400;
			break;
		case 'w':
			*delay = delay2*604800;
			break;
		case 'm':
			*delay = delay2*2592000;
			break;
		case 'y':
			*delay = delay2*31536000;
			break;
		default:
			*delay = delay2;
	} }
	else *delay = delay2;
	return 0;
}


int main(int argc, char **argv)
{
	/*
	 * bemeneti fájlok:
	 *  0. links -- összes előforduló él egyszer, címek szerint rendezve
	 * 	1. ids -- címek / felhasználók listája
	 * 	2. txedge -- összes első tranzakció, idő szerint rendezve (címek és időpont)
	 */
	unsigned int N = 0; //id-k száma
	char* fids = 0;
	char* ftxedge = 0;
	char* flinks = 0;
	int ignore_invalid = 1; /* ignore "invalid" node IDs that cause overflow / underflow (-1 for unknown addresses typically) */
	
	char* fout2 = 0; //eredmények kiírása ide -> alapfájlnév, ebből generálás: %s-%f-n/a.gz
	int foutn = 0; //új pontok (első kimenő él) kiírása külön
	char* gzip = gzip0; //tömörítéshez használt program, ha nincs megadva, akkor tömörítetlen kimenet
	//~ record rin;
	//~ record rout;
	erecord edge1;
	idlist* il = 0;
	rb_red_blk_tree* d1 = 0;
	int newedge = 0; /*	0 -- összes tranzakcióval foglalkozunk (akkor is, ha még aktív az adott él)
					1 -- csak akkor foglalkozunk, ha már nem aktív az adott él (lehet új él, és deaktivált él újraaktiválása)
					2 -- csak az új élekkel foglalkozunk (ha a timestamp == 0)*/
	
	unsigned int delay = 2592000; //linkek élettartama (30 nap)
	edges* ee = 0;
	edgeheap eh;
	unsigned int nedges = 0; //élek száma
	unsigned int DE1 = 100000; //ennyi él feldolgozása után kiírás
	
	double* aa = 0; //preferential attachment kitevők
	double** aa2 = 0;
	unsigned int na = 0;
	
	double a0 = 1.0;

	time_t t1 = time(0);
	time_t t3 = 0;
	int zip = 0; //bemeneti fájlok tömörítve
	int edgehelper = 0;

	//~ unsigned int neg = 0; //olyan esetek, amikor negatív számot kaptunk volna egyenlegnek
	unsigned int nedges2 = 0; //feldolgozott tranzakciók száma (többszörös élek többször)
	//~ unsigned int txin = 0; //feldolgozott bemenetek száma
	//~ unsigned int txout = 0; //feldolgozott kimenetek száma
	
	unsigned int DENEXT = DE1;
	double norm = 0.0; //normálási tényező
	
	unsigned int filesdelay = 0; //új kimeneti fájl nyitása ennyi időnként (bemeneti timestamp-ek szerint)
	time_t fdnext = 0;
	popen_noshell_pass_to_pclose pc1;

	int i,r = 0;
	for(i=1;i<argc;i++) if(argv[i][0] == '-') switch(argv[i][1]) {
		case 'e':
		case 't':
			ftxedge = argv[i+1];
			break;
		case 'l':
			flinks = argv[i+1];
			break;
		case 'D':
			DE1 = atoi(argv[i+1]);
			break;
		case 'i':
			fids = argv[i+1];
			break;
		case 'N':
		case 'n':
			N = atoi(argv[i+1]);
			break;
		case 'O':
			if(argv[i][2] == '2') { fout2 = argv[i+1]; break; }
			if(argv[i][2] == 'n') { foutn = 1; break; }
			fprintf(stderr,"Ismeretlen paraméter: %s!\n",argv[i]);
			break;
		case 'F':
			if(strtodint(argv[i+1],&filesdelay)) fprintf(stderr,"Érvénytelen paraméter: %s %s!\n",argv[i],argv[i+1]);
			if(filesdelay && filesdelay < 86400) filesdelay = 86400;
			break;
		case 'd':
			if(strtodint(argv[i+1],&delay)) fprintf(stderr,"Érvénytelen paraméter: %s %s!\n",argv[i],argv[i+1]);
			break;
		case '0':
			newedge = 2;
			break;
		case '1':
			newedge = 1;
			break;
		case 'z':
			if(argv[i][2] == '0') gzip = 0;
			else gzip = argv[i+1];
			break;
		case 'a':
			//vizsgálandó kitevők
			{
				if(aa) {
					fprintf(stderr,"Hibás paraméterek: több '-a' paraméter is meg van adva!\n");
					break;
				}
				unsigned int j = i+1;
				for(;j<argc;j++) {
					if(!(isdigit(argv[j][0]) || argv[j][0] == '.')) break;
				}
			unsigned int na2 = j-i-1;
				if(!na2) {
					aa = &a0;
					na = 1;
				}
				else {
					na = na2;
					aa = (double*)malloc(sizeof(double)*na);
					aa2 = (double**)malloc(sizeof(double**)*na);
					if(!(aa && aa2)) {
						fprintf(stderr,"Hiba: nem sikerült memóriát lefoglalni!\n");
						if(aa) free(aa);
						if(aa2) free(aa2);
						return 1;
					}
					for(j=0;j<na;j++) {
						aa[j] = atof(argv[i+j+1]);
						aa2[j] = aa + j;
						// aa[j] == aa2[j][0] (*(aa2[j]))
					}
				}
				break;
			}
			break;
		case 'Z':
			zip = 1;
			break;
		case 'H':
			edgehelper = 0;
			break;
		case 'I':
			ignore_invalid = 0;
			break;
		default:
			fprintf(stderr,"Ismeretlen paraméter: %s!\n",argv[i]);
			break;
	}
	
	if(!aa) {
		aa = &a0;
		aa2 = &aa;
		na = 1;
	}
	
	
	if( ! (ftxedge && fids && flinks && N) ) {
		fprintf(stderr,"Nincsenek bemeneti fájlok megadva!\n");
		return 1;
	}
	
	FILE* fi = 0;
	if(zip) {
		char* cmd1[3] = {zcat,fids,0};
		fi = popen_noshell(zcat,cmd1,"r",&pc1,0);
	}
	else fi = fopen(fids,"r");
	il = ids_read(fi,N);
	if(zip) pclose_noshell(&pc1);
	else fclose(fi);
	if(!il) {
		fprintf(stderr,"Nem sikerült azonosítókat beolvasni a(z) %s fájlból!\n",fids);
		return 3;
	}
	N = il->N;
	
	d1 = RBTreeCreate(CmpInt64,NullFunction,NullFunction,NullFunctionConst,NullFunction,DFInt64,(void**)aa2,na);
	if(!d1) {
		fprintf(stderr,"Nem sikerült memóriát lefoglalni (%u)!\n",__LINE__);
		ids_free(il);
		return 4;
	}
	
	FILE* e = 0;
	FILE* links = 0;
	
	if(zip) {
		char* cmd1[3] = {zcat,flinks,0};
		links = popen_noshell(zcat,cmd1,"r",&pc1,0);
	}
	else links = fopen(flinks,"r");
	if(!links) {
		fprintf(stderr,"Nem sikerült megnyitni a(z) %s bemeneti fájlt!\n",flinks);
		ids_free(il);
		RBTreeDestroy(d1);
		return 5;
	}
	
	ee = edges_read(links);
	if(zip) pclose_noshell(&pc1);
	else fclose(links);
	if(!ee) {
		fprintf(stderr,"Nem sikerült az éleket beolvasni a(z) %s fájlból!\n",flinks);
		ids_free(il);
		RBTreeDestroy(d1);
		return 2;
	}
	eh.e = ee; //ezt valahogy automatikussá kellene tenni (esetleg az edges tömböt teljesen integrálni az edgeheap-be)
	if(edgehelper) edges_createhelper(ee,il);
	
	if(zip) {
		char* cmd1[3] = {zcat,ftxedge,0};
		e = popen_noshell(zcat,cmd1,"r",&pc1,0);
	}
	else e = fopen(ftxedge,"r");
	if(!e) {
		fprintf(stderr,"Nem sikerült megnyitni a(z) %s bemeneti fájlt!\n",ftxedge);
		ids_free(il);
		RBTreeDestroy(d1);
		edges_free(ee);
		return 5;
	}
	read_table* e_rt = read_table_new(e);
	read_table_set_fn(e_rt,ftxedge);
	
	//kimeneti fájlok -- minden exponenshez külön fájl, tömörítve
	FILE** out1 = (FILE**)malloc(sizeof(FILE*)*na);
	FILE** outn = (FILE**)malloc(sizeof(FILE*)*na);
	popen_noshell_pass_to_pclose* p1 = 0;
	popen_noshell_pass_to_pclose* pn = 0;
	if(foutn) {
		p1 = (popen_noshell_pass_to_pclose*)malloc(sizeof(popen_noshell_pass_to_pclose)*na*2);
		pn = p1 + na;
	}
	else p1 = (popen_noshell_pass_to_pclose*)malloc(sizeof(popen_noshell_pass_to_pclose)*na);
	char* buf2[4];
	buf2[0] = (char*)malloc(sizeof(char)*(strlen(gzip) + 1));
	buf2[1] = (char*)malloc(sizeof(char)*(strlen(fout2) + 50));
	char buf3[4];
	buf2[2] = buf3;
	buf2[3] = 0;
	unsigned int lines1 = 0;
	unsigned int linesn = 0;
	int seq = 1;
	if(!(out1 && outn && buf2[0] && buf2[1] && buf2[2] && p1)) {
		fprintf(stderr,"Nem sikerült memóriát lefoglalni!\n");
		ids_free(il);
		RBTreeDestroy(d1);
		edges_free(ee);
		if(zip) pclose_noshell(&pc1);
		else fclose(e);
		read_table_free(e_rt);
		if(out1) free(out1);
		if(outn) free(outn);
		if(buf2[0]) free(buf2[0]);
		if(buf2[1]) free(buf2[1]);
		if(p1) free(p1);
		return 3;
	}
	
	if(ouf_open(out1,outn,na,gzip,foutn,fout2,aa,buf2,seq,p1,pn)) {
		fprintf(stderr,"Hiba a kimeneti fájlok megnyitásakor!\n");
		r = 4;
		goto pt6_end;
	}
	
	
	
	t3 = time(0);

	r = erecord_read(e_rt,&edge1,ignore_invalid);
	if(r != 0) {
		fprintf(stderr,"Nem sikerült adatokat beolvasni a bemeneti fáljokból!\n");
		read_table_write_error(e_rt,stderr);
		r = 8;
		goto pt6_end;
	}
	fdnext = edge1.timestamp + filesdelay;
	
	do {
		//új rekord az edge változóban, ezt kell feldolgozni, ehhez az rin és rout változókon kell iterálni, amíg el nem érjük a tranzakció időpontját
		unsigned int timestamp = edge1.timestamp;
		unsigned int time1 = 0;
		if(timestamp > delay) time1 = timestamp - delay;
		
		//régi élek törlése
		r = 0;
		while(eh.hn) {
			if(ee->e[eh.heap[0]].timestamp >= time1) break; //összes él újabb
			
			//az eh.heap[0] él régebbi, törölni kell
			//fokszámok csökkentése először
			unsigned int idin = ids_find2(il,ee->e[eh.heap[0]].p1); //változás a korábbi programhoz képest:
			unsigned int idout = ids_find2(il,ee->e[eh.heap[0]].p2); //az éleknél az eredeti ID-ket tároljuk
			if(idin >= il->N || idout >= il->N) { //ez itt nem fordulhat elő, az összes ID-nek szerepelnie kell a felsorolásban
				fprintf(stderr,"Hiba: inkonzisztens adatok (%u)!\n",__LINE__);
				break;
			}
			//~ r = deg2_del(d1,il->inlinks[idout]);
			if(il->inlinks[idout] == 0 || il->outlinks[idin] == 0) { //hiba
				fprintf(stderr,"Hiba: inkonzisztens adatok (%u)!\n",__LINE__);
				r = 1;
				break;
			}
			rb_red_blk_node* node = RBExactQuery(d1,(void*)(uint64_t)(il->inlinks[idout]));
			if(!node) {
				fprintf(stderr,"Hiba: inkonzisztens adatok (%u)!\n",__LINE__);
				r = 1;
				break;
			}
			RBDelete(d1,node);
			//~ norm -= pow((double)(il->inlinks[idout]),aa[0]);
			r = 0;
			il->inlinks[idout]--;
			il->outlinks[idin]--;
			
			if(il->inlinks[idout]) {
				RBTreeInsert(d1,(void*)(uint64_t)(il->inlinks[idout]),0);
				//~ norm += pow((double)(il->inlinks[idout]),aa[0]);
			}
			
			eh.del0(); //heap átrendezése (legfelső elem törlése)
		}
		if(r) break;
		
		
		//feldolgoztuk a be- és kimenő összegeket, most vethetjük össze a statisztikákkal
		unsigned int idin = ids_find2(il,edge1.in);
		unsigned int idout = ids_find2(il,edge1.out);
		
		if(idin >= il->N || idout >= il->N) { //ez itt nem fordulhat elő, az összes ID-nek szerepelnie kell a felsorolásban
			fprintf(stderr,"Hiba: inkonzisztens adatok (%u)!\n",__LINE__);
			r = 1;
			break;
		}
	
		if(edge1.in != edge1.out) { //érvényes tranzakciót olvastunk be
			//a "cél" címmel foglalkozunk
			unsigned int indeg = il->inlinks[idout];
			unsigned int outdeg = il->outlinks[idin];
			r = 0;
			int new1 = 0; //0, ha aktív az él, ekkor semmit sem kell csinálni
				//1, ha már szerepelt ez az él, de nem volt aktív (delay-nél nagyobb idő óta)
				//2, ha még nem szerepelt (timestamp == 0)
			
			unsigned int eid = edges_find(ee,edge1.in,edge1.out); //feltesszük, hogy ez az él még nem szerepelt
			if(eid >= ee->nedges) {
				fprintf(stderr,"Hiba: inkonzisztens adatok (%u)!\n",__LINE__);
				r = 1;
				break;
			}
			
			{ //időpont frissítése
				unsigned int t2 = ee->e[eid].timestamp;
				ee->e[eid].timestamp = timestamp;
				if(t2 < time1) { //nem aktív ez az él
					r = eh.add(eid); //hozzá kell adni a heap-hoz
					if(r) break; //hiba
					if(t2 == 0) new1 = 2; //még egyszer sem volt aktív
					else new1 = 1; //korábban már aktív volt, de már deaktiváltuk
				}
				else { //még aktív, az időpontot kell csak frissíteni
					unsigned int off = ee->e[eid].offset;
					eh.heapdown(off); //csak lefelé mehet, az új timestamp nagyobb
				}
			}
			
			nedges2++;
			/*
			 * newedge és new1 különböző variációi:
			 * 	deg_add() szükséges, ha new1 == 1 vagy 2 (nem volt aktív az él)
			 * 	newedge == 0 -- ekkor mindenképpen szükséges a deg_link()
			 * 	newedge == 1 -- ekkor csak akkor szükséges a deg_link(), ha new1 == 1 vagy new1 == 2
			 * 	newedge == 2 -- csak akkor szükséges a deg_link(), ha new1 == 2
			 * => összesítve: deg_add(), ha new1 > 0, deg_link(), ha new1 >= newedge
			 */
			rb_red_blk_node* node = 0;
			if(new1 >= newedge) { //új él, T_k valószínűségek frissítése
			unsigned int j;
				nedges++;
				r = 0;
				//T_k valószínűség frissítése (csak ha ez az él még egyáltalán nem szerepelt)
				double cdf1[na]; //!! ezt csak gcc-n lehet megcsinálni !!
				double norm1[na];
				if(indeg) {
					node = RBExactQueryMin(d1,(void*)(uint64_t)(indeg));
					if(!node) {
						fprintf(stderr,"Hiba: inkonzisztens adatok (%u)!\n",__LINE__);
						r = 1;
						break;
					}
					GetNodeRank(d1,node,cdf1);
					GetNorm(d1,norm1);
					for(j=0;j<na;j++) cdf1[j] /= norm1[j];
				}
				else for(j=0;j<na;j++) cdf1[j] = 0.0;
				
				//kiírás (minden egyedi eseményt kiírunk)
				FILE** out3 = out1;
				if(il->outtx[idin] == 0) {
					out3 = outn; //teljesen új pont, első él
					linesn++;
				}
				else lines1++;
				for(j=0;j<na;j++) fprintf(out3[j],"%g\n",cdf1[j]);
			}
			
			if(new1) { //inaktív él, fokszámok frissítése
				//fokszámok frissítése
				if(indeg) {
					if(!node) node = RBExactQuery(d1,(void*)(uint64_t)(indeg));
					if(!node) {
						fprintf(stderr,"Hiba: inkonzisztens adatok (%u)!\n",__LINE__);
						r = 1;
						break;
					}
					RBDelete(d1,node);
					//~ norm -= pow((double)indeg,aa[0]);
				}
				il->inlinks[idout]++;
				il->outlinks[idin]++;
				
				il->intx[idout]++;
				il->outtx[idin]++;
				
				RBTreeInsert(d1,(void*)(uint64_t)(il->inlinks[idout]),0);
				//~ norm += pow((double)(il->inlinks[idout]),aa[0]);
				r = 0;
			}
		}
		
		//új kimeneti fájlok megnyitása, ha külön írjuk ki
		if(filesdelay) {
			if(timestamp > fdnext) {
				outf_close(out1,outn,na,gzip,foutn,p1,pn);
				seq++;
				if(ouf_open(out1,outn,na,gzip,foutn,fout2,aa,buf2,seq,p1,pn)) {
					fprintf(stderr,"Hiba a kimeneti fájlok megnyitásakor!\n");
					r = 4;
					goto pt6_end;
				}
				fdnext += filesdelay;
			}
		}
		
		if(DE1) if(nedges2 >= DENEXT) {
			fprintf(stderr,"%u él feldolgozva\n",nedges2);
			DENEXT = nedges2 + DE1;
		}
		
		//új tranzakció beolvasása
		r = erecord_read(e_rt,&edge1,ignore_invalid);
	} while(r == 0);
	
	if(read_table_get_last_error(e_rt) != T_EOF) {
		fprintf(stderr,"Nem sikerült adatokat beolvasni a bemeneti fáljokból!\n");
		read_table_write_error(e_rt,stderr);
		r = 8;
	}
	
pt6_end:
	
	if(zip) pclose_noshell(&pc1);
	else fclose(e);
	read_table_free(e_rt);
	outf_close(out1,outn,na,gzip,foutn,p1,pn);
	free(out1);
	free(outn);
	free(buf2[0]);
	free(buf2[1]);
	free(p1);
	
	fprintf(stdout,"feldolgozott élek: %u, tranzakciók: %u\n",nedges,nedges2);
	//kiírt sorok számának a kiírása
	if(foutn) {
		fprintf(stdout,"meglevő pontból kiinduló élek: %u\núj pontból kiinduló élek: %u\n",lines1,linesn);
	}
	
	time_t t2 = time(0);
	fprintf(stdout,"futásidő: %u\n",(unsigned int)(t2-t1));
	fprintf(stdout,"futásidő: %u (adatok beolvasása: %u, feldolgozás: %u)\n",(unsigned int)(t2-t1),(unsigned int)(t3-t1),(unsigned int)(t2-t3));
	
	
	ids_free(il);
	RBTreeDestroy(d1);
	edges_free(ee);
	
	return r;
}

