# =================
# Compiler settings
# =================
CC      = gcc
PKGS    = gtk+-3.0 cairo
CFLAGS  = -w # -g -Wall
CLIBS   = -lpam -lX11 `pkg-config ${PKGS} --cflags --libs`


# ===============
# Directory paths
# ===============
HDR_DIR = ./src/login/hdr
OBJ_DIR = ./src/login/obj
SRC_DIR = ./src/login/src

CFLAGS += -I ${HDR_DIR}


# ================
# Program settings
# ================
PROG  = elysia
AUTH  = authenticate
GUI   = interface
APPS  = clock frame panel password textimage username windowmanager 
UTIL  = benchmark elyapp elyconfig elyx utility 
FILES = ${PROG} ${AUTH} ${GUI} ${APPS} ${UTIL}
OBJ   = $(addprefix ${OBJ_DIR}/, $(addsuffix .o, ${FILES}))



# ==========
# Make Rules
# ==========
all: ${PROG}

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c 
	${CC} ${CFLAGS} \
		-c $< \
		-o $@ \
		${CLIBS}

${PROG}: ${OBJ}
	${CC} ${CFLAGS} \
		-o ${PROG} ${OBJ} \
		${CLIBS}

.PHONY: clean
clean : 
	@rm -v -f ${OBJ}
	@rm -v -f ${PROG}
