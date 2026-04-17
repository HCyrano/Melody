# ============================================================
# Makefile — Roxane/Melody  (macOS + Linux, arm64 / x86_64)
# ============================================================

CXX = clang++

OS   := $(shell uname -s)
ARCH := $(shell uname -m)

ifeq ($(ARCH), arm64)
    MARCH = -march=native -mcpu=native
else ifeq ($(ARCH), aarch64)
    MARCH = -march=native -mcpu=native
else
    MARCH = -march=native -mavx2
endif

ifeq ($(OS), Darwin)
    SDK      = $(shell xcrun --sdk macosx --show-sdk-path)
    OS_FLAGS = -arch $(ARCH) -isysroot $(SDK)
    OS_LIBS  =
    CXX      = clang++
else
    OS_FLAGS =
    OS_LIBS  = -lpthread -lm
    CXX      = g++
endif

CXXFLAGS = -std=c++20 \
           $(MARCH) \
           $(OS_FLAGS) \
           -Os \
           -ffast-math \
           -fomit-frame-pointer \
           -fvectorize \
           -fslp-vectorize \
           -I./include \
           -I./Roxane/RXRoxane \
           -I./Roxane/GGS_Client \
           -I./Roxane/Std_Client \
           -Wall \
           -Wno-char-subscripts \
           -Wno-reorder \
           -Wno-misleading-indentation \
           -Wno-unused-private-field
           
LDFLAGS = $(OS_FLAGS) $(OS_LIBS)

TARGET = build/Melody

# ============================================================
# Objets — nommés explicitement pour éviter les conflits
# ============================================================

OBJS = \
    build/GGSMessage.o \
    build/GGSObjects.o \
    build/ggsstream.o \
    build/ODKStream.o \
    build/OsMessage.o \
    build/OsObjects.o \
    build/sockbuf.o \
    build/main.o \
    build/RXBBCountFlips_NEON.o \
    build/RXBBCountFlips.o \
    build/RXBBDoFlips_NEON.o \
    build/RXBBDoFlips.o \
    build/RXBBGenerateFlips_NEON.o \
    build/RXBBGenerateFlips.o \
    build/RXBBPatterns.o \
    build/RXBitBoard.o \
    build/RXEndGame.o \
    build/RXEngine.o \
    build/RXEvaluation.o \
    build/RXHashShallow.o \
    build/RXHashTable.o \
    build/RXMidGame.o \
    build/RXMove.o \
    build/RXRoxane.o \
    build/RXSearch.o \
    build/IOStdProtocol.o \
    build/StdInput.o

# ============================================================
# Règles
# ============================================================

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) -o $@ $^
	@echo "✅ Build OK → $(TARGET)  [$(OS)/$(ARCH)]"

# GGS Client
build/GGSMessage.o:       Roxane/GGS_Client/GGSMessage.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/GGSObjects.o:       Roxane/GGS_Client/GGSObjects.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/ggsstream.o:        Roxane/GGS_Client/ggsstream.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/ODKStream.o:        Roxane/GGS_Client/ODKStream.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/OsMessage.o:        Roxane/GGS_Client/OsMessage.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/OsObjects.o:        Roxane/GGS_Client/OsObjects.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/sockbuf.o:          Roxane/GGS_Client/sockbuf.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# main
build/main.o:             Roxane/main.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# RXRoxane
build/RXBBCountFlips_NEON.o:  Roxane/RXRoxane/RXBBCountFlips_NEON.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXBBCountFlips.o:       Roxane/RXRoxane/RXBBCountFlips.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXBBDoFlips_NEON.o:     Roxane/RXRoxane/RXBBDoFlips_NEON.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXBBDoFlips.o:          Roxane/RXRoxane/RXBBDoFlips.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXBBGenerateFlips_NEON.o: Roxane/RXRoxane/RXBBGenerateFlips_NEON.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXBBGenerateFlips.o:    Roxane/RXRoxane/RXBBGenerateFlips.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXBBPatterns.o:         Roxane/RXRoxane/RXBBPatterns.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXBitBoard.o:           Roxane/RXRoxane/RXBitBoard.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXEndGame.o:            Roxane/RXRoxane/RXEndGame.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXEngine.o:             Roxane/RXRoxane/RXEngine.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXEvaluation.o:         Roxane/RXRoxane/RXEvaluation.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXHashShallow.o:        Roxane/RXRoxane/RXHashShallow.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXHashTable.o:          Roxane/RXRoxane/RXHashTable.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXMidGame.o:            Roxane/RXRoxane/RXMidGame.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXMove.o:               Roxane/RXRoxane/RXMove.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXRoxane.o:             Roxane/RXRoxane/RXRoxane.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXSearch.o:             Roxane/RXRoxane/RXSearch.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Std Client
build/IOStdProtocol.o:    Roxane/Std_Client/IOStdProtocol.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/StdInput.o:         Roxane/Std_Client/StdInput.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# ============================================================

clean:
	rm -rf build/

cleanobj:
	@rm -f build/*.o build/*.d
	@echo "Objets supprimés"

rebuild: clean all

info:
	@echo "OS     : $(OS)"
	@echo "ARCH   : $(ARCH)"
	@echo "CXX    : $(CXX)"
	@echo "MARCH  : $(MARCH)"
	@echo "SDK    : $(SDK)"
	@echo "OBJS   : $(words $(OBJS)) fichiers"

-include $(wildcard build/*.d)

.PHONY: all clean rebuild info
