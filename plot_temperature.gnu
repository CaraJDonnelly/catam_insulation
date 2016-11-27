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
splot 'heatmap.dat' u 1:2:3 with image
