CC=gcc
RM=rm -f

CFLAGS=-W -Wall -std=c99 -Ofast

SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
BASENAMES=$(basename $(SOURCES)) 
OBJECTS=$(addsuffix .o , $(BASENAMES))
EXEC=main

all: $(EXEC)

.c.o:
	@echo -e "\033[1;33m=== COMPILING $*.c INTO $@ ===\033[0m"
	$(CC) $(CFLAGS) -c $*.c
	@echo

$(EXEC): $(OBJECTS)
	@echo -e "\033[1;32m=== LINKING OBJECTS INTO $(EXEC) ===\033[0m"
	$(CC) $(CFLAGS) $^ -o $@

clean:
	@echo -e "\033[1;31m=== CLEANING OBJECTS ===\033[0m"
	$(RM) $(OBJECTS) $(EXEC)

display:
	@echo -e "SOURCES:" $(SOURCES)
	@echo -e "HEADERS:" $(HEADERS)
	@echo -e "BASENAMES:" $(BASENAMES)
	@echo -e "OBJECTS:" $(OBJECTS)
	@echo -e "EXEC:" $(EXEC)

depend: $(SOURCES) $(HEADERS)
	@echo -e "\033[1;34m=== UPDATING DEPENDENCIES ===\033[0m"
	gcc $(CFLAGS) -MM $(SOURCES) | tee depend

-include depend
