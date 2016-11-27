#!/bin/bash

x=3
until [ $x = "98" ]; do
    file=`printf "hypothesis/100/heatmap_%0.3d.dat" $x`
    ./a.out > $file
    ./advance_wall.sh
    ((x++))
done
