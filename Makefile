CC      = gcc
LIBS    = gtk+-3.0 cairo
CFLAGS  = -g -Wall -lpam -lX11
CFLAGS += `pkg-config $(LIBS) --cflags --libs`

PROGRAM = elysia
NAMES   = $(PROGRAM) Username Password Clock Frame TextImage WindowManager Panel Authenticate Interface Xsetup Utility Benchmark
SOURCES = $(addprefix ./src/login/src/, $(addsuffix .c, $(NAMES)))
HEADERS = $(addprefix ./src/login/hdr/, $(addsuffix .h, $(NAMES)))
OBJECTS = $(addprefix ./src/login/obj/, $(addsuffix .o, $(NAMES)))

all: $(PROGRAM)

./src/login/obj/%.o: ./src/login/src/%.c 
	$(CC) $(CFLAGS) \
		-c $< \
		-o $@ 

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) \
		-o $(PROGRAM) $(OBJECTS) 

.PHONY: clean
clean : 
	@rm -v -f $(OBJECTS)
	@rm -v -f $(PROGRAM)
