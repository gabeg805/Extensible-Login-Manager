CC      = gcc
LIBS    = gtk+-3.0 cairo
CFLAGS  = -w -lpam -lX11 `pkg-config $(LIBS) --cflags --libs`
# CFLAGS  = -g -Wall -lpam -lX11 `pkg-config $(LIBS) --cflags --libs`

HDR_DIR = ./src/login/hdr
OBJ_DIR = ./src/login/obj
SRC_DIR = ./src/login/src

PROG = elysia
NAMES   = $(PROG) Username Password Clock Frame TextImage WindowManager Panel Authenticate Interface Xsetup utility benchmark
HDR = $(addprefix ${HDR_DIR}/, $(addsuffix .h, ${NAMES}))
OBJ = $(addprefix ${OBJ_DIR}/, $(addsuffix .o, ${NAMES}))
SRC = $(addprefix ${SRC_DIR}/, $(addsuffix .c, ${NAMES}))

CFLAGS += -I ${HDR_DIR}

all: ${PROG}

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c 
	${CC} ${CFLAGS} \
		-c $< \
		-o $@ 

${PROG}: ${OBJ}
	${CC} ${CFLAGS} \
		-o ${PROG} ${OBJ} 

.PHONY: clean
clean : 
	@rm -v -f ${OBJ}
	@rm -v -f ${PROG}
