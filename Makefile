# ============================================================
# Makefile — Melody  (macOS + Linux, arm64 / x86_64)
# ============================================================

OS   := $(shell uname -s)
ARCH := $(shell uname -m)

# ============================================================
# Compilateur selon l'OS
# ============================================================

ifeq ($(OS), Darwin)
    CXX      = clang++
    SDK      = $(shell xcrun --sdk macosx --show-sdk-path)
    OS_FLAGS = -arch $(ARCH) -isysroot $(SDK)
    OS_LIBS  =
    VEC_FLAGS = -fvectorize -fslp-vectorize
else
    CXX      = g++
    OS_FLAGS =
    OS_LIBS  = -lpthread -lm
    VEC_FLAGS =
endif

# ============================================================
# Flags architecture : ARM vs x86_64
# ============================================================

ifeq ($(ARCH), arm64)
    MARCH     = -march=native -mcpu=native
    SIMD_OBJS = build/RXBBCountFlips_NEON.o \
                build/RXBBDoFlips_NEON.o
    SIMD_FLAGS =
else ifeq ($(ARCH), aarch64)
    MARCH     = -march=native -mcpu=native
    SIMD_OBJS = build/RXBBCountFlips_NEON.o \
                build/RXBBDoFlips_NEON.o
    SIMD_FLAGS =
else
    # Intel / AMD x86_64
    MARCH     = -march=x86-64-v3   # baseline AVX2+BMI2 portable (pas native)
    SIMD_OBJS = build/RXBBCountFlips_AVX2.o \
                build/RXBBDoFlips_AVX2.o
    SIMD_FLAGS = -mavx2 -mbmi2     # flags explicites pour les TU SIMD
endif

# ============================================================
# Flags de compilation communs
# ============================================================

CXXFLAGS = -std=c++20             \
           $(MARCH)               \
           $(OS_FLAGS)            \
           -O3                    \
           -ffast-math            \
           -fomit-frame-pointer   \
           $(VEC_FLAGS)           \
           -I./include            \
           -I./Roxane/RXRoxane    \
           -I./Roxane/GGS_Client  \
           -I./Roxane/Std_Client  \
           -Wall                  \
           -Wno-char-subscripts   \
           -Wno-reorder           \
           -Wno-misleading-indentation \
           -Wno-unused-private-field

LDFLAGS = $(OS_FLAGS) $(OS_LIBS)

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
    build/RXBBCountFlips_X86.o \
    build/RXBBDoFlips_X86.o    \
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

# ============================================================
# GGS Client
# ============================================================

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

# ============================================================
# Main
# ============================================================

build/main.o:             Roxane/main.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# ============================================================
# RXRoxane — code générique
# ============================================================

build/RXBBCountFlips_X86.o:   Roxane/RXRoxane/RXBBCountFlips_X86.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXBBDoFlips_X86.o:      Roxane/RXRoxane/RXBBDoFlips_X86.cpp
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

# ============================================================
# RXRoxane — SIMD x86_64 (AVX2 + BMI2) — flags explicites
# ============================================================

build/RXBBCountFlips_AVX2.o:  Roxane/RXRoxane/RXBBCountFlips_AVX2.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) $(SIMD_FLAGS) -MMD -MP -c $< -o $@

build/RXBBDoFlips_AVX2.o:     Roxane/RXRoxane/RXBBDoFlips_AVX2.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) $(SIMD_FLAGS) -MMD -MP -c $< -o $@

# ============================================================
# RXRoxane — SIMD ARM (NEON)
# ============================================================

build/RXBBCountFlips_NEON.o:  Roxane/RXRoxane/RXBBCountFlips_NEON.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/RXBBDoFlips_NEON.o:     Roxane/RXRoxane/RXBBDoFlips_NEON.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# ============================================================
# Std Client
# ============================================================

build/IOStdProtocol.o:        Roxane/Std_Client/IOStdProtocol.cpp
	@mkdir -p build && $(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/StdInput.o:             Roxane/Std_Client/StdInput.cpp
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
	@echo "SIMD_FLAGS: $(SIMD_FLAGS)"
	@echo "VEC_FLAGS : $(VEC_FLAGS)"
	@echo "SDK       : $(SDK)"
	@echo "OBJS      : $(words $(OBJS)) fichiers"
	@echo "SIMD_OBJS : $(SIMD_OBJS)"

-include $(wildcard build/*.d)

.PHONY: all clean cleanobj rebuild info
