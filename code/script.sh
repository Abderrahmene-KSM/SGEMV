#!/bin/bash

if [[ $1 == "-d" ]]; then
  wget -O libHalide.a https://www.dropbox.com/s/1n66ld5h6lw3rqa/libHalide.a?dl=0
  g++ SGEMV_halide.cpp -g -I . -L  .  -lHalide  -lpthread -ldl -lz -ltinfo -o SGEMV_halide -std=c++17
  
else
  if [[ $1 == "-p" ]]; then
    path="$2"
    include_path="$path/include"
    lib_path="$path/lib"
    g++ SGEMV_halide.cpp -g -I "$include_path" -L  "$lib_path"  -lHalide  -lpthread -ldl -lz -ltinfo -o SGEMV_halide -std=c++17
  else
    library_path=$(dirname $(find /home -name libHalide.a | head -n 1))
    g++ SGEMV_halide.cpp -g -I . -L  "$library_path"   -lHalide  -lpthread -ldl -lz -ltinfo -o SGEMV_halide -std=c++17
  fi    	
fi

if [[ $1 == "-pb" ]]; then
  path="$2"
  include_path="$path/include"
  lib_path="$path/lib"
  g++ SGEMV_halide.cpp -g -I "$include_path" -L  "$lib_path"  -lHalide  -lpthread -ldl -o SGEMV_halide -std=c++17
fi


if [ $? -eq 0 ]; then
    echo ""
    echo ""
    echo "Halide program compiled successfully."
    echo ""
    echo ""
    echo "Halide execution ..."
    echo ""
    echo ""


    # Run the compiled program
    ./SGEMV_halide
else
    echo ""
    echo ""
    echo "Halide program compilation failed."
    echo ""
    echo ""
fi

echo ""
echo ""
echo ""
echo "**************************************"
echo ""
echo ""

gcc SGEMV_c.c -o SGEMV_c

if [ $? -eq 0 ]; then
    echo "C program compiled successfully."
    echo ""
    echo ""
    echo "C execution ..."
    echo ""
    echo ""

    # Run the compiled program
    ./SGEMV_c
else
    echo "C program compilation failed."
    echo ""
    echo ""
fi


file1="../results/c_result.txt"
file2="../results/halide_result.txt"

echo ""
echo ""
if diff -q "$file1" "$file2" >/dev/null; then
    echo "Perfect, the results are the same!"
else
    echo "Something is wrong ! please make sure that the same example is provided"
fi








