CC      = gcc
LIBS    = gtk+-3.0 cairo
CFLAGS  = -g -Wall -lpam
CFLAGS += `pkg-config $(LIBS) --cflags --libs`

PROGRAM = glm
NAMES   = $(PROGRAM) Username Password Interface Authenticate CommandLine OpenDisplay Transparency Clock Frame TextImage WindowManager Power
SOURCES = $(addprefix ./src/, $(addsuffix .c, $(NAMES)))
HEADERS = $(addprefix ./hdr/, $(addsuffix .h, $(NAMES)))
OBJECTS = $(addprefix ./obj/, $(addsuffix .o, $(NAMES)))

all: $(PROGRAM)

./obj/%.o: ./src/%.c 
	$(CC) $(CFLAGS) -o $@ -c $<

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(OBJECTS)

.PHONY: clean
clean : 
	-rm -f $(OBJECTS)
