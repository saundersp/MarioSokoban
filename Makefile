PREFIX :=
CC := $(PREFIX)g++
SRC_DIR := src
OBJ_DIR := out
BIN_DIR := bin
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
RELEASE := $(BIN_DIR)/game
DEBUG := $(BIN_DIR)/game_debug
ICON_SRC := $(SRC_DIR)/icon.rc
ICON_OBJ := $(OBJ_DIR)/icon.res
CPPFLAGS := -DDEBUG -D __DEBUG__ --wrapv -I./src -MMD -MP
CFLAGS := -m64 -std=c++17 -g -Wall -Werror -Wextra
LDLIBS := $(shell $(PREFIX)pkg-config --cflags --libs sdl3)
LDLIBS := $(LDLIBS) $(shell $(PREFIX)pkg-config --cflags --libs sdl3-image)
LDLIBS := $(LDLIBS) $(shell $(PREFIX)pkg-config --cflags --libs sdl3-ttf)

WINDOWS := 0
ifeq ($(OS),Windows_NT)
	WINDOWS := 1
endif
ifeq ($(PREFIX),x86_64-w64-mingw32-)
	WINDOWS := 1
endif

ifeq ($(WINDOWS),1)
	RELEASE := $(RELEASE).exe
	DEBUG := $(DEBUG).exe
	LDLIBS := -static-libgcc -static-libstdc++ -lmingw32 $(LDLIBS)
endif

.PHONY: all
all: $(DEBUG)

.PHONY: debug
debug: $(DEBUG)

.PHONY: release
release: $(RELEASE)

$(OBJ_DIR):
	@mkdir -v $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo Compiling $<
	@$(CC) -c $< $(LDLIBS) $(CPPFLAGS) $(CFLAGS) -o $@

$(DEBUG): $(OBJ)
	@echo Linking objects files to $@
	@$(CC) $^ $(LDLIBS) -o $@

ifeq ($(WINDOWS),1)
$(ICON_OBJ): $(ICON_SRC)
	@echo Creating icon $<
	@$(PREFIX)windres $^ -O coff $@

$(RELEASE): $(OBJ) $(ICON_OBJ)
	@echo Linking objects files to $@
	@$(CC) $^ -O2 $(LDLIBS) -o $@ -mwindows
else
$(RELEASE): $(OBJ)
	@echo Linking objects files to $@
	@$(CC) $^ -O2 $(LDLIBS) -o $@
endif

.PHONY: start
start: $(DEBUG)
ifeq ($(WINDOWS),1)
	@cd $(BIN_DIR) && wine ./$(shell basename $(DEBUG))
else
	@cd $(BIN_DIR) && ./$(shell basename $(DEBUG))
endif

.PHONY: check
check: $(DEBUG)
	@cd $(BIN_DIR) && valgrind -q --leak-check=full --show-leak-kinds=all ./$(shell basename $(DEBUG))

.PHONY: clean
clean:
	@rm -fv $(RELEASE) $(DEBUG)

.PHONY: mrproper
mrproper: clean
	@rm -frv $(ICON_OBJ) $(OBJ_DIR)

-include $(OBJ:.o=.d)
