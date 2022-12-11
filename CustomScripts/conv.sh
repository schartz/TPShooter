#!/bin/bash

cd /home/neo/Footsteps

for x in *WAV *wav ; do
ffmpeg -i "$x" -acodec pcm_s16le "converted_$x"
echo "converted $x \n"
done

for x in converted_* ; do
mv "$x" "${x##converted_}"

echo "replaced $x \n"
done