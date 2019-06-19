#!/bin/sh
rm -rf 20180336_assign5
rm -f 20180336_assign5.tar.gz
mkdir -p 20180336_assign5
cp makefile readme EthicsOath.pdf dfa.c dfa.h dynarray.c dynarray.h main.c main.h utils.c utils.h 20180336_assign5
tar zcf 20180336_assign5.tar.gz 20180336_assign5
