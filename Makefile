СС = gcc
STD = -std=gnu99

default: SimplestTCPclient

SimplestTCPclient: TCPclient.c
	$(CC) $(STD) TCPclient.c -lpthread -o SimplestTCPclient

