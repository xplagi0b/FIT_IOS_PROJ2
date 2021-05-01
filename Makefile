CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -pedantic -Werror -pthread

proj2: proj2.c utils.h

#ipcs -tm | grep "$(whoami)" | awk '{print $1}' | xargs -L1 ipcrm -m
