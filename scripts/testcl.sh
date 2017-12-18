#! /bin/bash
OUT=./garbage/
SER=server.out
STAT=statistic.out
CL=client.out
TSTP=test.out
TEST=./tests/
RES=./results/
	echo client is running
	$OUT/$CL "Topic #1" >$RES/res9
