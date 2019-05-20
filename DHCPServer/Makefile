CC = gcc

CFLAGS = -g
COMMON_OBJS = common.o 

all: $(COMMON_OBJS) dhcp

#Common wrappers and definitions 
common.o: common.h common.c 
	$(CC) $(CFLAGS) -c common.c

dhcps: main.c $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o dhcp.o main.c $(COMMON_OBJS)

clean:
	rm -rf *.o *.dSY dhcp.o 