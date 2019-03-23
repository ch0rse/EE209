#!/bin/sh
rm -f 20180336_assign1
rm -f 20180336_assign1.tar.gz
./gcc209 -E wc209.c -o wc209.i
./gcc209 -S wc209.c -o wc209.s 
./gcc209 -c wc209.c -o wc209.o
./gcc209 wc209.c -o wc209
mkdir -p 20180336_assign1
mv wc209.c wc209.i wc209.s wc209.o wc209 readme EthicsOath.pdf dfa.pdf 20180336_assign1
tar zcf 20180336_assign1.tar.gz 20180336_assign1
