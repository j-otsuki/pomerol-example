reset
set key bottom
set zeroaxis lw 2

set style line 1 lt 1 lc 1 lw 3 pt 6 ps 1.5
set style line 2 lt 1 lc 3 lw 3 pt 6 ps 1.5
set style line 3 lt 1 lc 2 lw 3 pt 4 ps 1.5
set style line 4 lt 1 lc 4 lw 3 pt 4 ps 1.5


# set contour
set pm3d

set terminal postscript eps color enhanced "Times-Roman" 24
set output "| epstopdf -f -o=chi.pdf"
# set xlabel "{/Symbol-Oblique w}_{/Times-Italic n}"
# set ylabel "{/Times-Italic G}(i{/Symbol-Oblique w}_{/Times-Italic n})"
set xrange[*:*]
set yrange[*:*]

splot "< grep -A 1 'wb=0' chi.dat" u 2:3:5 title"" w l ls 1,\

set output
