BASE_DIR = .
BIN_DIR = $(BASE_DIR)/bin
OBJ_DIR = $(BIN_DIR)/obj
ENCODEOBJ_DIR = $(OBJ_DIR)/enc
DECODEOBJ_DIR = $(OBJ_DIR)/dec
ENCODETGT = $(BIN_DIR)/encode
DECODETGT = $(BIN_DIR)/decode
# пути задаются относительно $(BASE_DIR)
SRC = $(BASE_DIR)/src/htree.cpp $(BASE_DIR)/src/list_node.cpp
ENCODESRC = $(BASE_DIR)/src/huffman.cpp $(BASE_DIR)/src/encode.cpp
DECODESRC = $(BASE_DIR)/src/huffman.cpp $(BASE_DIR)/src/decode.cpp

OBJ = $(SRC:.cpp=.o)
OBJ := $(subst $(BASE_DIR)/src,$(OBJ_DIR),$(OBJ))
ENCODEOBJ = $(ENCODESRC:.cpp=.o)
ENCODEOBJ := $(subst $(BASE_DIR)/src,$(ENCODEOBJ_DIR),$(ENCODEOBJ))
DECODEOBJ = $(DECODESRC:.cpp=.o)
DECODEOBJ := $(subst $(BASE_DIR)/src,$(DECODEOBJ_DIR),$(DECODEOBJ))
$(info OBJ=$(OBJ) $(OBJ_DIR))

uname_S := $(shell uname -s)
ifeq (MINGW,$(findstring MINGW,$(uname_S)))
  targethost = MINGW
  target-ext = .exe
endif
ifeq ($(uname_S), Linux)
  targethost = Linux
endif

CXXFLAGS += -std=gnu++11 -Wall -pedantic -O3 -flto -msse4.2

#LDFLAGS += -static -static-libgcc

.PHONY: all clean cleanobj makedir

all: makedir $(ENCODETGT)$(target-ext) $(DECODETGT)$(target-ext)

$(ENCODETGT)$(target-ext): $(OBJ) $(ENCODEOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(DECODETGT)$(target-ext): $(OBJ) $(DECODEOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(BASE_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(ENCODEOBJ_DIR)/%.o: $(BASE_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(DECODEOBJ_DIR)/%.o: $(BASE_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) -DHUFFMANDECODE -c -o $@ $<

cleanobj:
	$(RM) -f $(OBJ)
	$(RM) -f $(ENCODEOBJ)
	$(RM) -f $(DECODEOBJ)

clean: cleanobj
	$(RM) -f $(ENCODETGT)$(target-ext)
	$(RM) -f $(DECODETGT)$(target-ext)

makedir:
	- 2>/dev/null 1>&2 mkdir $(BIN_DIR)
	- 2>/dev/null 1>&2 mkdir $(OBJ_DIR)
	- 2>/dev/null 1>&2 mkdir $(ENCODEOBJ_DIR)
	- 2>/dev/null 1>&2 mkdir $(DECODEOBJ_DIR)
