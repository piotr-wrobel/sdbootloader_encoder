# Project: SDBootloader Encoder
# Makefile 

CPP      = g++
CC       = gcc

SRC		 = main.cpp
OBJ      = main.o
LINKOBJ  = main.o
LIBS     = 
INCS     = 
CXXINCS  = 
BIN      = sdbootloader_encoder.exe
CXXFLAGS = $(CXXINCS) -Os -Wall -Wfatal-errors
CFLAGS   = $(INCS) -Os -Wall -Wfatal-errors
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

$(OBJ): $(SRC)
	$(CPP) -c $(SRC) -o $(OBJ) $(CXXFLAGS)
