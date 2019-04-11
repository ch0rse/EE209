#!/bin/sh
./gcc209 -o sgrep sgrep.c str.c
rm -rf 20180336_assign2
rm -f 20180336_assign2.tar.gz
mkdir -p 20180336_assign2
cp sgrep.c str.c str.h readme EthicsOath.pdf 20180336_assign2
tar zcf 20180336_assign2.tar.gz 20180336_assign2
