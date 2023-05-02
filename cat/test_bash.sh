#!/bin/bash

filename1=test_1_cat.txt

sflags=("" "-b" "-e" "-n" "-s" "-t")


make

for ((i = 0; i < 6; i++))
do
    n=$((i+1))
    echo "\n\nTEST $n \n---------\n">>res_s21_cat.txt
    ./s21_cat ${sflags[$i]} "$filename1">>res_s21_cat.txt
    echo "\n\nTEST $n \n---------\n">>res_orig_cat.txt
    cat ${sflags[$i]} "$filename1">>res_orig_cat.txt
    
    if diff res_s21_cat.txt res_orig_cat.txt
    then
        echo ${sflags[$i]} SUCCESS
    else
        echo ${sflags[$i]} FAIL
    fi
done
