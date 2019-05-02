#!/bin/sh
./gcc209 -D_GNU_SOURCE -g -o testclient2 testclient.c customer_manager2.c
./testclient2 -c
