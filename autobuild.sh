#!/bin/bash
set -e

if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

rm -rf `pwd`/build/*

cd `pwd`/build && cmake .. && make

cd ..

if [ ! -d /usr/include/muduo ]; then
    mkdir /usr/include/muduo
fi

cd `pwd`/src

for header in `ls *.h`
do 
    cp $header /usr/include/muduo
done

cd ..

cp `pwd`/lib/libmuduo.so /usr/lib

ldconfig
