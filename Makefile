# ------------------------------------------------------------------------------
# Project
PROJECT = elm

# ------------------------------------------------------------------------------
# Compiler settings
CC      = gcc
PKGS    = gtk+-3.0 cairo libsystemd
CFLAGS  = -g -Wall
LIBS   = -lpam -lpam_misc -lX11 -lXrandr -lutil `pkg-config $(PKGS) --cflags --libs`

# ------------------------------------------------------------------------------
# Directories
BUILDDIR  = $(CURDIR)
LOGDIR    = /var/log/$(PROJECT)
DATADIR   = $(BUILDDIR)/data
OBJDIR    = $(BUILDDIR)/obj
SRCDIR    = $(BUILDDIR)/src
INCDIR    = $(BUILDDIR)/include
OBJDIR    = $(BUILDDIR)/obj
APPSRCDIR = $(SRCDIR)/app
APPINCDIR = $(INCDIR)/app

# ------------------------------------------------------------------------------
# Files
SRC = $(wildcard $(SRCDIR)/*.c) $(wildcard $(APPSRCDIR)/*.c)
OBJ = $(addprefix $(OBJDIR)/, $(notdir $(SRC:.c=.o)))
LOG = /tmp/$(PROJECT).log

# ------------------------------------------------------------------------------
# Redefine compiler settings
CFLAGS += -I$(INCDIR) -I$(APPINCDIR)

# ------------------------------------------------------------------------------
# Targets
all: $(PROJECT)

$(PROJECT): $(OBJ)
	$(CC) $(CFLAGS) \
		-o $(PROJECT) $(OBJ) \
		$(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c 
	$(CC) $(CFLAGS) \
		-c $< \
		-o $@ \
		$(LIBS)

$(OBJDIR)/%.o: $(APPSRCDIR)/%.c 
	$(CC) $(CFLAGS) \
		-c $< \
		-o $@ \
		$(LIBS)

.PHONY: all clean install uninstall
clean : 
	@rm -v -f $(OBJDIR)/*.o
	@rm -v -f $(PROJECT)
	@rm -v -f $(LOG)

install:
	@echo ":: Installing '$(PROJECT)'."
	@ln -svf $(CURDIR)/$(PROJECT) /usr/bin/
	@cp -av $(DATADIR)/$(PROJECT).pam /etc/pam.d/$(PROJECT)
	@cp -av $(DATADIR)/$(PROJECT).service /usr/lib/systemd/system/
	@systemctl enable $(PROJECT).service
	@mkdir -pv $(LOGDIR)

uninstall:
	@echo ":: Uninstalling '$(PROJECT)'."
	@rm -v /usr/bin/$(PROJECT)
	@rm -v /etc/pam.d/$(PROJECT)
	@rm -v /usr/lib/systemd/system/$(PROJECT).service
	@systemctl disable $(PROJECT).service
	@rm -rvf $(LOGDIR)
