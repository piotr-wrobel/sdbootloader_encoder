# Project: SDBootloader Encoder
# Makefile 

CPP      = g++
CC       = gcc

WINDRES  = windres
SRC		 = main.cpp
OBJ      = main.o
LINKOBJ  = main.o
LIBS     = 
INCS     = 
CXXINCS  = 
BIN      = sdbootloader_encoder.exe
CXXFLAGS = $(CXXINCS) -march=i386 -Os -m32 -Wall -Wfatal-errors
CFLAGS   = $(INCS) -march=i386 -Os -m32 -Wall -Wfatal-errors
RM       = rm.exe -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

$(OBJ): $(SRC)
	$(CPP) -c $(SRC) -o $(OBJ) $(CXXFLAGS)
