#!/bin/bash
# Generate all scene images using the compiled program.

SIZE="800 800"
BIN=build/a2

mkdir -p output

${BIN} -size ${SIZE} -input data/scene01_plane.txt  -output output/a01.png -normals output/a01n.png -depth 8 18 output/a01d.png
${BIN} -size ${SIZE} -input data/scene02_cube.txt   -output output/a02.png -normals output/a02n.png -depth 8 18 output/a02d.png
${BIN} -size ${SIZE} -input data/scene03_sphere.txt -output output/a03.png -normals output/a03n.png -depth 8 18 output/a03d.png
${BIN} -size ${SIZE} -input data/scene04_axes.txt   -output output/a04.png -normals output/a04n.png -depth 8 18 output/a04d.png
${BIN} -size ${SIZE} -input data/scene05_bunny_200.txt -output out/a05.png -normals out/a05n.png -depth 0.8 1.0 output/a05d.png
${BIN} -size ${SIZE} -input data/scene06_bunny_1k.txt -bounces 4 -output output/a06.png -normals output/a06n.png -depth 8 18 output/a06d.png
${BIN} -size ${SIZE} -input data/scene07_arch.txt -bounces 4 -shadows -output output/a07.png -normals output/a07n.png -depth 8 18 output/a07d.png
