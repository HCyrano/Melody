# ============================================================
# Makefile — Melody  (macOS + Linux, arm64 / x86_64)
# ============================================================

OS   := $(shell uname -s)
ARCH := $(shell uname -m)

# ============================================================
# Flags d'Architecture (ARM vs x86_64)
# ============================================================

# ============================================================
# Flags d'Architecture (ARM vs x86_64)
# ============================================================

ifeq ($(ARCH), arm64)
    MARCH      = -march=armv8-a+crc
    SIMD_FLAGS =
else ifeq ($(ARCH), aarch64)
    MARCH      = -march=armv8-a+crc
    SIMD_FLAGS =
else
    # Intel / AMD x86_64
    MARCH      = -march=x86-64-v3
    SIMD_FLAGS = -mavx2 -mbmi2
endif

# ============================================================
# Compilateur et Flags spécifiques par OS
# ============================================================

ifeq ($(OS), Darwin)
    CXX        = clang++
    SDK        = $(shell xcrun --sdk macosx --show-sdk-path)
    OS_FLAGS   = -arch $(ARCH) -isysroot $(SDK)
    OS_LIBS    =
    VEC_FLAGS  = -fvectorize -fslp-vectorize
    WARN_FLAGS = -Wno-unused-private-field
    LDFLAGS_STRIP = -Wl,-dead_strip
else
    CXX       ?= g++
    OS_FLAGS   =
    OS_LIBS    = -lpthread -lm
    VEC_FLAGS  =
    WARN_FLAGS =
    LDFLAGS_STRIP = -s
endif

# ============================================================
# Flags de compilation communs
# ============================================================

CXXFLAGS = -std=c++23             \
           $(MARCH)               \
           $(OS_FLAGS)            \
           -O3                    \
           -ffast-math            \
           -fomit-frame-pointer   \
           -ffunction-sections    \
           -fdata-sections        \
           $(VEC_FLAGS)           \
           -I./include            \
           -I./Roxane/RXRoxane    \
           -I./Roxane/GGS_Client  \
           -I./Roxane/Std_Client  \
           -Wall                  \
           -Wno-char-subscripts   \
           -Wno-reorder           \
           -Wno-misleading-indentation \
           $(WARN_FLAGS)

LDFLAGS = $(OS_FLAGS) $(OS_LIBS) $(LDFLAGS_STRIP)

TARGET = build/Melody

# ============================================================
# Objets communs (toutes plateformes)
# ============================================================

COMMON_OBJS = \
    build/GGSMessage.o         \
    build/GGSObjects.o         \
    build/ggsstream.o          \
    build/ODKStream.o          \
    build/OsMessage.o          \
    build/OsObjects.o          \
    build/sockbuf.o            \
    build/main.o               \
    build/RXBBPatterns.o       \
    build/RXBitBoard.o         \
    build/RXEndGame.o          \
    build/RXEngine.o           \
    build/RXEvaluation.o       \
    build/RXHashShallow.o      \
    build/RXHashTable.o        \
    build/RXMidGame.o          \
    build/RXMove.o             \
    build/RXRoxane.o           \
    build/RXSearch.o           \
    build/IOStdProtocol.o      \
    build/StdInput.o

OBJS = $(COMMON_OBJS) $(SIMD_OBJS)

# ============================================================
# Règles
# ============================================================

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) -o $@ $^
	@echo "✅ Build OK → $(TARGET)  [$(OS)/$(ARCH)]"
	@du -h $(TARGET)

# ============================================================
# GGS Client
# ============================================================

build/%.o: Roxane/GGS_Client/%.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# ============================================================
# Main
# ============================================================

build/main.o: Roxane/main.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# ============================================================
# RXRoxane — code générique
# ============================================================

build/%.o: Roxane/RXRoxane/%.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# ============================================================
# Std Client
# ============================================================

build/%.o: Roxane/Std_Client/%.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# ============================================================
# Utilitaires
# ============================================================

clean:
	rm -rf build/

cleanobj:
	@rm -f build/*.o build/*.d
	@echo "Objets supprimés"

rebuild: clean all

info:
	@echo "OS        : $(OS)"
	@echo "ARCH      : $(ARCH)"
	@echo "CXX       : $(CXX)"
	@echo "MARCH     : $(MARCH)"
	@echo "LDFLAGS   : $(LDFLAGS)"
	@echo "OBJS      : $(words $(OBJS)) fichiers"

-include $(wildcard build/*.d)

.PHONY: all clean cleanobj rebuild info
