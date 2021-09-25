
SHELL = /bin/bash
include makefile.configure

SRC_DIR   := src/
BUILD_DIR := build/
OBJ_DIR   := $(BUILD_DIR)obj/
GEN_HEAD  := $(BUILD_DIR)include/
INC_DIR   := src/ $(GEN_HEAD)

SRC := $(foreach dir, $(INC_DIR), $(notdir $(wildcard $(dir)*.c)))
INC := $(foreach dir, $(INC_DIR), $(notdir $(wildcard $(dir)*.h)))
OBJ := $(SRC:%.c=$(OBJ_DIR)%.o)

CC  := gcc
LD  := gcc

CFLAGS += -std=gnu17 -pedantic -Wall $(OPT)
CFLAGS += $(foreach dir, $(INC_DIR), -iquote $(dir))
LDFLAGS = $(OPT) $(LDLIBS)

licht := $(BUILD_DIR)licht
BIN   := $(licht)

MKFILES := $(wildcard makefile*)


##############
## COMMANDS ##
##############
all: $(BIN)

install: all

clean:
	$(ECHO) Cleaning...
	$(CMD)rm -rf $(BUILD_DIR)

.SECONDEXPANSION:

$(licht): licht.o util.o
$(licht): LDLIBS := -pthread -lm

licht.o: config.h

$(GEN_HEAD)config.h: $(MKFILES) | $$(@D)/
	$(ECHO) "Generate \e[1;33m$@\e[0m..."
	$(CMD)bash ./gen-header.sh cfg $@

$(OBJ): $(OBJ_DIR)%.o: %.c $(INC) $(MKFILES) | $$(@D)/
	$(ECHO) "\e[1;32m$<\e[0m\t==> \e[1;31m$@\e[0m"
	$(CMD)$(strip $(CC) $(CFLAGS) -c $< -o $@)

$(BIN): %:
	$(ECHO) "Linking \e[1m$@\e[0m (\e[1;31m$(notdir $^)\e[0m)"
	$(CMD)$(strip $(LD) $(LDFLAGS) $(LDLIBS) $^ -o $@)

%/:
	$(ECHO) "Make \e[1;34m$@\e[0m..."
	$(CMD)mkdir -p $@

	


.PHONY: all clean install
.SUFFIXES:
.DELETE_ON_ERROR:
vpath %.c  $(SRC_DIR)
vpath %.h  $(SRC_DIR) $(INC_DIR)
vpath %.o  $(OBJ_DIR)
