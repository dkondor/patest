/*
 * patestbr2.c -- preferential attachment tesztelése a címekhez bejövő összegekre
 *	egyenlegek / bejövő összegek nyilvántartása a red_black_tree.[ch] szerint,
 * 	egyelőre csak egy exponensre lehet egyszerre futtatni
 * 
 * Copyright 2015 Kondor Dániel <kondor.dani@gmail.com>
 * 
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
 * 
 * 
 */

#include "idlist.h"
#include "red_black_tree2.h"
#include "popen_noshell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>

typedef struct record_t { //egy bejegyzés a bemeneti fájlokban (txint.txt és txoutt.txt)
	unsigned int id;
	uint64_t value;
	unsigned int timestamp;
} record;

//egy új bejegyzés beolvasása (ha van még a fájlban)
int record_read(FILE* f, record* r) {
	if(!f) return -1; //ha már korábbana a fájl végére értünk
	
	//egy sort kell beolvasnunk, de lehetnek kommentek -- a nem számokkal kezdődő sorokat átugorjuk
	int read = 0;
	do {
		int a = fgetc(f);
		while(a == ' ' || a == '\t') a = fgetc(f);
		if(a == EOF) return -1; //véget ért a fájl, vagy hiba, még mielőtt sikerült valamit beolvasni
		if(a == '\n') continue;
		if(!isdigit(a)) goto rr_endl; //nem számmal kezdődik ('-' jel sem lehet, az ID-k pozitívak)
		
		ungetc(a,f);
		unsigned int id;
		uint64_t value;
		unsigned int timestamp;
		a = fscanf(f,"%u\t%lu\t%u",&id,&value,&timestamp);
		if(a == 3) {
			read = 1;
			r->id = id;
			r->value = value;
			r->timestamp = timestamp;
		}
rr_endl:
		do {
			a = fgetc(f);
		} while( ! (a == '\n' || a == EOF) );
		if(a == EOF) {
			if(!read) return -1; //véget ért a fájl anélkül, hogy sikerült volna valamit beolvasni
		}
	} while(!read);
	return 0; //ha ide jutottunk, akkor sikerült egy érvényes sort beolvasni
}



static char gzip0[] = "/usr/bin/zip";
static char zcat[] = "/bin/zcat";


int ouf_open(FILE** out1, unsigned int na, char* gzip, char* fout2, double* aa, char** buf, int seq, struct popen_noshell_pass_to_pclose* p1) {
	unsigned int j;
	for(j=0;j<na;j++) out1[j] = 0;
	int hiba = 0;
	for(j=0;j<na;j++) {
		if(gzip) {
			//tömörített kimenet
			sprintf(buf[0],"%s",gzip);
			buf[2][0] = '-';
			buf[2][1] = 0;
			{
				//csak egy kimenet
				sprintf(buf[1],"%s-%.2f-0-%d.dat.zip",fout2,aa[j],seq);
				out1[j] = popen_noshell(gzip,buf,"w",p1+j,0);
				if(!out1[j]) { hiba = 1; break; }
			}
		}
		else {
			//fájlok normál megnyitása
			{
				//csak egy kimenet
				sprintf(buf[1],"%s-%.2f-0-%d.dat",fout2,aa[j],seq);
				out1[j] = fopen(buf[1],"w");
				if(!out1[j]) { hiba = 1; break; }
			}
		}
	} //for(j) -- kimeneti fájlok megnyitása
	return hiba;
}

//kimeneti fájlok bezárása
void outf_close(FILE** out1, unsigned int na, char* gzip, struct popen_noshell_pass_to_pclose* p1) {
	unsigned int j;
	for(j=0;j<na;j++) {
		if(out1[j]) {
			if(gzip) {
				pclose_noshell(p1 + j);
			}
			else fclose(out1[j]);
		}
		out1[j] = 0;
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
	 * 	1. ids -- címek / felhasználók listája
	 * 	2. txout -- tranzakciók kimenetei (címekhez bejövő összegek)
	 * 	3. txin -- tranzakciók bemenetei (címektől kimenő összegek -- csak akkor van rá szükség,
	 * 		ha a teljes egyenlegeket tartjuk számon)
	 */
	unsigned int N = 0; //id-k száma
	char* fids = 0;
	char* ftxin = 0;
	char* ftxout = 0;
	
	char* fout2 = 0; //eredmények kiírása ide -> alapfájlnév, ebből generálás: %s-%f-n/a.gz
	char* gzip = gzip0; //tömörítéshez használt program, ha nincs megadva, akkor tömörítetlen kimenet
	//~ record rin;
	//~ record rout;
	
	idlist* il = 0;
	rb_red_blk_tree* d1 = 0;
	
	unsigned int DE1 = 100000; //ennyi bemenet feldolgozása után kiírás
/*	
dani@dani-desktop /m/d/b/patest2> zcat data2/txint2.gz | wc -l
36383197
dani@dani-desktop /m/d/b/patest2> zcat data2/txoutt2.gz | wc -l
41753851
*/

	double* aa = 0; //preferential attachment kitevők
	double** aa2 = 0;
	unsigned int na = 0;
	
	double a0 = 1.0;

	time_t t1 = time(0);
	int zip = 0; //bemeneti fájlok tömörítve
	unsigned int DENEXT;
	
	int txin0 = 0; //ha meg van adva, akkor a bemenetekkel (címektől kimenő összegek) is foglalkozunk, ha nincs,
		//akkor csak a címekhez bejövőkkel
	int64_t thres = 1; //csak az e feletti összegekkel foglalkozunk
	int partial = 0; //nem szerepel az összes ID a vizsgálandó címek között
	
	int64_t* excl = 0; //a pontosan az ebben szereplő egyenleggel rendelkező címeket kihagyjuk az eloszlásból
	unsigned int nexcl = 0;

	unsigned int filesdelay = 0; //új kimeneti fájl nyitása ennyi időnként (bemeneti timestamp-ek szerint)
	time_t fdnext = 0;
	struct popen_noshell_pass_to_pclose pc1,pc2;

	int i,r = 0;
	for(i=1;i<argc;i++) if(argv[i][0] == '-') switch(argv[i][1]) {
		case 'D':
			DE1 = atoi(argv[i+1]);
			break;
		case 'd':
			fids = argv[i+1];
			break;
		case 'i':
			ftxin = argv[i+1];
			txin0 = 1;
			break;
		case 'o':
			ftxout = argv[i+1];
			break;
		case 'N':
		case 'n':
			N = atoi(argv[i+1]);
			break;
		case 'O':
			if(argv[i][2] == '2') { fout2 = argv[i+1]; break; }
			fprintf(stderr,"Ismeretlen paraméter: %s!\n",argv[i]);
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
		case 'I':
			txin0 = 1;
			break;
		case 't':
			thres = strtoll(argv[i+1],0,10);
			break;
		case 'p':
			partial = 1;
			break;
		case 'F':
			if(strtodint(argv[i+1],&filesdelay)) fprintf(stderr,"Érvénytelen paraméter: %s %s!\n",argv[i],argv[i+1]);
			if(filesdelay && filesdelay < 86400) filesdelay = 86400;
			break;
		case 'e':
			{
				if(excl) {
					fprintf(stderr,"Hibás paraméterek: több '-e' paraméter is meg van adva!\n");
					break;
				}
				unsigned int j = i+1;
				for(;j<argc;j++) {
					if(!isdigit(argv[j][0])) break;
				}
				unsigned int ne2 = j-i-1;
				if(!ne2) break;
				else {
					nexcl = ne2;
					excl = (int64_t*)malloc(sizeof(int64_t)*nexcl);
					if(!excl) {
						fprintf(stderr,"Hiba: nem sikerült memóriát lefoglalni!\n");
						return 1;
					}
					for(j=0;j<ne2;j++) {
						char* tmp2 = 0;
						excl[j] = strtoll(argv[i+j+1],&tmp2,10);
						if(tmp2 == argv[i+j+1] || excl[j] < 0) {
							fprintf(stderr,"Hiba: érvénytelen paraméterek!\n");
							free(excl);
							nexcl = 0;
							break;
						}
						if(*tmp2 == 'b' || *tmp2 == 'B') excl[j] *= 100000000L;
					}
					if(excl) {
						fprintf(stderr,"Kihagyott egyenlegek:");
						for(j=0;j<nexcl;j++) fprintf(stderr,"\t%lld",excl[j]);
						putc('\n',stderr);
					}
				}
				break;
			}
			break;
		default:
			fprintf(stderr,"Ismeretlen paraméter: %s!\n",argv[i]);
			break;
	}
	
	DENEXT = DE1;
	
	if(!aa) {
		aa = &a0;
		aa2 = &aa;
		na = 1;
	}
	
	if( ! (fids && ftxout && N && (!txin0 || ftxin) ) ) {
		fprintf(stderr,"Nincsenek bemeneti fájlok megadva!\n");
		return 1;
	}
	
	char* fntmp = 0;
	if(zip) {
		unsigned int l1 = strlen(fids);
		unsigned int l2 = strlen(ftxout);
		if(l2 > l1) l1 = l2;
		
		if(txin0) {
			l2 = strlen(ftxin);
			if(l2 > l1) l1 = l2;
		}
		
		fntmp = (char*)malloc(sizeof(char)*(l1 + strlen(zcat) + 4));
		if(!fntmp) {
			fprintf(stderr,"Nem sikerült memóriát lefoglalni!\n");
			return 2;
		}
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
	
	FILE* in = 0;
	if(txin0) {
		if(zip) {
			char* cmd1[3] = {zcat,ftxin,0};
			in = popen_noshell(zcat,cmd1,"r",&pc1,0);
		}
		else in = fopen(ftxin,"r");
	}
	FILE* out = 0;
	if(zip) {
		char* cmd1[3] = {zcat,ftxout,0};
		out = popen_noshell(zcat,cmd1,"r",&pc2,0);
	}
	else out = fopen(ftxout,"r");
	if( ! ( (in || !txin0)  && out) ) {
		fprintf(stderr,"Nem sikerült megnyitni a bemeneti fájlokat!\n");
		ids_free(il);
		RBTreeDestroy(d1);
		if(in) {
			if(zip) pclose_noshell(&pc1);
			else fclose(in);
		}
		if(out) {
			if(zip) pclose_noshell(&pc2);
			else fclose(out);
		}
		if(fntmp) free(fntmp);
		return 5;
	}
	
	//kimeneti fájlok -- minden exponenshez külön fájl, tömörítve
	FILE** out1 = (FILE**)malloc(sizeof(FILE*)*na);
	struct popen_noshell_pass_to_pclose* p1 = (struct popen_noshell_pass_to_pclose*)malloc(sizeof(struct popen_noshell_pass_to_pclose)*na);
	char* buf2[4];
	buf2[0] = (char*)malloc(sizeof(char)*(strlen(gzip) + 1));
	buf2[1] = (char*)malloc(sizeof(char)*(strlen(fout2) + 50));
	char buf3[4];
	buf2[2] = buf3;
	buf2[3] = 0;
	unsigned int lines1 = 0;
	int seq = 1;
	if(!(out1 && buf2[0] && buf2[1] && buf2[2] && p1)) {
		fprintf(stderr,"Nem sikerült memóriát lefoglalni!\n");
		ids_free(il);
		RBTreeDestroy(d1);
		if(in) {
			if(zip) pclose_noshell(&pc1);
			else fclose(in);
		}
		if(out) {
			if(zip) pclose_noshell(&pc2);
			else fclose(out);
		}
		if(out1) free(out1);
		if(buf2[0]) free(buf2[0]);
		if(buf2[1]) free(buf2[1]);
		if(p1) free(p1);
		return 3;
	}
	
	if(ouf_open(out1,na,gzip,fout2,aa,buf2,seq,p1)) {
		fprintf(stderr,"Hiba a kimeneti fájlok megnyitásakor!\n");
		r = 4;
		goto pt6_end;
	}
	
	
	
	unsigned int txin = 0;
	unsigned int txout = 0;
	record rin,rout;
	if(txin0) r = record_read(in,&rin);
	else r = 0;
	r = r | record_read(out,&rout);
	if(r != 0) {
		fprintf(stderr,"Nem sikerült adatokat beolvasni a bemeneti fáljokból!\n");
		r = 8;
		goto pt6_end;
	}
	fdnext = rout.timestamp + filesdelay;
	
	//~ double norm = 0.0; //normálási tényező
	
	while(1) {
		//új rekord az edge változóban, ezt kell feldolgozni, ehhez az rin és rout változókon kell iterálni, amíg el nem érjük a tranzakció időpontját
		unsigned int timestamp = rout.timestamp;
		
		if(in) {
			while(rin.timestamp < timestamp) {
				unsigned int id = rin.id;
				unsigned int j = ids_find(il,id);
				if(j >= il->N) {
					if(!partial) {
						fprintf(stderr,"Hiba: nem találtam %u azonosítójú pontot (%u)!\n",rin.id,__LINE__);
						r = 8;
						goto pt6_end;
					}
				}
				else {
					int64_t b1 = il->balance[j];
					
					if(rin.value > 0) {
						il->balance[j] -= rin.value;
						
						if(b1 > thres) {
							int bdel = 1;
							unsigned int e1;
							if(nexcl) for(e1=0;e1<nexcl;e1++) {
								if(b1 == excl[e1]) { bdel = 0; break; }
							}
							
							if(bdel) {
								//~ norm -= pow((double)b1,aa[0]);
								//~ if(norm < 0.0) {
									//~ fprintf(stderr,"Hiba: norm < 0.0 (%u)!\n",__LINE__);
									//~ r = 10;
									//~ goto pt6_end;
								//~ }
								
								rb_red_blk_node* n = RBExactQuery(d1,(void*)b1);
								if(!n) {
									fprintf(stderr,"Hiba: nem találtam %lu egyenlegű pontot (%u)!\n",b1,__LINE__);
									r = 9;
									goto pt6_end;
								}
								RBDelete(d1,n);
							}
						}
						
						
						if(il->balance[j] > thres) {
							int badd = 1;
							int64_t b2 = il->balance[j];
							unsigned int e1;
							if(nexcl) for(e1=0;e1<nexcl;e1++) {
								if(b2 == excl[e1]) { badd = 0; break; }
							}
							if(!badd) {
								rb_red_blk_node* n = RBExactQuery(d1,(void*)b2);
								if(!n) badd = 1;
							}
							
							if(badd) {
								RBTreeInsert(d1,(void*)b2,0);
								//~ norm += pow((double)b2,aa[0]);
							}
						}
					/*	if(r != 0) {
							fprintf(stderr,"Hibás adatok (%u)!\n",__LINE__);
							goto pt6_end;
						} */
					}
					
					txin++;
				}
				
				r = record_read(in,&rin);
				if(r != 0) { //a fájl végére értünk
					if(zip) pclose(in);
					else fclose(in);
					in = 0;
					r = 0;
					break;
				}
			}
		}
		
		unsigned int id = ids_find(il,rout.id); //pénzt kapó felhasználó
		if(id >= il->N) {
			if(!partial) {
				fprintf(stderr,"Hiba: nem találtam %u azonosítójú pontot (%u)!\n",rout.id,__LINE__);
				r = 8;
				break;
			}
		}
		
		else {
			//CDF számolása, kiírás
			if(il->balance[id] > thres) {
				double cdf1[na]; //!! ezt csak gcc-n lehet megcsinálni !!
				double norm1[na];
				unsigned int j;
				
				rb_red_blk_node* n = RBExactQuery(d1,(void*)(il->balance[id]));
				if(!n) {
					fprintf(stderr,"Hiba: nem találtam %lu egyenlegű pontot (%u)!\n",il->balance[id],__LINE__);
					r = 9;
					goto pt6_end;
				}
				GetNodeRank(d1,n,cdf1);
				GetNorm(d1,norm1);
				for(j=0;j<na;j++) cdf1[j] /= norm1[j];
				
				//kiírás (minden egyedi eseményt kiírunk) -- na == 1 egyelőre
				lines1++;
				for(j=0;j<na;j++) fprintf(out1[j],"%llu\t%llu\t%g\n",rout.value,il->balance[id],cdf1[j]);
				
				if(rout.value > 0) {
					int bdel = 1;
					unsigned int e1;
					if(nexcl) for(e1=0;e1<nexcl;e1++) {
						if(il->balance[id] == excl[e1]) { bdel = 0; break; }
					}
							
					if(bdel) RBDelete(d1,n);
					//~ norm -= pow((double)(il->balance[id]),aa[0]);
				}
			}
			
			//hozzáadás
			if(rout.value > 0) {
				il->balance[id] += rout.value;
				if(il->balance[id] > thres) {
					int badd = 1;
					int64_t b2 = il->balance[id];
					unsigned int e1;
					if(nexcl) for(e1=0;e1<nexcl;e1++) {
						if(b2 == excl[e1]) { badd = 0; break; }
					}
					if(!badd) {
						rb_red_blk_node* n = RBExactQuery(d1,(void*)b2);
						if(!n) badd = 1;
					}
					
					if(badd) {
						RBTreeInsert(d1,(void*)b2,0);
						//~ norm += pow((double)b2,aa[0]);
					}
				}
			}
			
			txout++;
		}
		
		r = record_read(out,&rout);
		if(r != 0) { //a fájl végére értünk
			if(zip) pclose_noshell(&pc2);
			else fclose(out);
			out = 0;
			r = 0;
			break;
		}
		
		//új kimeneti fájlok megnyitása, ha külön írjuk ki
		if(filesdelay) {
			if(timestamp > fdnext) {
				outf_close(out1,na,gzip,p1);
				seq++;
				if(ouf_open(out1,na,gzip,fout2,aa,buf2,seq,p1)) {
					fprintf(stderr,"Hiba a kimeneti fájlok megnyitásakor!\n");
					r = 4;
					goto pt6_end;
				}
				fdnext += filesdelay;
			}
		}
		
		if(DE1) if(txout >= DENEXT) {
			fprintf(stderr,"%u kimenet feldolgozva\n",txout);
			DENEXT = txout + DE1;
		}
	}
	
pt6_end:
	if(in) {
		if(zip) pclose_noshell(&pc1);
		else fclose(in);
	}
	if(out) {
		if(zip) pclose_noshell(&pc2);
		else fclose(out);
	}
	outf_close(out1,na,gzip,p1);
	
	free(out1);
	free(p1);
	free(buf2[0]);
	free(buf2[1]);
	
	fprintf(stdout,"feldolgozott bemenetek: %u, kimenetek: %u\n",txin,txout);
	
	
	time_t t2 = time(0);
	fprintf(stdout,"futásidő: %u\n",(unsigned int)(t2-t1));
	
	
	ids_free(il);
	RBTreeDestroy(d1);
	
	return r;
}

/*
start -Z -i data2/txint2.gz -o data2/txoutt2.gz -d data2/addrids.gz -N 13086528 -a 0.5 1.0 1.5 -O2 ptb/addr1

*/

