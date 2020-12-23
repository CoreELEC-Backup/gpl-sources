CC?=gcc 
CFLG=-O -g -W
SRC=diseqc.c blindscan-s2.c
HED=diseqc.h blindscan-s2.h
OBJ=diseqc.o blindscan-s2.o

BIND=/usr/local/bin/
#INCLUDE=-I../s2-liplianin-v37/linux/include
#INCLUDE=/usr/include/linux

TARGET=blindscan-s2

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLG) $(OBJ) -o $(TARGET) $(CLIB) -lm

$(OBJ): $(HED)

install: all
	cp $(TARGET) $(BIND)

uninstall:
	rm $(BIND)$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET) *~ ._*

%.o: %.c
	$(CC) $(CFLG) $(INCLUDE) -c $< -o $@

