#!/usr/bin/gnuplot
# set terminal svg
# set yrange [0:10]
# set xrange [0:55]
set xlabel "Input Size: 2^n"
set ylabel "Runtime Cycles"
# set format x "%g K"
set format y "%g K"
# set format y "%g ms"
set title "float div performance plot(gcc )"
# font "Helvetica,9"
set style line 1  lc rgb '#fc7303' lt 1 lw 1.5
set style line 2  lc rgb '#fc7303' lt 1 lw 1.5 
set style line 3  lc rgb '#035afc' lt 1 lw 1.5 
set style line 4  lc rgb '#0ffc03' lt 1 lw 1.5 
set style line 5  lc rgb '#fc032d' lt 1 lw 1.5 
set style line 6  lc rgb '#454545' lt 1 lw 1.5
set style line 7  lc rgb '#a81da4' lt 1 lw 1.5
plot "a.dat"  title "9 iterations" w l ls 1, \
# "result/data/part1/l1i.dat" title "l1i"  w xyerrorlines  ls 2 , \
# "result/data/part1/l1d.dat" title "l1d" w xyerrorlines ls 3 , \
# "result/data/part1/l2.dat"  title "l2" w xyerrorlines ls 4 , \
# "result/data/part1/llc.dat"   title "llc" w xyerrorlines ls 5,\
# "result/data/part1/membw.dat"   title "membw" w xyerrorlines ls 6,\
# "result/data/part1/none.dat" title "no interference" w xyerrorlines ls 7 
