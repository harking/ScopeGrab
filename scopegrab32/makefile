# Project: ScopeGrab32
# Makefile created by Dev-C++ 4.9.9.2

CPP  = g++ -D__DEBUG__
CC   = gcc -D__DEBUG__
WINDRES = windres.exe
RES  = ScopeGrab32_private.res
OBJ  = main.o CSerialLinux.o $(RES)
LINKOBJ  = main.o CSerialLinux.o $(RES)
LIBS =  `wx-config --libs`
INCS =  
CXXINCS = 
BIN  = ScopeGrab32.bin
CXXFLAGS = `wx-config --cxxflags` -Wall  -fno-pcc-struct-return  -fstrict-aliasing  -g3 -O0   -g3
CFLAGS = $(INCS)   -g3
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before ScopeGrab32.bin all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "ScopeGrab32.bin" $(LIBS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)

CSerialLinux.o: CSerialLinux.cpp
	$(CPP) -c CSerialLinux.cpp -o CSerialLinux.o $(CXXFLAGS)

ScopeGrab32_private.res: ScopeGrab32_private.rc ScopeGrab32_private.rc Resources.rc 
	$(WINDRES) -i ScopeGrab32_private.rc --input-format=rc -o ScopeGrab32_private.res -O coff 
