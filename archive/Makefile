#
#
#*****************************Makefile for bulding AR1212 CSS C-utility binary*****************************
#
#
detected_OS := ""
ifeq ($(OS),Windows_NT) 
	detected_OS := Windows
else
	SHELL = /bin/bash
	detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

print_OS:
	@echo 'Detected $(detected_OS) System'
ifeq ($(detected_OS), Linux)
	@echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}"
endif


# Source C files
SRC = mac_rx.c

# Output executable/binary and other variables
ifeq ($(detected_OS), Windows)
	OUT = mac_rx.exe
	INCLUDE_FILE = C:/openssl-1.1.1i-dev/openssl-1.1/x86/include
	LIBRARY_PATH = C:/openssl-1.1.1i-dev/openssl-1.1/x86/lib
	REM = cmd //C del //Q //F
	SSL_LIB = -llibssl -llibcrypto
endif
ifeq ($(detected_OS), Linux)
	OUT = mac_rx
	REM = rm -f
	export LD_LIBRARY_PATH:=/tools/icad/src/builds/openssl/1.1.1-pre7-dev/lib:${LD_LIBRARY_PATH}
	LIBRARY_PATH = /tools/icad/src/builds/openssl/1.1.1-pre7-dev/lib/
	INCLUDE_FILE = /tools/icad/src/builds/openssl/1.1.1-pre7-dev/include/
	SSL_LIB= -lssl -lcrypto
endif


# Specify compiler
CC=gcc

CFLAG = 

compile_cmd = $(CC) $(SRC) -o $(OUT) $(CFLAG) -I$(INCLUDE_FILE) -L$(LIBRARY_PATH) $(SSL_LIB)

# Clean target
.PHONY : clean
clean: print_OS
	@echo "Removing existing \"$(OUT)\" executable"
	@$(REM) $(OUT)

# Build all target
.PHONY : compile 
# Link the object files and dependent libraries into a binary
compile: clean	
	@if test -e $(SRC); then \
		echo "Compiling \"$(SRC)\"...., output file: \"$(OUT)\" "; \
		$(compile_cmd) ; \
	else \
		echo "source file \"$(SRC)\" not found"; \
	fi

