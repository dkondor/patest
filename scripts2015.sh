./pt6r -e txedge10m.gz -i addrids.gz -N 64605039 -O2 out1/a3r.out -l links.gz -Z -a 1.0 -d 10y
futásidő: 185
futásidő: 185 (adatok beolvasása: 154, feldolgozás: 31)
=> 31*50 = 1500 -> + 154 -> ~1700 s -> *11 -> 18700

./pt62 -e txedge10m.gz -i addrids.gz -N 64605039 -O2 out1/a32.out -l links.gz -Z -a 0.0 0.25 0.5 0.7 0.8 0.9 1.0 1.1 1.2 1.3 1.5 -d 10y
feldolgozott élek: 9692263, tranzakciók: 9692263
futásidő: 630 (adatok beolvasása: 153, feldolgozás: 477)
=> 50*477 = 23850

nem nagy különbség; cikkben 6/8
0.7 0.85 1.0 1.1 1.2 1.35
+ 0.0 0.25 0.5 1.5 -> ez csak 10, az rbtree változat gyorsabb + az scc-s esetben lehet többet is futtatni egyszerre

futtatások:
10y: semmi és -0 (-1 és -0 ugyanazt csinálja itt) + -On kapcsoló: pontok első tranzakciója külön is
1m: semmi, -1, -0 -> 3 fajta
1d: itt is 3 fajta

3+3+2 szkript:

pt6r10y2.sh
#!/bin/sh
for a in 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5
do
./pt6r -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r10y2 -On -l links.gz -Z -a $a -d 10y -D 5000000 >> logs/pt6r10y2.log 2>&1
done

pt6r10y0.sh
#!/bin/sh
for a in 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5
do
./pt6r -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r10y0 -0 -On -l links.gz -Z -a $a -d 10y -D 5000000 >> logs/pt6r10y0.log 2>&1
done

pt6r1m2.sh
#!/bin/sh
for a in 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5
do
./pt6r -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1m2 -On -l links.gz -Z -a $a -d 1m -D 5000000 >> logs/pt6r1m2.log 2>&1
done

pt6r1m1.sh
#!/bin/sh
for a in 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5
do
./pt6r -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1m1 -1 -On -l links.gz -Z -a $a -d 1m -D 5000000 >> logs/pt6r1m1.log 2>&1
done

pt6r1m0.sh
#!/bin/sh
for a in 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5
do
./pt6r -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1m0 -0 -On -l links.gz -Z -a $a -d 1m -D 5000000 >> logs/pt6r1m0.log 2>&1
done

pt6r1d2.sh
#!/bin/sh
for a in 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5
do
./pt6r -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1d2 -On -l links.gz -Z -a $a -d 1d -D 5000000 >> logs/pt6r1d2.log 2>&1
done

pt6r1d1.sh
#!/bin/sh
for a in 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5
do
./pt6r -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1d1 -1 -On -l links.gz -Z -a $a -d 1d -D 5000000 >> logs/pt6r1d1.log 2>&1
done

pt6r1d0.sh
#!/bin/sh
for a in 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5
do
./pt6r -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1d0 -0 -On -l links.gz -Z -a $a -d 1d -D 5000000 >> logs/pt6r1d0.log 2>&1
done


# ezeket futtató 3 szkript
pt6r10y.sh
#!/bin/sh
./pt6r10y0.sh &
./pt6r10y2.sh &
wait

pt6r1d.sh
#!/bin/sh
./pt6r1d0.sh &
./pt6r1d1.sh &
./pt6r1d2.sh &
wait

pt6r1m.sh
#!/bin/sh
./pt6r1m0.sh &
./pt6r1m1.sh &
./pt6r1m2.sh &
wait


################################################################################################################
# új változat: számolás egyszerre sok kitevőre, de egyszerre csak egy program futtatása, nem fér be a memóriába


pt6r10y2.sh
#!/bin/sh
./pt6r2 -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r10y2 -On -l links.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 10y -D 5000000 > logs/pt6r10y2.log 2>&1

pt6r10y0.sh
#!/bin/sh
./pt6r2 -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r10y0 -0 -On -l links.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 10y -D 5000000 > logs/pt6r10y0.log 2>&1

pt6r1m2.sh
#!/bin/sh
./pt6r2 -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1m2 -On -l links.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 1m -D 5000000 > logs/pt6r1m2.log 2>&1

pt6r1m1.sh
#!/bin/sh
./pt6r2 -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1m1 -1 -On -l links.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 1m -D 5000000 > logs/pt6r1m1.log 2>&1

pt6r1m0.sh
#!/bin/sh
./pt6r2 -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1m0 -0 -On -l links.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 1m -D 5000000 > logs/pt6r1m0.log 2>&1

pt6r1d2.sh
#!/bin/sh
./pt6r2 -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1d2 -On -l links.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 1d -D 5000000 > logs/pt6r1d2.log 2>&1

pt6r1d1.sh
#!/bin/sh
./pt6r2 -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1d1 -1 -On -l links.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 1d -D 5000000 > logs/pt6r1d1.log 2>&1

pt6r1d0.sh
#!/bin/sh
./pt6r2 -e txedge.gz -i addrids.gz -N 64605039 -O2 out1/pt6r1d0 -0 -On -l links.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 1d -D 5000000 > logs/pt6r1d0.log 2>&1




###########################################################################
## feldolgozás
## minta az eloszlásokból
for a in 0.70 0.85 1.00 1.10 1.20 1.35 0.00 0.25 0.50 1.50
do
for n in 0 2
do
zcat out1/pt6r10y$n-$a* | sort -g -S 16g | awk '{if(NR%400000==0) print $0}' | gzip -c > out2/pt6r10y$n-$a.gz
echo $a $n
done
done



############################################################################
# plot
se xr [0:204728]
se ke l
p 'pt6r10y0-0.00.dat' u 0:1 w l t '0.0'
rep 'pt6r10y0-0.50.dat' u 0:1 w l t '0.5'
rep 'pt6r10y0-0.70.dat' u 0:1 w l t '0.7'
rep 'pt6r10y0-0.85.dat' u 0:1 w l t '0.85'
rep 'pt6r10y0-1.00.dat' u 0:1 w l t '1.00'
rep 'pt6r10y0-1.10.dat' u 0:1 w l t '1.10'
rep 'pt6r10y0-1.20.dat' u 0:1 w l t '1.20'
rep 'pt6r10y0-1.35.dat' u 0:1 w l t '1.35'
rep 'pt6r10y0-1.50.dat' u 0:1 w l t '1.50'
se te png size 800,600
se out 'pt6r10y0.png'
rep
se out
se te wxt

se xr [0:238617]
p 'pt6r10y2-0.00.dat' u 0:1 w l t '0.0'
rep 'pt6r10y2-0.25.dat' u 0:1 w l t '0.25'
rep 'pt6r10y2-0.50.dat' u 0:1 w l t '0.5'
rep 'pt6r10y2-0.70.dat' u 0:1 w l t '0.7'
rep 'pt6r10y2-0.85.dat' u 0:1 w l t '0.85'
rep 'pt6r10y2-1.00.dat' u 0:1 w l t '1.00'
rep 'pt6r10y2-1.10.dat' u 0:1 w l t '1.10'
rep 'pt6r10y2-1.20.dat' u 0:1 w l t '1.20'
rep 'pt6r10y2-1.35.dat' u 0:1 w l t '1.35'
rep 'pt6r10y2-1.50.dat' u 0:1 w l t '1.50'
se te png size 800,600
se out 'pt6r10y2.png'
rep
se out
se te wxt


se xr [0:204728]
se ke l
p 'pt6r1m0-0.00.dat' u 0:1 w l t '0.0'
rep 'pt6r1m0-0.25.dat' u 0:1 w l t '0.25'
rep 'pt6r1m0-0.50.dat' u 0:1 w l t '0.5'
rep 'pt6r1m0-0.70.dat' u 0:1 w l t '0.7'
rep 'pt6r1m0-0.85.dat' u 0:1 w l t '0.85'
rep 'pt6r1m0-1.00.dat' u 0:1 w l t '1.00'
rep 'pt6r1m0-1.10.dat' u 0:1 w l t '1.10'
rep 'pt6r1m0-1.20.dat' u 0:1 w l t '1.20'
rep 'pt6r1m0-1.35.dat' u 0:1 w l t '1.35'
rep 'pt6r1m0-1.50.dat' u 0:1 w l t '1.50'
se te png size 800,600
se out 'pt6r1m0.png'
rep
se out
se te wxt

se xr [0:205525]
se ke l
p 'pt6r1m1-0.00.dat' u 0:1 w l t '0.0'
rep 'pt6r1m1-0.25.dat' u 0:1 w l t '0.25'
rep 'pt6r1m1-0.50.dat' u 0:1 w l t '0.5'
rep 'pt6r1m1-0.70.dat' u 0:1 w l t '0.7'
rep 'pt6r1m1-0.85.dat' u 0:1 w l t '0.85'
rep 'pt6r1m1-1.00.dat' u 0:1 w l t '1.00'
rep 'pt6r1m1-1.10.dat' u 0:1 w l t '1.10'
rep 'pt6r1m1-1.20.dat' u 0:1 w l t '1.20'
rep 'pt6r1m1-1.35.dat' u 0:1 w l t '1.35'
rep 'pt6r1m1-1.50.dat' u 0:1 w l t '1.50'
se te png size 800,600
se out 'pt6r1m1.png'
rep
se out
se te wxt

se xr [0:238617]
p 'pt6r1m2-0.00.dat' u 0:1 w l t '0.0'
rep 'pt6r1m2-0.25.dat' u 0:1 w l t '0.25'
rep 'pt6r1m2-0.50.dat' u 0:1 w l t '0.5'
rep 'pt6r1m2-0.70.dat' u 0:1 w l t '0.7'
rep 'pt6r1m2-0.85.dat' u 0:1 w l t '0.85'
rep 'pt6r1m2-1.00.dat' u 0:1 w l t '1.00'
rep 'pt6r1m2-1.10.dat' u 0:1 w l t '1.10'
rep 'pt6r1m2-1.20.dat' u 0:1 w l t '1.20'
rep 'pt6r1m2-1.35.dat' u 0:1 w l t '1.35'
rep 'pt6r1m2-1.50.dat' u 0:1 w l t '1.50'
se te png size 800,600
se out 'pt6r1m2.png'
rep
se out
se te wxt


se xr [0:204728]
se ke l
p 'pt6r1d0-0.00.dat' u 0:1 w l t '0.0'
rep 'pt6r1d0-0.25.dat' u 0:1 w l t '0.25'
rep 'pt6r1d0-0.50.dat' u 0:1 w l t '0.5'
rep 'pt6r1d0-0.70.dat' u 0:1 w l t '0.7'
rep 'pt6r1d0-0.85.dat' u 0:1 w l t '0.85'
rep 'pt6r1d0-1.00.dat' u 0:1 w l t '1.00'
rep 'pt6r1d0-1.10.dat' u 0:1 w l t '1.10'
rep 'pt6r1d0-1.20.dat' u 0:1 w l t '1.20'
rep 'pt6r1d0-1.35.dat' u 0:1 w l t '1.35'
rep 'pt6r1d0-1.50.dat' u 0:1 w l t '1.50'
se te png size 800,600
se out 'pt6r1d0.png'
rep
se out
se te wxt

se xr [0:216588]
se ke l
p 'pt6r1d1-0.00.dat' u 0:1 w l t '0.0'
rep 'pt6r1d1-0.25.dat' u 0:1 w l t '0.25'
rep 'pt6r1d1-0.50.dat' u 0:1 w l t '0.5'
rep 'pt6r1d1-0.70.dat' u 0:1 w l t '0.7'
rep 'pt6r1d1-0.85.dat' u 0:1 w l t '0.85'
rep 'pt6r1d1-1.00.dat' u 0:1 w l t '1.00'
rep 'pt6r1d1-1.10.dat' u 0:1 w l t '1.10'
rep 'pt6r1d1-1.20.dat' u 0:1 w l t '1.20'
rep 'pt6r1d1-1.35.dat' u 0:1 w l t '1.35'
rep 'pt6r1d1-1.50.dat' u 0:1 w l t '1.50'
se te png size 800,600
se out 'pt6r1d1.png'
rep
se out
se te wxt

se xr [0:238617]
p 'pt6r1d2-0.00.dat' u 0:1 w l t '0.0'
rep 'pt6r1d2-0.25.dat' u 0:1 w l t '0.25'
rep 'pt6r1d2-0.50.dat' u 0:1 w l t '0.5'
rep 'pt6r1d2-0.70.dat' u 0:1 w l t '0.7'
rep 'pt6r1d2-0.85.dat' u 0:1 w l t '0.85'
rep 'pt6r1d2-1.00.dat' u 0:1 w l t '1.00'
rep 'pt6r1d2-1.10.dat' u 0:1 w l t '1.10'
rep 'pt6r1d2-1.20.dat' u 0:1 w l t '1.20'
rep 'pt6r1d2-1.35.dat' u 0:1 w l t '1.35'
rep 'pt6r1d2-1.50.dat' u 0:1 w l t '1.50'
se te png size 800,600
se out 'pt6r1d2.png'3
rep
se out
se te wxt





###########################################################################
## SCC-kre számolás, a grail12-n
cd /media/data3/dkondor/bitcoin/patest2015/sccs
set DATA /media/data2/bitcoin/patest2/data2015
~/bitcoin/patest/patest2/pt6r2 -e $DATA/scctxedge.gz -i $DATA/sccids.gz -N 26634104 -O2 out1/pt6r10y2 -On -l $DATA/scclinks.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 10y -D 5000000 -F 6m > logs/pt6r10y2.log 2>&1

~/bitcoin/patest/patest2/pt6r2 -e $DATA/scctxedge.gz -i $DATA/sccids.gz -N 26634104 -O2 out1/pt6r10y0 -0 -On -l $DATA/scclinks.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 10y -D 5000000 -F 6m > logs/pt6r10y0.log 2>&1


for a in 1m 1d
~/bitcoin/patest/patest2/pt6r2 -e $DATA/scctxedge.gz -i $DATA/sccids.gz -N 26634104 -O2 out1/pt6r"$a"0 -0 -On -l $DATA/scclinks.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d $a -D 5000000 -F 6m > logs/pt6r"$a"0.log 2>&1
~/bitcoin/patest/patest2/pt6r2 -e $DATA/scctxedge.gz -i $DATA/sccids.gz -N 26634104 -O2 out1/pt6r"$a"1 -1 -On -l $DATA/scclinks.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d $a -D 5000000 -F 6m > logs/pt6r"$a"1.log 2>&1
~/bitcoin/patest/patest2/pt6r2 -e $DATA/scctxedge.gz -i $DATA/sccids.gz -N 26634104 -O2 out1/pt6r"$a"2 -On -l $DATA/scclinks.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d $a -D 5000000 -F 6m > logs/pt6r"$a"2.log 2>&1
echo $a
end

## megjegyzés: 10y változatok nem futottak le, futtatás az új verzióval (popen_noshell-t és zip-et használva):
~/bitcoin/patest/patest2/pt6r3 -e $DATA/scctxedge.gz -i $DATA/sccids.gz -N 26634104 -O2 out2/pt6r10y0 -0 -On -l $DATA/scclinks.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 10y -D 5000000 -F 6m > out2/pt6r10y0.log 2>&1
~/bitcoin/patest/patest2/pt6r3 -e $DATA/scctxedge.gz -i $DATA/sccids.gz -N 26634104 -O2 out2/pt6r10y2 -On -l $DATA/scclinks.gz -Z -a 0.7 0.85 1.0 1.1 1.2 1.35 0.0 0.25 0.5 1.5 -d 10y -D 5000000 -F 6m > out2/pt6r10y2.log 2>&1


for a in 0.70 0.85 1.00 1.10 1.20 1.35 0.00 0.25 0.50 1.50
for n in 0 2
for b in out2/pt6r10y$n-$a*
unzip -p $b
end | ~/bitcoin/patest/patest2/fit1 -S -n -o out3/kmtest/pt6r10y$n-$a | awk '{if(NR%400000==0) print $0}' > out3/pt6r10y$n-$a.dat
echo $a $n
end
end

for c in 1m 1d
for a in 0.70 0.85 1.00 1.10 1.20 1.35 0.00 0.25 0.50 1.50
for n in 0 2
zcat out1/pt6r"$c"$n-$a* | ~/bitcoin/patest/patest2/fit1 -S -n -o out3/kmtest/pt6r"$c"$n-$a | awk '{if(NR%400000==0) print $0}' > out3/pt6r"$c"$n-$a.dat
echo $c $a $n
end
end
end

for c in 1m 1d
for a in 0.70 0.85 1.00 1.10 1.20 1.35 0.00 0.25 0.50 1.50
set n 1
zcat out1/pt6r"$c"$n-$a* | ~/bitcoin/patest/patest2/fit1 -s $a -n -o out3/kmtest/pt6r"$c"$n.dat2
echo $c $a $n
end
end

for c in 1m 1d
for a in 0.70 0.85 1.00 1.10 1.20 1.35 0.00 0.25 0.50 1.50
set n 1
zcat out1/pt6r"$c"$n-$a* | ~/bitcoin/patest/patest2/fit1 -n -o out3/kmtest/pt6r$c$n-$a
echo $c $a $n
end
end

cd out3
for c in 1m 1d 10y
for n in 0 2
for a in 0.70 0.85 1.00 1.10 1.20 1.35 0.00 0.25 0.50 1.50
echo $a\t(fgrep ksmax01 pt6r$c$n-$a | cut -f 3 -d ' ') >> pt6r$c$n.dat01
echo $c $n $a
end
end
end

for c in 1m 1d
set n 1
cut -f 1,6 pt6r"$c"$n.dat2 > pt6r"$c"$n.dat
end

for c in 1m 1d
set n 1
cut -f 1,5 pt6r"$c"$n.dat2 > pt6r"$c"$n.dat01
end



cd ..
# nem jó, túl kevés sor
for a in 0.70 0.85 1.00 1.10 1.20 1.35 0.00 0.25 0.50 1.50
for n in 0 2
for b in out2/pt6r10y$n-$a*
unzip -p $b
end | qsd | awk '{if(NR%140==0) print $0}' > out3/pt6r10y$n-$a.dat
echo $a $n
end
end

for c in 1m 1d
for a in 0.70 0.85 1.00 1.10 1.20 1.35 0.00 0.25 0.50 1.50
for n in 0 1 2
zcat out1/pt6r"$c"$n-$a* | qsd | awk '{if(NR%140==0) print $0}' > out3/pt6r"$c"$n-$a.dat
echo $c $a $n
end
end
end

# túl sok sor
cd out3
set n1 0 2
set r1 4 8
for c in 1m 1d 10y
for a in 0.70 0.85 1.00 1.10 1.20 1.35 0.00 0.25 0.50 1.50
for i in 1 2
mv pt6r$c$n1[$i]-$a.dat /tmp
awk "{if(NR%$r1[$i]==0) print \$0}" /tmp/pt6r$c$n1[$i]-$a.dat > pt6r$c$n1[$i]-$a.dat
rm /tmp/pt6r$c$n1[$i]-$a.dat
echo $c $a $n1[$i]
end
end
end



#####################################################################
# KS-távolságok számítása (fit1) az eredeti eredményekre (nem SCC-k)
cd /media/data3/dkondor/bitcoin/patest2015/out1
mkdir kmtest
for c in 10y 1m 1d
  if [ $c = 10y ]
    set i1 0 2
  else
    set i1 0 1 2
  end
  for n in $i1
    for a in 0.70 0.85 1.00 1.10 1.20 1.35 0.00 0.25 0.50 1.50
      zcat pt6r$c"$n"-$a* | ~/bitcoin/patest/patest2/fit1 -n -o kmtest/pt6r"$c"$n.out -s $a
      echo $c $n $a
    end
  end
end


#####################################################################
# plotokat létrehozó script
# összes ábra az összehasonlításokhoz
cd /media/data3/dkondor/bitcoin/patest2015/out2
set a1 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
set colors 5aa732 e75216 009ace ffd500 e78502 db6aa2 007939 8671a7 005c96 815234 9a2846
set colors 5aa732 005c96 e75216 009ace ffd500 8671a7 e78502 db6aa2 007939 9a2846 815234
echo "se ke off"
for c in 10y 1m 1d
  if [ $c = 10y ]
    set i1 0 2
  else
    set i1 0 1 2
  end
  for n in $i1
    echo "# $c $n"
    set ntotal (cat pt6r$c"$n"-1.00.dat | wc -l)
    set nzero (awk '{if($1 > 0) {print NR;exit;}}' pt6r$c"$n"-1.00.dat)
    echo "se yr [0:1]"
    echo "se xr [0:1]"
    set a $a1[1]
    set c1 \#$colors[1]
    echo "p 'pt6r$c$n-$a.dat' u (\$0/$ntotal.0):1 w l lw 2 lc rgbcolor '$c1' t 'a = $a'"
    for j in (seq 2 10)
      echo "rep 'pt6r$c$n-$a1[$j].dat' u (\$0/$ntotal.0):1 w l lw 2 lc rgbcolor '#$colors[$j]' t 'a = $a1[$j]'"
    end
    echo "rep (x-$nzero.0/$ntotal.0)/(1.0-$nzero.0/$ntotal.0) w l lw 2 lc -1 not"
    echo "se te post eps color solid size 3,2"
    echo "se out 'pt6r$c$n.eps'"\nrep\nse out\nse te wxt
    echo "!epstopdf pt6r$c$n.eps"
    echo "!convert -density 200 pt6r$c$n.eps -bordercolor white -border 0x0 pt6r$c$n.png"
    echo \n\n
  end
end

# inset-ek
echo "unse ytics"
echo "se y2tics 0.4"
echo "se xtics 0.5"
for c in 10y 1m 1d
  if [ $c = 10y ]
    set i1 0 2
  else
    set i1 0 1 2
  end
  for n in $i1
      echo "p '< sort ../out1/kmtest/pt6r$c$n.out' u 1:6 axes x1y2 w lp lw 3 pt 5 not"
      echo "se te post eps color solid size 1.2,0.7"
      echo "se out 'pt6r$c$n"inset".eps'"
      echo rep\nse out\nse te wxt
      echo "!epstopdf pt6r$c$n"inset".eps"
    echo "!convert -density 200 pt6r$c$n"inset".eps -bordercolor white -border 0x0 pt6r$c$n"inset".png"
    echo \n\n
  end
end


###############################################################
## ugyanezek az SCC-kre
cd /media/data3/dkondor/bitcoin/patest2015/sccs/out3
set a1 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
set colors 5aa732 e75216 009ace ffd500 e78502 db6aa2 007939 8671a7 005c96 815234 9a2846
set colors 5aa732 005c96 e75216 009ace ffd500 8671a7 e78502 db6aa2 007939 9a2846 815234
echo "se ke off"
for c in 10y 1m 1d
  if [ $c = 10y ]
    set i1 0 2
  else
    set i1 0 1 2
  end
  for n in $i1
    echo "# $c $n"
    set ntotal (cat pt6r$c"$n"-1.00.dat | wc -l)
    set nzero (awk '{if($1 > 0) {print NR;exit;}}' pt6r$c"$n"-1.00.dat)
    echo "se yr [0:1]"
    echo "se xr [0:1]"
    set a $a1[1]
    set c1 \#$colors[1]
    echo "p 'pt6r$c$n-$a.dat' u (\$0/$ntotal.0):1 w l lw 2 lc rgbcolor '$c1' t 'a = $a'"
    for j in (seq 2 10)
      echo "rep 'pt6r$c$n-$a1[$j].dat' u (\$0/$ntotal.0):1 w l lw 2 lc rgbcolor '#$colors[$j]' t 'a = $a1[$j]'"
    end
    echo "rep (x-$nzero.0/$ntotal.0)/(1.0-$nzero.0/$ntotal.0) w l lw 2 lc -1 not"
    echo "se te post eps color solid size 3,2"
    echo "se out 'ptscc6r$c$n.eps'"\nrep\nse out\nse te wxt
    echo "!epstopdf ptscc6r$c$n.eps"
    echo "!convert -density 200 ptscc6r$c$n.eps -bordercolor white -border 0x0 ptscc6r$c$n.png"
    echo \n\n
  end
end

# inset-ek
echo "unse ytics"
echo "se y2tics 0.4"
echo "se xtics 0.5"
for c in 10y 1m 1d
  if [ $c = 10y ]
    set i1 0 2
  else
    set i1 0 1 2
  end
  for n in $i1
      echo "p '< sort kmtest/pt6r$c$n.dat' u 1:2 axes x1y2 w lp lw 3 pt 5 not"
      echo "se te post eps color solid size 1.2,0.7"
      echo "se out 'ptscc6r$c$n"inset".eps'"
      echo rep\nse out\nse te wxt
      echo "!epstopdf ptscc6r$c$n"inset".eps"
    echo "!convert -density 200 ptscc6r$c$n"inset".eps -bordercolor white -border 0x0 ptscc6r$c$n"inset".png"
    echo \n\n
  end
end



####################################################################
## CDF/R ábrák, az alpha = 1.00-ra illesztett egyenest is belevéve
## sorok száma: n=0: 140*4 = 560, n=1: 800, n=2: 140*8 = 1120
set a1 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
set colors 5aa732 e75216 009ace ffd500 e78502 db6aa2 007939 8671a7 005c96 815234 9a2846
set colors 5aa732 005c96 e75216 009ace ffd500 8671a7 e78502 db6aa2 007939 9a2846 815234
echo "se ke off"
for c in 10y 1m 1d
  if [ $c = 10y ]
    set i1 0 2
  else
    set i1 0 1 2
  end
  for n in $i1
    echo "# $c $n"
    set ntotal (cat pt6r$c"$n"-1.00.dat | wc -l)
    set nzero (awk '{if($1 > 0) {print NR;exit;}}' pt6r$c"$n"-1.00.dat)
    if [ $n = 0 ]
      set norig 560
    else
      if [ $n = 1 ]
        set norig 800
      else
        set norig 1120
      end
    end 
    echo "se yr [0:1]"
    echo "se xr [0:1]"
    set a $a1[1]
    set c1 \#$colors[1]
    echo "p 'pt6r$c$n-$a.dat' u (\$0/$ntotal.0):1 w l lw 2 lc rgbcolor '$c1' t 'a = $a'"
    for j in (seq 2 10)
      echo "rep 'pt6r$c$n-$a1[$j].dat' u (\$0/$ntotal.0):1 w l lw 2 lc rgbcolor '#$colors[$j]' t 'a = $a1[$j]'"
    end
    echo "rep (x-$nzero.0/$ntotal.0)/(1.0-$nzero.0/$ntotal.0) w l lw 2 lc -1 not"
    if [ $c = 10y ]
      set a2 1.00
    else
      if [ $c = 1m ]
        set a2 1.10
      else
        set a2 1.20
      end
    end
    set a (grep '^a =' kmtest/pt6r$c$n-$a2 | cut -f 3 -d ' ')
    set b (grep '^b =' kmtest/pt6r$c$n-$a2 | cut -f 3 -d ' ')
    echo "rep $ntotal*$norig*$a*(x-$nzero.0/$ntotal.0)+$b w l lw 2 lc -1 not"
    echo "se te post eps color solid size 3,2"
    echo "se out 'ptscc6r$c$n.eps'"\nrep\nse out\nse te wxt
    echo "!epstopdf ptscc6r$c$n.eps"
    echo "!convert -density 200 ptscc6r$c$n.eps -bordercolor white -border 0x0 ptscc6r$c$n.png"
    echo \n\n
  end
end

# inset-ek -> inkább külön, nagyobb méretben
echo "se auto xy"
for c in 10y 1m 1d
  if [ $c = 10y ]
    set i1 0 2
  else
    set i1 0 1 2
  end
  for n in $i1
      echo "p '< sort kmtest/pt6r$c$n.dat' u 1:2 w lp lw 3 pt 5 not"
      echo "rep '< sort kmtest/pt6r$c$n.dat01' u 1:2 w lp lw 3 pt 5 not"
      echo "se te post eps color solid size 2,1.4"
      echo "se out 'ptscc6r$c$n"inset".eps'"
      echo rep\nse out\nse te wxt
      echo "!epstopdf ptscc6r$c$n"inset".eps"
    echo "!convert -density 200 ptscc6r$c$n"inset".eps -bordercolor white -border 0x0 ptscc6r$c$n"inset".png"
    echo \n\n
  end
end





#######################################################################
## tesztelés vagyonokra
/media/data3/dkondor/bitcoin/patest2015/money
with a as (select txid, addr, sum(value) as value from txin group by txid, addr)
select addr, value, btime from a join tx on a.txid = tx.txid join blockhash bh on tx.blockid = bh.blockid
order by a.txid;
go -h -f | sed -e 's/  */\t/g' -e 's/^\t//' | gzip -c > txin.gz

with a as (select txid, addr, sum(value) as value from txout group by txid, addr)
select addr, value, btime from a join tx on a.txid = tx.txid join blockhash bh on tx.blockid = bh.blockid
order by a.txid;
go -h -f | sed -e 's/  */\t/g' -e 's/^\t//' | gzip -c > txout.gz

mkdir ptbr
for a in 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
/home/dani/bitcoin/patest/patest2/ptbr -Z -i txin.gz -o txout.gz -d /media/data2/bitcoin/patest2/data2015/addrids.gz -N 64605039 -a $a -O2 ptbr/addr1
echo $a
end


## feldolgozás (illesztés, ábrák)
cd ptbr
mkdir out3
for a in 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
zcat addr1-$a-0.dat.gz | awk '{print $3,$1}' | ~/bitcoin/patest/patest2/cdft | /home/dani/bitcoin/patest/patest2/fit1c -min 0.05 -max 0.95 -s $a -S -o out3/errksc.dat | awk '{if(NR%800==0) print $0}' > out3/sample-$a
echo $a
end
## nem jó, az ábrákhoz az összes adatból kellene minta
rm out3/sample-*
for a in 0.00 0.25 0.50 0.70 0.85
zcat addr1-$a-0.dat.gz | awk '{print $3,$1}' | ~/bitcoin/patest/patest2/cdft | awk '{if(NR%800==0) print $0}' > out3/sample-$a
echo $a
end
# 1.00 1.10 1.20 1.35 1.50 külön

# ábra
set a1 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
set colors 5aa732 005c96 e75216 009ace ffd500 8671a7 e78502 db6aa2 007939 9a2846 815234
echo "se ke off"
echo "se yr [0:1]"
echo "se xr [0:1]"
set n1 2.30034e+16
set n1 (tail -n 1 sample-$a1[1] | cut -f 1)
echo "p 'sample-$a1[1]' u (\$1/$n1.0):2 w l lw 2 lc rgbcolor '#$colors[1]' t 'a = $a1[1]'"
for j in (seq 2 10)
  set n1 (tail -n 1 sample-$a1[$j] | cut -f 1)
  echo "rep 'sample-$a1[$j]' u (\$1/$n1.0):2 w l lw 2 lc rgbcolor '#$colors[$j]' t 'a = $a1[$j]'"
end
echo "se te post eps color solid size 5,3"
echo "se out 'addrbalct3.eps'"\nrep\nse out\nse te wxt
echo "!epstopdf addrbalct3.eps"
echo "!convert -density 200 addrbalct3.eps -bordercolor white -border 0x0 addrbalct3.png"


##################################################
## új változat, 50 BTC és 25 BTC egyenlegek kihagyása a vagyok eloszlásból
cd /media/data3/dkondor/bitcoin/patest2015/money

mkdir ptbr2
for a in 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
/home/dani/bitcoin/patest/patest2/ptbr -Z -i txin.gz -o txout.gz -d /media/data2/bitcoin/patest2/data2015/addrids.gz -N 64605039 -a $a -O2 ptbr2/addr1 -e 50B 25B
echo $a
end

## új program, összes exponensre számolás egyszerre
/home/dani/bitcoin/patest/patest2/ptbr2 -Z -i txin.gz -o txout.gz -d /media/data2/bitcoin/patest2/data2015/addrids.gz -N 64605039 -a 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50 -O2 ptbr2/addr1 -e 50B 25B

## feldolgozás a fenti scriptekkel


## zip kimenetek feldolgozása
cd ptbr2
for a in 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
mkfifo /tmp/ptbr2.fifo
awk '{if(NR%800==0) print $0}' /tmp/ptbr2.fifo > out3/sample-$a &
unzip -p addr1-$a-0-1.dat.zip | awk '{print $3,$1}' | ~/bitcoin/patest/patest2/cdft | tee -a /tmp/ptbr2.fifo | /home/dani/bitcoin/patest/patest2/fit1c -min 0.05 -max 0.95 -s $a -o out3/errksc.dat
rm /tmp/ptbr2.fifo
echo $a
end

## ábra: elrendezés: fő ábra és az "inset" egymás mellett
set a1 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
set colors 5aa732 005c96 e75216 009ace ffd500 8671a7 e78502 db6aa2 007939 9a2846 815234
echo "se ke off"
echo "se yr [0:1]"
echo "se xr [0:1]"
set n1 (tail -n 1 sample-$a1[1] | cut -f 1)
echo "p 'sample-$a1[1]' u (\$1/$n1.0):2 w l lw 2 lc rgbcolor '#$colors[1]' t 'a = $a1[1]'"
for j in (seq 2 10)
  set n1 (tail -n 1 sample-$a1[$j] | cut -f 1)
  echo "rep 'sample-$a1[$j]' u (\$1/$n1.0):2 w l lw 2 lc rgbcolor '#$colors[$j]' t 'a = $a1[$j]'"
end
echo "se te post eps color solid size 4,3"
echo "se out 'addrbalct3.eps'"\nrep\nse out\nse te wxt
echo "!epstopdf addrbalct3.eps"
echo "!convert -density 200 addrbalct3.eps -bordercolor white -border 0x0 addrbalct3.png"

se auto xy
se xtics 0.5
se ytics 0.4
p 'errksc.dat' u 1:6 w lp lw 2 pt 5 ps 1.6 not
se te post eps color solid size 2,1.5
se out 'errksc.eps'
rep
se out
se te wxt
!epstopdf errksc.eps
!convert -density 200 errksc.eps -bordercolor white -border 0x0 errksc.png


#################################################
## SCC-kre ugyanez
cd /media/data3/dkondor/bitcoin/patest2015/sccmoney
with a as (select txins.*, blockid from txins inner merge join tx on txins.txid = tx.txid)
select sccid, value, btime from blockhash bh inner hash join a on bh.blockid = a.blockid
order by a.txid;
go -h -f | sed -e 's/  */\t/g' -e 's/^\t//' | gzip -c > txin.gz

with a as (select txouts.*, blockid from txouts inner merge join tx on txouts.txid = tx.txid)
select sccid, value, btime from blockhash bh inner hash join a on bh.blockid = a.blockid
order by a.txid;
go -h -f | sed -e 's/  */\t/g' -e 's/^\t//' | gzip -c > txout.gz

mkdir ptbr
mkdir ptbr2
begin
for a in 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
/home/dani/bitcoin/patest/patest2/ptbr -Z -i txin.gz -o txout.gz -d /media/data2/bitcoin/patest2/data2015/sccids.gz -N 26634104 -a $a -O2 ptbr/addr1
echo ptbr $a
end
for a in 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
/home/dani/bitcoin/patest/patest2/ptbr -Z -i txin.gz -o txout.gz -d /media/data2/bitcoin/patest2/data2015/sccids.gz -N 26634104 -a $a -O2 ptbr2/addr1 -e 50B 25B
echo ptbr2 $a
end
end

/home/dani/bitcoin/patest/patest2/ptbr2 -Z -i txin.gz -o txout.gz -d /media/data2/bitcoin/patest2/data2015/sccids.gz -N 26634104 -a 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50 -O2 ptbr2/addr1 -e 50B 25B


# feldolgozás
mkdir ptbr/out3
mkdir ptbr2/out3
for b in ptbr ptbr2
for a in 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
zcat $b/addr1-$a-0.dat.gz | awk '{print $3,$1}' | ~/bitcoin/patest/patest2/cdft | /home/dani/bitcoin/patest/patest2/fit1c -min 0.05 -max 0.95 -s $a -o $b/out3/errksc.dat
echo $b fit $a
end
for a in 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
zcat $b/addr1-$a-0.dat.gz | awk '{print $3,$1}' | ~/bitcoin/patest/patest2/cdft | awk '{if(NR%800==0) print $0}' > $b/out3/sample-$a
echo $a
end
end


## zip kimenetek feldolgozása
cd ptbr2
for a in 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
mkfifo /tmp/ptbr2.fifo2
awk '{if(NR%800==0) print $0}' /tmp/ptbr2.fifo2 > out3/sample-$a &
unzip -p addr1-$a-0-1.dat.zip | awk '{print $3,$1}' | ~/bitcoin/patest/patest2/cdft | tee -a /tmp/ptbr2.fifo2 | /home/dani/bitcoin/patest/patest2/fit1c -min 0.05 -max 0.95 -s $a -o out3/errksc.dat
rm /tmp/ptbr2.fifo2
echo $a
end

## ábra: elrendezés: fő ábra és az "inset" egymás mellett
set a1 0.00 0.25 0.50 0.70 0.85 1.00 1.10 1.20 1.35 1.50
set colors 5aa732 005c96 e75216 009ace ffd500 8671a7 e78502 db6aa2 007939 9a2846 815234
echo "se ke off"
echo "se yr [0:1]"
echo "se xr [0:1]"
set n1 (tail -n 1 sample-$a1[1] | cut -f 1)
echo "p 'sample-$a1[1]' u (\$1/$n1.0):2 w l lw 2 lc rgbcolor '#$colors[1]' t 'a = $a1[1]'"
for j in (seq 2 10)
  set n1 (tail -n 1 sample-$a1[$j] | cut -f 1)
  echo "rep 'sample-$a1[$j]' u (\$1/$n1.0):2 w l lw 2 lc rgbcolor '#$colors[$j]' t 'a = $a1[$j]'"
end
echo "se te post eps color solid size 4,3"
echo "se out 'sccbalct3.eps'"\nrep\nse out\nse te wxt
echo "!epstopdf sccbalct3.eps"
echo "!convert -density 200 sccbalct3.eps -bordercolor white -border 0x0 sccbalct3.png"

se auto xy
se xtics 0.5
se ytics 0.2
p 'errksc.dat' u 1:6 w lp lw 2 pt 5 ps 1.6 not
se te post eps color solid size 2,1.5
se out 'errkscsccs.eps'
rep
se out
se te wxt
!epstopdf errkscsccs.eps
!convert -density 200 errkscsccs.eps -bordercolor white -border 0x0 errkscsccs.png



##############################################################
## egyéb statisztikák:
##   Figure 4.2: The growth of the Bitcoin network
##      címek > 0 (> 1) egyenleggel, 1 hetes időtartamokban aktív címek, árfolyam
##      -> "egyszerű", SQL-ben meg lehet kapni, illetve csak a tranzakció be-/kimenetekből
##      gini2.c -> > 0 egyenlegű címek
##      edgesnew/edgesnew.c -> 1 hetes időszakokban aktív címek száma
##
##   Figure 4.5: Gini-coefficients
##      gini2.c -> fokszámok, aggregált hálózat + vagyonok
##      edgesnew/edgesnew.c -> partially aggregated hálózat, fokszámok
##      
##   Figure 4.6: clustering coefficient + degree correlation coefficient
##      netevol/degcorr2.c -> aggregált hálózat
##      edgesnew/edgesnew.c -> partially aggregated hálózat
##

## futtatások:
##   1. gini2.c, 1 hetes időszak -> aggregált hálózat Gini-együtthatók (Fig 4.5a), > 1 egyenlegű címek (Fig. 4.2)
##      kell hozzá: txin, txout, txedge, ids
##   2. edgesnew/edgesnew.c 1 hetes binek és időszakok (degcorr és clustering nem) -> 1 hetes időszakokban aktív címek száma (Fig. 4.2)
##      kell hozzá: txedge
##   3. edgesnew/edgesnew.c 30 napos binek, 1 hetes időszakok, minden -> Fig. 4.5b: Gini-együtthatók a partially aggregated hálózatokban, Fig. 4.6b: clustering és degree correlation a partially aggregated hálózatokban
##      kell hozzá: txedge
##   4. netevol/degcorr2.c deaktiválás nélkül -> Fig. 4.6a aggregált hálózatban clustering és degree correlation
##      kell hozzá: txedge, links, ids


## b ábrák lehetnek a "robustness" részbe
## + ugyanezek a contracted hálózatra is, az is mehet a robustness részbe


#############################################################
# /media/data1/dani/bitcoin/miscruns2015
mkdir addr
mkdir sccs

mkdir addr/total
mkdir sccs/total
mkdir addr/partial
mkdir sccs/partial

## gini-hez minden él első előfordulása kell
cd /media/data2/bitcoin/patest2/data2015
set -x LC_ALL en_US.utf8
sqsh -G 7.0 -U 'VO\dkondor' -S future1.vo.elte.hu
select ain, aout, min(btime) from txedge e join tx t on e.txid = t.txid
join blockhash bh on t.blockid = bh.blockid group by ain,aout order by min(btime)
go -h -f | sed -e 's/  */\t/g' -e 's/^\t//' | gzip -c > txedgemint.gz
## !!
select sccin, sccout, min(btime) from txedges e join tx t on e.txid = t.txid
join blockhash bh on t.blockid = bh.blockid group by sccin, sccout order by min(btime)
go -h -f | sed -e 's/  */\t/g' -e 's/^\t//' | gzip -c > scctxedgemint.gz


## 1. Fig 4.5a, Fig 4.2
/home/dani/bitcoin/patest/patest2/gini2 -i /media/data3/dkondor/bitcoin/patest2015/money/txin.gz -o /media/data3/dkondor/bitcoin/patest2015/money/txout.gz -d /media/data2/bitcoin/patest2/data2015/addrids.gz -N 64605039 -e /media/data2/bitcoin/patest2/data2015/txedgemint.gz -b 2 -t 1d -z > addr/total/gini1.out
# -B csak a > 0 fokszámú pontokat veszi bele a vagyoneloszlásba; lehetne a vagyoneloszlás ábrákból is készíteni egy ilyen változatot
/home/dani/bitcoin/patest/patest2/gini2 -i /media/data3/dkondor/bitcoin/patest2015/money/txin.gz -o /media/data3/dkondor/bitcoin/patest2015/money/txout.gz -d /media/data2/bitcoin/patest2/data2015/addrids.gz -N 64605039 -e /media/data2/bitcoin/patest2/data2015/txedgemint.gz -b 2 -t 1d -z -B > addr/total/gini1B.out

/home/dani/bitcoin/patest/patest2/gini2 -i /media/data3/dkondor/bitcoin/patest2015/sccmoney/txin.gz -o /media/data3/dkondor/bitcoin/patest2015/sccmoney/txout.gz -d /media/data2/bitcoin/patest2/data2015/sccids.gz -N 26634104 -e /media/data2/bitcoin/patest2/data2015/scctxedgemint.gz -b 2 -t 1d -z > sccs/total/gini1.out
/home/dani/bitcoin/patest/patest2/gini2 -i /media/data3/dkondor/bitcoin/patest2015/sccmoney/txin.gz -o /media/data3/dkondor/bitcoin/patest2015/sccmoney/txout.gz -d /media/data2/bitcoin/patest2/data2015/sccids.gz -N 26634104 -e /media/data2/bitcoin/patest2/data2015/scctxedgemint.gz -b 2 -t 1d -z -B > sccs/total/gini1B.out


## 2. Fig 4.2
/home/dani/bitcoin/patest/patest2/edgesnew/en1 -i /media/data2/bitcoin/patest2/data2015/txedge.gz -z -D -C -t1 1w > addr/partial/nedges1w.out
# edges_read(): összesen 477235560 élt olvastam be
/home/dani/bitcoin/patest/patest2/edgesnew/en1 -i /media/data2/bitcoin/patest2/data2015/scctxedge.gz -z -D -C -t1 1w > sccs/partial/nedges1w.out
# edges_read(): összesen 119480444 élt olvastam be


## 3. Fig 4.5b, 4.6b
/home/dani/bitcoin/patest/patest2/edgesnew/en1 -i /media/data2/bitcoin/patest2/data2015/txedge.gz -z -t1 1m > addr/partial/edgesnew1m.out
/home/dani/bitcoin/patest/patest2/edgesnew/en1 -i /media/data2/bitcoin/patest2/data2015/scctxedge.gz -z -t1 1m > sccs/partial/edgesnew1m.out
# edges_read(): összesen 119480444 élt olvastam be



## 4. Fig 4.6a
/home/dani/bitcoin/patest/patest2/netevol/dc2 -e /media/data2/bitcoin/patest2/data2015/txedge.gz -l /media/data2/bitcoin/patest2/data2015/links.gz -d /media/data2/bitcoin/patest2/data2015/addrids.gz -N 64605039 -f 1w -Z -cf 1w -co addr/total/cl1.out -O addr/total/dc1.out
/home/dani/bitcoin/patest/patest2/netevol/dc2 -e /media/data2/bitcoin/patest2/data2015/scctxedge.gz -l /media/data2/bitcoin/patest2/data2015/scclinks.gz -d /media/data2/bitcoin/patest2/data2015/sccids.gz -N 26634104 -f 1w -Z -cf 1w -co sccs/total/cl1.out -O sccs/total/dc1.out

###############################################
## új netevol program tesztelése
cd /media/data2/bitcoin/patest2/data2015
/home/dani/bitcoin/patest/patest2/netevol/dc2 -e txedge2010.gz -l links2010.gz -d addr2010.gz -N 125035 -f 1w -Z -cf 1w -co netevoltest2010/cl1.out -O netevoltest2010/dc1.out



################################################################
## ábrák elkészítése
## Fig 4.2
cd /media/data1/dani/bitcoin/miscruns2015/addr/total
gnuplot
# set colors 5aa732 005c96 e75216 009ace ffd500 8671a7 e78502 db6aa2 007939 9a2846 815234

se xdata time
se timefmt '%s'
se xtics format '%Y'
se ke r b
se yr [10:]
se ytics nomirror
se y2tics
se y2r [0.1:]
se log y
se log y2
se xr ['1230768000':'1425168000']
se xl 'Time'
se yl '# of addresses'
se y2l 'Exchange price [USD/BTC]'
p '../partial/nedges1w.out' u 2:3 w l lw 3 lc rgbcolor '#e75216' t 'Weekly active addresses'
rep 'gini1.out' u 1:2 w l lw 3 lc rgbcolor '#5aa732' t 'Addresses with nonzero balance'
rep '../../price2.dat' u (strptime("%d/%m/%YT%H:%M:%S",strcol(1))+946684800):2 axes x1y2 w l lw 3 lc rgbcolor '#005c96' t 'Exchange price'

se te post eps color solid size 5,3
se out 'addrweek8.eps'
rep
se out
!epstopdf addrweek8.eps
!convert -density 200 addrweek8.eps -bordercolor white -border 0x0 addrweek8.png

exit
# Fig 4.5a Gini-coeff, aggregált hálózat
gnuplot
se xdata time
se timefmt '%s'
se xtics format '%Y'
se ke r b
se xr ['1230768000':'1425168000']
p 'gini1B.out' u 1:5 w l lw 3 lc rgbcolor '#e75216' t 'Indegrees'
rep 'gini1B.out' u 1:6 w l lw 3 lc rgbcolor '#5aa732' t 'Outdegrees'
rep 'gini1B.out' u 1:4 w l lw 3 lc rgbcolor '#005c96' t 'Balances'
se te post eps color solid size 3,2
se out 'ginib2.eps'
rep
se out
se te wxt
!epstopdf ginib2.eps
!convert -density 200 ginib2.eps -bordercolor white -border 0x0 ginib2.png

#rep 'gini1.out' u 1:5 w l lw 3 lc rgbcolor '#009ace' t 'Indegrees'
#rep 'gini1.out' u 1:6 w l lw 3 lc rgbcolor '#ffd500' t 'Outdegrees'
#rep 'gini1.out' u 1:4 w l lw 3 lc rgbcolor '#8671a7' t 'Balances'
exit

# Fig 4.5b Gini-coeff, partial hálózat
cd /media/data1/dani/bitcoin/miscruns2015/addr/partial
gnuplot
se xdata time
se timefmt '%s'
se xtics format '%Y'
se xr ['1230768000':'1425168000']
se xl 'Time'
se yl 'Gini-coefficient'
p 'edgesnew1m.out' u 2:5 w l lw 3 lc rgbcolor '#e75216' t 'Indegrees'
rep 'edgesnew1m.out' u 2:6 w l lw 3 lc rgbcolor '#5aa732' t 'Outdegrees'

se te post eps color solid size 3,2
se out 'ginipartial1m.eps'
rep
se out
se te wxt
!epstopdf ginipartial1m.eps
!convert -density 200 ginipartial1m.eps -bordercolor white -border 0x0 ginipartial1m.png

# Fig 4.6b degree correlation, clustering, teljes
cd '/media/data1/dani/bitcoin/miscruns2015/addr/total'
se yl 'Measured coefficient'
se yr [-0.4:0.3]
p 'dc1.out' u 1:2 w l lw 3 lc rgbcolor '#e75216' t 'Degree correlation'
rep 'cl1.out'u 1:2 w l lw 3 lc rgbcolor '#5aa732' t 'Clustering'
rep 0 w l lw 2 lc -1 not

se te post eps color solid size 3,2
se out 'degcltotal.eps'
rep
se out
se te wxt
!epstopdf degcltotal.eps
!convert -density 200 degcltotal.eps -bordercolor white -border 0x0 degcltotal.png


# Fig 4.6b degree correlation, clustering, partial hálózat
cd '/media/data1/dani/bitcoin/miscruns2015/addr/partial'
se yl 'Measured coefficient'
se yr [-0.4:0.6]
p 'edgesnew1m.out' u 2:7 w l lw 3 lc rgbcolor '#e75216' t 'Degree correlation'
rep 'edgesnew1m.out' u 2:11 w l lw 3 lc rgbcolor '#5aa732' t 'Clustering'
rep 0 w l lw 1 lc -1 not

se te post eps color solid size 3,2
se out 'degclpartial1m.eps'
rep
se out
se te wxt
!epstopdf degclpartial1m.eps
!convert -density 200 degclpartial1m.eps -bordercolor white -border 0x0 degclpartial1m.png
exit

###################################################
## ugyanezek az SCC-kre
# Fig 4.5a Gini-coeff, aggregált hálózat
cd /media/data1/dani/bitcoin/miscruns2015/sccs/total
gnuplot
se xdata time
se timefmt '%s'
se xtics format '%Y'
se ke r b
se xr ['1230768000':'1425168000']
p 'gini1B.out' u 1:5 w l lw 3 lc rgbcolor '#e75216' t 'Indegrees'
rep 'gini1B.out' u 1:6 w l lw 3 lc rgbcolor '#5aa732' t 'Outdegrees'
rep 'gini1B.out' u 1:4 w l lw 3 lc rgbcolor '#005c96' t 'Balances'
se te post eps color solid size 3,2
se out 'sccginib2.eps'
rep
se out
se te wxt
!epstopdf sccginib2.eps
!convert -density 200 sccginib2.eps -bordercolor white -border 0x0 sccginib2.png

#rep 'gini1.out' u 1:5 w l lw 3 lc rgbcolor '#009ace' t 'Indegrees'
#rep 'gini1.out' u 1:6 w l lw 3 lc rgbcolor '#ffd500' t 'Outdegrees'
#rep 'gini1.out' u 1:4 w l lw 3 lc rgbcolor '#8671a7' t 'Balances'
exit

# Fig 4.5b Gini-coeff, partial hálózat
cd /media/data1/dani/bitcoin/miscruns2015/sccs/partial
gnuplot
se xdata time
se timefmt '%s'
se xtics format '%Y'
se xr ['1230768000':'1425168000']
se xl 'Time'
se yl 'Gini-coefficient'
p 'edgesnew1m.out' u 2:5 w l lw 3 lc rgbcolor '#e75216' t 'Indegrees'
rep 'edgesnew1m.out' u 2:6 w l lw 3 lc rgbcolor '#5aa732' t 'Outdegrees'

se te post eps color solid size 3,2
se out 'sccginipartial1m.eps'
rep
se out
se te wxt
!epstopdf sccginipartial1m.eps
!convert -density 200 sccginipartial1m.eps -bordercolor white -border 0x0 sccginipartial1m.png


# Fig 4.6b degree correlation, clustering, teljes
cd '/media/data1/dani/bitcoin/miscruns2015/sccs/total'
se yl 'Measured coefficient'
se yr [-0.4:0.6]
p 'dc1.out' u 1:2 w l lw 3 lc rgbcolor '#e75216' t 'Degree correlation'
rep 'cl1.out'u 1:2 w l lw 3 lc rgbcolor '#5aa732' t 'Clustering'
rep 0 w l lw 2 lc -1 not

se te post eps color solid size 3,2
se out 'sccdegcltotal.eps'
rep
se out
se te wxt
!epstopdf sccdegcltotal.eps
!convert -density 200 sccdegcltotal.eps -bordercolor white -border 0x0 sccdegcltotal.png

# Fig 4.6b degree correlation, clustering, partial hálózat
cd '/media/data1/dani/bitcoin/miscruns2015/sccs/partial'
se yl 'Measured coefficient'
p 'edgesnew1m.out' u 2:7 w l lw 3 lc rgbcolor '#e75216' t 'Degree correlation'
rep 'edgesnew1m.out' u 2:11 w l lw 3 lc rgbcolor '#5aa732' t 'Clustering'
rep 0 w l lw 1 lc -1 not

se te post eps color solid size 3,2
se out 'sccdegclpartial1m.eps'
rep
se out
se te wxt
!epstopdf sccdegclpartial1m.eps
!convert -density 200 sccdegclpartial1m.eps -bordercolor white -border 0x0 sccdegclpartial1m.png



################################################################
## kell még (SQL Server).
# Fig 4.7.: ANND ábra a végső állapotban
# Fig 4.9.: tranzakciók között eltelt idők eloszlása (SCC-kre is külön)
# Fig 4.11.: 1 hónapos időablakokban változás -> /media/data3/dkondor/bitcoin/patest2015/money/bdiffs/run1.sh
# Fig 4.13.: fokszám és vagyon közötti korreláció a végállapotban

## Fig 4.7.: ANND a végső hálózatra
cd /media/data1/dani/bitcoin/miscruns2015/addr/total
set -x LC_ALL en_US.utf8
sqsh -G 7.0 -U 'VO\dkondor' -S future1.vo.elte.hu

use BitcoinNew
with a as (select d1.addr, d1.outdeg, d2.indeg from txedgemin e join degree d1 on e.ain = d1.addr join degree d2 on e.aout = d2.addr)
select min(outdeg), avg(indeg) from a group by addr order by min(outdeg);
go -h -f | sed -e 's/  */\t/g' -e 's/^\t//' > annd1.dat
exit

cd /media/data1/dani/bitcoin/miscruns2015/sccs/total
set -x LC_ALL en_US.utf8
sqsh -G 7.0 -U 'VO\dkondor' -S future1.vo.elte.hu

use BitcoinNew
with a as (select d1.addr, d1.outdeg, d2.indeg from txedgemins e join degree_sccs d1 on e.sccin = d1.addr join degree_sccs d2 on e.sccout = d2.addr)
select min(outdeg), avg(indeg) from a group by addr order by min(outdeg);
go -h -f | sed -e 's/  */\t/g' -e 's/^\t//' > annd1.dat
exit


cd /media/data1/dani/bitcoin/miscruns2015/addr/total
awk 'BEGIN{d=1;s=0.0;cnt=0.0;}{if($1>d) {printf "%d\t%g\n",d,s/cnt; d=$1;s=0;cnt=0;} s+=$2;cnt++;}END{if(cnt>0)printf "%d\t%g\n",d,s/cnt;}' annd1.dat > anndavg.dat

se log xy
se xl 'outdegree'
se yl 'avg. indegree of neighbors'
p 'anndavg.dat' u 1:2 w p ps 1.5 pt 7 not
se te post eps color solid size 5,3
se out 'anndoutin4.eps'
rep
se out
se te wxt
!epstopdf anndoutin4.eps
!convert -density 200 anndoutin4.eps -bordercolor white -border 0x0 anndoutin4.png

cd /media/data1/dani/bitcoin/miscruns2015/sccs/total
awk 'BEGIN{d=1;s=0.0;cnt=0.0;}{if($1>d) {printf "%d\t%g\n",d,s/cnt; d=$1;s=0;cnt=0;} s+=$2;cnt++;}END{if(cnt>0)printf "%d\t%g\n",d,s/cnt;}' annd1.dat > anndavg.dat

se log xy
se xl 'outdegree'
se yl 'avg. indegree of neighbors'
p 'anndavg.dat' u 1:2 w p ps 1.5 pt 7 not
se te post eps color solid size 3,2
se out 'sccanndoutin4.eps'
rep
se out
se te wxt
!epstopdf sccanndoutin4.eps
!convert -density 200 sccanndoutin4.eps -bordercolor white -border 0x0 sccanndoutin4.png



###################################################################################
## delay-ek eloszlása
cd /media/data1/dani/bitcoin/miscruns2015/addr/total
set -x LC_ALL en_US.utf8
sqsh -G 7.0 -U 'VO\dkondor' -S future1.vo.elte.hu

use BitcoinNew
go

with a as (select distinct addr, txid from txin), b as
(select *, row_number() over(partition by addr order by txid) rn from a), c as
(select b1.txid as tx1, b2.txid as tx2 from b b1 inner merge join b b2 on b1.addr = b2.addr
	and b1.rn + 1 = b2.rn)
select t2.btime - t1.btime from c join tx t1 on c.tx1 = t1.txid join tx t2 on
	c.tx2 = t2.txid;
go -h -f | sed -e 's/  */\t/g' -e 's/^\t//' | gzip -c > dt1.gz


with b as (select sccid, txid, row_number() over(partition by sccid order by txid) rn from txins),
c as (select b1.txid as tx1, b2.txid as tx2 from b b1 inner merge join b b2 on b1.sccid = b2.sccid
	and b1.rn + 1 = b2.rn)
select t2.btime - t1.btime from c join tx t1 on c.tx1 = t1.txid join tx t2 on c.tx2 = t2.txid;
go -h -f | sed -e 's/  */\t/g' -e 's/^\t//' | gzip -c > ../../sccs/total/dt1.gz

zcat dt1.gz | tail -n +2 | qsd | gzip -c > dt1.s.gz

zcat dt1.s.gz | awk 'BEGIN{xmin=0;binsize=2;binscale=2;sum=0;}{if($1>xmin+binsize) { printf "%g\t%g\t%g\n",xmin,xmin+binsize,sum/binsize; xmin += binsize; binsize *= binscale; sum=0;} sum++;}END{if(sum>0) {printf "%g\t%g\t%g\n",xmin,xmin+binsize,sum/binsize;}}' > dt1.lb

se log xy
se xl 'Delay [s]'
se yl 'Normalized frequency'
p 'dt1.lb' u (sqrt(($1+0.5)*$2)):3 w lp lw 3 ps 1.5 pt 7 not
se te post eps color solid size 3,2
se out 'sccdt1.eps'
rep
se out
se te wxt
!epstopdf sccdt1.eps
!convert -density 200 sccdt1.eps  -bordercolor white -border 0x0 sccdt1.png
!cp sccdt1.pdf /home/dani/phdthesis/figsbitcoin1/



-- korábbi adatok (blockchain.info-s időpontok)
use Bitcoin;
with a as (select addrid, txtime, row_number() over(partition by addrid order by txtime) rn from txin
	join txtime2 on txin.txID = txtime2.txID)
select a2.txtime - a1.txtime from a a1 join a a2 on a1.addrID = a2.addrID and a1.rn + 1 = a2.rn;
go -h -f | sed -e 's/  */\t/g' -e 's/^\t//' | gzip -c > dtold.gz

zcat dtold.gz | qsd | gzip -c > dtold.s.gz
zcat dtold.s.gz | awk 'BEGIN{xmin=0;binsize=2;binscale=2;sum=0;}{if($1>xmin+binsize) { printf "%g\t%g\t%g\n",xmin,xmin+binsize,sum/binsize; xmin += binsize; binsize *= binscale; sum=0;} sum++;}END{if(sum>0) {printf "%g\t%g\t%g\n",xmin,xmin+binsize,sum/binsize;}}' > dtold.lb


## közös ábra
se log xy
se xl 'Delay [s]'
se yl 'Normalized frequency'
se ke l b
se xtics 1e-2
se ytics 1e-3
p 'dt1.lb' u (sqrt(($1+0.5)*$2)):3 w lp lw 3 ps 1.5 pt 7 lc rgbcolor '#e75216' t 'Block timestamps'
rep 'dtold.lb' u (sqrt(($1+0.5)*$2)):3 w lp lw 3 ps 1.5 pt 7 lc rgbcolor '#5aa732' t 'blockchain.info timestamps'
rep 1e8/x w l lw 3 lc -1 not
se te post eps color solid size 3,2
se out 'dt2.eps'
rep
se out
se te wxt
!epstopdf dt2.eps
!convert -density 200 dt2.eps -bordercolor white -border 0x0 dt2.png

## scc-k
cd '../../sccs/total'
p 'dt1.lb' u (sqrt(($1+0.5)*$2)):3 w lp lw 3 ps 1.5 pt 7 lc rgbcolor '#e75216' not
rep 1e7/x w l lw 3 lc -1 not
se te post eps color solid size 3,2
se out 'sccdt1.eps'
rep
se out
se te wxt
!epstopdf sccdt1.eps
!convert -density 200 sccdt1.eps -bordercolor white -border 0x0 sccdt1.png





#################################################################
# patest eredmények külön féléves szakaszokban
cd /media/data3/dkondor/bitcoin/patest2015/addr2
# sorok leszámolása külön
for i in (seq 13)
  echo $i
  set l (for a in pt6r10y2-1.00-?-$i.dat.zip
  unzip -p $a
  end | wc -l)
  echo $l
  echo $i (math $l/100000)
end
# 0 0 0 1 21 45 89 212 372 440 1273 1572 741
set lr 1 1 1 1 21 45 89 212 372 440 1273 1572 741

for i in (seq 13)
for a in 0.70 0.85 1.00 1.10 1.20 1.35 0.00 0.25 0.50 1.50
set n 2
for b in pt6r10y$n-$a-?-$i.dat.zip
unzip -p $b
end | ~/bitcoin/patest/patest2/fit1 -S -n -o out3/pt6r10y$n-$a-$i | awk "{if(NR%$lr[$i]==0) print \$0}" > out3/pt6r10y$n-$a-$i.dat
echo $a $n
end
end




