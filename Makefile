# =================
# Compiler settings
# =================

CC      = gcc
PKGS    = gtk+-3.0 cairo
CFLAGS  = -g -Wall
CLIBS   = -lpam -lX11 `pkg-config ${PKGS} --cflags --libs`
CMACROS = 

# Macros
# BENCHMARK = 
BENCHMARK = -D BENCHMARK
# DEBUG     = 
DEBUG     = -D DEBUG

CMACROS += ${BENCHMARK} ${DEBUG}



# ===============
# Directory paths
# ===============

APP_DIR  = ./app
CORE_DIR = ./core
LIB_DIR  = ./lib
OBJ_DIR  = ./obj

APP_SRC_DIR  = ${APP_DIR}/src
CORE_SRC_DIR = ${CORE_DIR}/src
LIB_SRC_DIR  = ${LIB_DIR}/src

APP_HDR_DIR  = ${APP_DIR}/hdr
CORE_HDR_DIR = ${CORE_DIR}/hdr
LIB_HDR_DIR  = ${LIB_DIR}/hdr

CFLAGS += -I ${APP_HDR_DIR} -I ${CORE_HDR_DIR} -I ${LIB_HDR_DIR}



# ================
# Program settings
# ================

PROG  = elysia
AUTH  = authenticate
GUI   = interface xserver
APPS  = login frame clock windowmanager panel  
UTIL  = benchmark elyapp elyconfig utility 
FILES = ${PROG} ${AUTH} ${GUI} ${APPS} ${UTIL}
OBJ   = $(addprefix ${OBJ_DIR}/, $(addsuffix .o, ${FILES}))



# ==========
# Make Rules
# ==========

all: ${PROG}

${PROG}: ${OBJ}
	${CC} ${CFLAGS} ${CMACROS} \
		-o ${PROG} ${OBJ} \
		${CLIBS}

${OBJ_DIR}/%.o: ${APP_SRC_DIR}/%.c 
	${CC} ${CFLAGS} ${CMACROS} \
		-c $< \
		-o $@ \
		${CLIBS}

${OBJ_DIR}/%.o: ${CORE_SRC_DIR}/%.c 
	${CC} ${CFLAGS} ${CMACROS} \
		-c $< \
		-o $@ \
		${CLIBS}

${OBJ_DIR}/%.o: ${LIB_SRC_DIR}/%.c 
	${CC} ${CFLAGS} ${CMACROS} \
		-c $< \
		-o $@ \
		${CLIBS}

.PHONY: all clean
clean : 
	@rm -v -f ${OBJ}
	@rm -v -f ${PROG}
