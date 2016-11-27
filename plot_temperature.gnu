# run this with 
# $ gnuplot
# $ gnuplot> load "plot_temperature.gnu"

unset key
set tic scale 0

# Color runs from red(hot) to blue(cold)
set palette defined (0 "blue", 0.5 "green", 1 "red")
set cbrange [0:1]
set cblabel "Temperature"
set xlabel 'x'
set ylabel 'y'

set xrange [0.0:1.0]
set yrange [0.0:1.0]

set view map
# Insulation is set to -DBL_MAX, so we can plot that as white space.
splot 'heatmap.dat' u 1:2:($3>=-1?$3:NaN) with image

set term png
set output "heatmap.png"
replot
set output
set term x11
replot
