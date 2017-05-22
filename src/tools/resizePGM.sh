#!/bin/bash

# $1 <- carpeta de donde convertir imagenes
# $2 <- ancho
# $3 <- alto

if test "$#" -ne 3; then
    echo "-----------------------------------"
    echo "Cambia tamaños de todos los .pgm de los subdirectorios"
    echo "No se sobreescribe nada. Se crea una nueva carpeta."
    echo "-----------------------------------"
    echo "USO: ./resizePGM.sh carpeta/con/subdirec ancho alto"
    echo "-----------------------------------"
    exit
fi

pwd=$(pwd)

cp -r $1 $1_$2x$3
cd $1_$2x$3


find . -name \*.pgm -exec mogrify -format pgm -resize $2x$3\! "{}" \;

cd $pwd

