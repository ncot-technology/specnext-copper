# This is the name of your final .nex file without the .nex extension
PROJ_NAME=copper-test

# What's the image file for the emulator?
IMAGEFILE=/mnt/f/zxnext/cspect/sdcard.img

# List all your source locations here
CSOURCES = $(wildcard src/*.c) \
#$(wildcard src/l2gfx/*.c) \
#$(wildcard src/sprites/*.c) \
#$(wildcard src/utils/*.c) \
#$(wildcard src/input/*.c)

# These are for Z80 assembly files
ASOURCES = $(wildcard src/*.asm)

# sprites
SPRITEASSETS = $(wildcard assets/sprites/*.bmp)
SPRITEDEFS = $(wildcard assets/sprites/*.json)

#EXTRALIBS = libchai-input libchai-layer2 libchai-spritemanager libchai-sprites libchai-utils
#EXTRALIBS = libchai-layer2 libchai-spritemanager libchai-sprites

# Maybe you'll need to edit this
CRT=31
# 31

# You don't need to edit below here, have a nice day.

NEXT_DIR=$(PROJ_NAME)
EXEC_OUTPUT=build/$(PROJ_NAME)
MKDIR = mkdir -p
CC=zcc
AS=zcc
NEXTBMP=../zxn-tools/nextbmp
NEXTRAW=../zxn-tools/nextraw
MAKESPRITE=../zxn-tools/makesprite.py
TARGET=+zxn -subtype=nex
VERBOSITY=-v
OUT_DIR=build dist
PRAGMA_FILE=zpragma.inc

OBJS=$(CSOURCES:.c=.o) $(ASOURCES:.asm=.o)
SPRS=$(SPRITEASSETS:.bmp=.spr)
SPRDEFS=$(SPRITEDEFS:.json=.h)
ELIBS=$(addprefix -llib/zxn/,$(EXTRALIBS))

C_OPT_FLAGS=-SO3 --max-allocs-per-node200000 --opt-code-size -Ca-v -Cl-v

CFLAGS=$(TARGET) $(VERBOSITY) -c $(C_OPT_FLAGS) -compiler sdcc -clib=sdcc_iy -pragma-include:$(PRAGMA_FILE)
LDFLAGS=$(TARGET) $(VERBOSITY) -clib=sdcc_iy -pragma-include:$(PRAGMA_FILE) $(ELIBS)
ASFLAGS=$(TARGET) $(VERBOSITY) -c

EXEC=$(EXEC_OUTPUT).nex

%.o: %.c $(PRAGMA_FILE)
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $<

all : dirs $(EXEC) $(SPRS) $(SPRDEFS)

$(EXEC) : $(OBJS)
	$(CC) $(LDFLAGS) -startup=$(CRT) $(OBJS) -o $(EXEC_OUTPUT) -create-app

.PHONY: clean dirs install

%.spr: %.bmp
	$(NEXTBMP) $< $(addprefix /tmp/,$(notdir $<))
	$(NEXTRAW) -sep-palette $(addprefix /tmp/,$(notdir $<)) $@

%.h: %.json
	$(MAKESPRITE) $< $(addprefix sprite_defs_,$(notdir $@))

assets: $(SPRS) $(SPRDEFS)

install: all
#	cp $(SPRS) $(SPRS:.spr=.nxp) dist
	cp $(EXEC) dist
#	hdfmonkey put $(IMAGEFILE) dist/* $(PROJ_NAME)
#	hdfmonkey ls $(IMAGEFILE) $(PROJ_NAME)

clean:
	rm -f $(OBJS) $(SPRS) $(SPRS:.spr=.nxp)
	rm -rf $(OUT_DIR) /tmp/tmpXX*
	rm -f src/*.lis src/*.sym src/*.o

dirs: $(OUT_DIR)
#	-hdfmonkey mkdir $(IMAGEFILE) $(PROJ_NAME)

$(OUT_DIR):
	$(MKDIR) $(OUT_DIR)
