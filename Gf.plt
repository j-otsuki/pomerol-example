reset
set key bottom
set zeroaxis lw 2

set style line 1 lt 1 lc 1 lw 3 pt 6 ps 1.5
set style line 2 lt 1 lc 3 lw 3 pt 6 ps 1.5
set style line 3 lt 1 lc 2 lw 3 pt 4 ps 1.5
set style line 4 lt 1 lc 4 lw 3 pt 4 ps 1.5



set terminal postscript eps color enhanced "Times-Roman" 24
# set output "Gf.eps"
set output "| epstopdf -f -o=Gf.pdf"
set xlabel "{/Symbol-Oblique w}_{/Times-Italic n}"
set ylabel "{/Times-Italic G}(i{/Symbol-Oblique w}_{/Times-Italic n})"
set xrange[0:*]
set yrange[*:*]

plot "Gf.dat" u 1:2 title"Re" w lp ls 1,\
 ""u 1:3 title"Im" w lp ls 2\

set output
