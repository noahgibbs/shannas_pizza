#!/bin/bash
CONN_SOURCE="../connectors.c test_conn.c"
OPTIONS="-Wall -O -g"
gcc $OPTIONS -o test_conn $CONN_SOURCE -I. -I..
