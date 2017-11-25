reset
unset key
set zeroaxis lw 2

set style line 1 lt 1 lc 1 lw 3 pt 1 ps 2.0
set style line 2 lt 1 lc 3 lw 3 pt 6 ps 1.5
set style line 3 lt 1 lc 2 lw 3 pt 4 ps 1.5
set style line 4 lt 1 lc 4 lw 3 pt 4 ps 1.5



set terminal postscript eps color enhanced "Times-Roman" 24
set output "| epstopdf -f -o=eigenvalues.pdf"
# set xlabel "{/Symbol-Oblique w}_{/Times-Italic n}"
set ylabel "{/Times-Italic E}"
set xrange[*:*]
set yrange[*:*]

plot "eigenvalues.dat" u 0:1 title"" w p ls 1,\

set output




set output "| epstopdf -f -o=eigenvalues-n.pdf"
# set xlabel "{/Symbol-Oblique w}_{/Times-Italic n}"
set ylabel "{/Times-Italic E}"
set xtics 1
set grid lw 2
set xrange[-0.5:3.5]
set yrange[*:*]

plot "< grep -F '[0,' eigenvalues.dat" u (0):1 title"" w p ls 1,\
 "< grep -F '[1,' eigenvalues.dat" u (1):1 title"" w p ls 1,\
 "< grep -F '[2,' eigenvalues.dat" u (2):1 title"" w p ls 1,\
 "< grep -F '[3,' eigenvalues.dat" u (3):1 title"" w p ls 1,\

set output
