#!/bin/bash

x=3
until [ $x = "138" ]; do
    file=`printf "periodic/140/heatmap_%0.3d.dat" $x`
    ./a.out > $file
    ./advance_wall.sh
    ((x++))
    ./advance_wall.sh
    ((x++))
done
