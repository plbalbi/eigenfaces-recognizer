#!/bin/bash

# $1 <- carpeta de donde convertir imagenes
# $2 <- ancho
# $3 <- alto

if test "$#" -ne 3; then
    echo "-----------------------------------"
    echo "Conversor a PGM"
    echo "-----------------------------------"
    echo "USO: ./convertToPGM.sh carpeta/donde/tan/imagenes ancho alto"
    echo "-----------------------------------"
    echo "Se van a guardar en ./pgm/"
    echo "Las imagenes que se pasan deben ser .jpg, sino cambiar en el .sh"
    exit
fi



count=`ls -1 $1/*.jpg 2>/dev/null | wc -l`
if [ $count != 0 ]
then 
	mogrify -format pgm -resize $2x$3\! $1/*.jpg
	mkdir pgm
fi 

count=`ls -1 $1/*.png 2>/dev/null | wc -l`
if [ $count != 0 ]
then 
	mogrify -format pgm -resize $2x$3\! $1/*.png
	mkdir pgm
fi 

count=1
for i in $1/*.pgm
do
    if test -f $i 
    then
       mv $i pgm/$count.pgm
       echo "convertida $i"
       let count+=1
    fi
done
