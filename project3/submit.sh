#!/bin/sh
./gcc209 -D_GNU_SOURCE -o testclient1 testclient.c customer_manager1.c
./testclient1 -c
#gcc209 -D_GNU_SOURCE -o testclient2 testclient.c customer_manager2.c
#./testclient2
