BASE_DIR = .
BIN_DIR = $(BASE_DIR)/bin
OBJ_DIR = $(BIN_DIR)/obj
ENCODEOBJ_DIR = $(OBJ_DIR)/enc
DECODEOBJ_DIR = $(OBJ_DIR)/dec
ENCODETGT = $(BIN_DIR)/encode.exe
DECODETGT = $(BIN_DIR)/decode.exe
# пути задаются относительно $(BASE_DIR)
SRC = ./src/htree.cpp ./src/list_node.cpp
ENCODESRC = ./src/huffman.cpp ./src/encode.cpp
DECODESRC = ./src/huffman.cpp ./src/decode.cpp

OBJ = $(SRC:./src/=./bin/obj/)
OBJ = $(OBJ:.cpp=.obj)
ENCODEOBJ = $(ENCODESRC:./src/=./bin/obj/enc/)
ENCODEOBJ = $(ENCODEOBJ:.cpp=.obj)
DECODEOBJ = $(DECODESRC:./src/=./bin/obj/dec/)
DECODEOBJ = $(DECODEOBJ:.cpp=.obj)

CFLAGS = $(CFLAGS) /nologo /W3 \
          /O2 /Oi /Ot \
          /GL /Gw /Gy /GR- /GS- /EHsc \
          /TP /MD \
          /I$(BASE_DIR) \

LDFLAGS = $(LDFLAGS) /release

!IF "$(VSCMD_ARG_TGT_ARCH)"=="X64" || "$(VSCMD_ARG_TGT_ARCH)"=="x64"
LDFLAGS = $(LDFLAGS) /opt:ref,icf /subsystem:console,5.02 /MACHINE:X64 /INCREMENTAL:NO /NOLOGO /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /LTCG
!ELSE
CFLAGS = $(CFLAGS) /arch:SSE2
LDFLAGS = $(LDFLAGS) /opt:ref /subsystem:console,5.01 /MACHINE:X86 /INCREMENTAL:NO /NOLOGO /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /LTCG
!ENDIF

all: makedir $(ENCODETGT) $(DECODETGT) test

$(ENCODETGT): $(OBJ) $(ENCODEOBJ)
	link /OUT:$@ $(LDFLAGS) $**

$(DECODETGT): $(OBJ) $(DECODEOBJ)
	link /OUT:$@ $(LDFLAGS) $**

{$(BASE_DIR)/src/}.cpp{$(OBJ_DIR)/}.obj::
	$(CC) /MP $(CFLAGS) /Fo$(OBJ_DIR)/ /c $<

{$(BASE_DIR)/src/}.cpp{$(ENCODEOBJ_DIR)/}.obj::
	$(CC) /MP $(CFLAGS) /Fo$(ENCODEOBJ_DIR)/ /c $<

{$(BASE_DIR)/src/}.cpp{$(DECODEOBJ_DIR)/}.obj::
	$(CC) /MP $(CFLAGS) /DHUFFMANDECODE /Fo$(DECODEOBJ_DIR)/ /c $<

makedir:
	- @md "$(BIN_DIR)" 1>nul 2>&1
	- @md "$(OBJ_DIR)" 1>nul 2>&1
	- @md "$(ENCODEOBJ_DIR)" 1>nul 2>&1
	- @md "$(DECODEOBJ_DIR)" 1>nul 2>&1

cleanobj:
	- @del /f /q $(OBJ:/=\) 2>nul

clean: cleanobj
	- @del /f /q "$(ENCODETGT)" 2>nul
	- @del /f /q "$(DECODETGT)" 2>nul

test: $(ENCODETGT) $(DECODETGT)
	test\run-test.bat
