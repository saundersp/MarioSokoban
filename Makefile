CC := g++ -std=c++17 -m64
SRC_DIR := src
OBJ_DIR := out
$(shell mkdir -p $(OBJ_DIR))
BIN_DIR := bin
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
RELEASE := $(BIN_DIR)/game
DEBUG := $(BIN_DIR)/game_debug
ICON_SRC := $(SRC_DIR)/icon.rc
ICON_OBJ := $(OBJ_DIR)/icon.res
CFLAGS := -Wall -Werror -Werror=implicit-fallthrough=0 -Wextra -DDEBUG -g -D __DEBUG__ --wrapv
LFLAGS := -L./lib -I./include -I./src -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image

ifeq ($(OS),Windows_NT)
	RELEASE := $(RELEASE).exe
	DEBUG := $(DEBUG).exe
	LFLAGS := -lmingw32 $(LFLAGS)
endif

.PHONY: all release debug start clean mrproper

all: $(DEBUG)

debug: $(DEBUG)

release: $(RELEASE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo Compiling $<
	@$(CC) -c $< $(CFLAGS) $(LFLAGS) -o $@

$(DEBUG): $(OBJ)
	@echo Linking objects files to $@
	@$(CC) $^ ${CFLAGS} ${LFLAGS} -o $@

ifeq ($(OS),Windows_NT)
$(ICON_OBJ): $(ICON_SRC)
	@echo Creating icon $<
	@windres $^ -O coff $@

$(RELEASE): $(OBJ) $(ICON_OBJ)
	@echo Compiling $<
	@$(CC) $^ -O4 ${LFLAGS} -o $@ -mwindows
else
$(RELEASE): $(OBJ)
	@echo Compiling $<
	@$(CC) $^ -O4 ${LFLAGS} -o $@
endif

start: $(DEBUG)
	@cd $(BIN_DIR) && ./$(shell basename $(DEBUG))

clean:
	@rm $(RELEASE) $(DEBUG) | true

mrproper: clean
	@rm $(ICON_OBJ) $(OBJ) | true
