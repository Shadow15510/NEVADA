CC=gcc
CFLAGS=-I. `pkg-config --cflags gtk+-3.0 gdk-3.0 sdl2 SDL2_image SDL2_ttf`
LDFLAGS=`pkg-config --libs gtk+-3.0 gdk-3.0 sdl2 SDL2_image SDL2_ttf` -rdynamic -lnetcdf -lm

EXEC=nevada
SRC=src/main.c src/callbacks.c src/netcdf_api.c src/sdl_api.c src/colors.c
OBJ=$(SRC:%.c=build/%.o)

all: $(EXEC)

# Compilation Linux
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

main.o : include/main.h

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS)

# Nettoyage des sources
clean:
	rm -rf build	

clear:
	echo "Removing NEVADA\nAdministrator password required:"
	sudo bash uninstall.sh

install:
	echo -e "Installing NEVADA\nAdministrator password required:"
	sudo bash install.sh
