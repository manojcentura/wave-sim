#!/bin/bash

for f in *ppm ; do convert -quality 100 $f `basename $f ppm`jpg; done 

#ffmpeg -codec mjpeg -y -r 30 -b 1800 -i %03d.jpg test1800.mp4

#ffmpeg -y -r 30 -i %03d.jpg -vcodec mjpeg -qscale 1 -an output.avi 

rm output.avi
ffmpeg -y -r 30 -i %04d.png -vcodec huffyuv output.avi 
rm *.jpg
rm *.png
totem output.avi
