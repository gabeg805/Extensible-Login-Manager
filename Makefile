# Compiler settings
# -----------------
CC      = gcc
PKGS    = gtk+-3.0 cairo
CFLAGS  = -g -Wall
CLIBS   = -lpam -lX11 -lutil `pkg-config $(PKGS) --cflags --libs`

# Program Files
# -------------
EXE = elm
LOG = /tmp/elm.log

# Directory paths
# ---------------
ROOT_DIR = .
OBJ_DIR  = $(ROOT_DIR)/obj
SRC_DIR  = $(ROOT_DIR)/src
INC_DIR  = $(ROOT_DIR)/include
OBJ_DIR  = $(ROOT_DIR)/obj
APP_SRC_DIR = $(ROOT_DIR)/src/app

# Source files
# ------------
SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(APP_SRC_DIR)/*.c)
OBJ = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC:.c=.o)))

# Make Rules
# ----------
CFLAGS += -I $(INC_DIR)

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) \
		-o $(EXE) $(OBJ) \
		$(CLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	$(CC) $(CFLAGS) \
		-c $< \
		-o $@ \
		$(CLIBS)

$(OBJ_DIR)/%.o: $(APP_SRC_DIR)/%.c 
	$(CC) $(CFLAGS) \
		-c $< \
		-o $@ \
		$(CLIBS)

.PHONY: all clean
clean : 
	@rm -v -f $(OBJ)/*.o
	@rm -v -f $(EXE)
	@rm -v -f $(LOG)
