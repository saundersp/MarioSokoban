CC := g++ -std=c++17
SRC :=  src/main.cpp\
		src/loader.cpp\
		src/main_menu.cpp\
		src/game.cpp\
		src/credits.cpp\
		src/options_menu.cpp\
		src/editor.cpp
RELEASE := bin/game.exe
DEBUG := bin/game_debug.exe
ICON_SRC := src/icon.rc
ICON_OBJ := bin/icon.res
CFLAGS := -Wall -Werror -Werror=implicit-fallthrough=0 -Wextra -DDEBUG -g -D __DEBUG__ --wrapv
LFLAGS := -L./lib -I./include -I./src/headers
LLFLAGS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
PRECOMPILED_HEADER_SOURCE := src/headers/libraries.hpp
PRECOMPILED_HEADER = $(PRECOMPILED_HEADER_SOURCE).gch

# target: dependances list
# $@ : $^ (or $< for first)
# Source: (https://www.labri.fr/perso/billaud/Resources/resources/AP2-POO-0910/060-faire-makefile.pdf)

all: $(DEBUG)

release: $(RELEASE)

debug: $(DEBUG)

$(RELEASE): $(SRC) $(ICON_OBJ) $(PRECOMPILED_HEADER)
	$(CC) $(SRC) ${LLFLAGS} ${LFLAGS} -o $@ $(ICON_OBJ) -mwindows

$(DEBUG): $(SRC) $(ICON_OBJ) $(PRECOMPILED_HEADER)
	$(CC) $(SRC) ${CFLAGS} $(LFLAGS) $(LLFLAGS) -o $@

$(PRECOMPILED_HEADER):
	$(CC) -c $(PRECOMPILED_HEADER_SOURCE) $(CFLAGS) $(LFLAGS) $(LLFLAGS) -o $@

# Perprocessor doc (https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html)

$(ICON_OBJ): $(ICON_SRC)
	windres $^ -O coff $@

clean:
	rm $(RELEASE) | rm $(DEBUG) | true

cleanall: clean
	rm $(PRECOMPILED_HEADER) | rm $(ICON_OBJ) | true