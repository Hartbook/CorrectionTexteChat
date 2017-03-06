CC=gcc
RM=rm -f

CFLAGS=-W -Wall -std=c99 -g3
LFLAGS=-lpthread

SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
BASENAMES=$(basename $(SOURCES)) 
OBJECTS=$(addsuffix .o , $(BASENAMES))
OBJECTSLOC=$(addprefix objects/, $(addsuffix .o, $(BASENAMES)))
EXEC=../main

all: $(EXEC)

objects/%.o: %.c
	@echo -e "\033[1;33m=== COMPILING $*.c INTO $@ ===\033[0m"
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo

$(EXEC): $(OBJECTSLOC)
	@echo -e "\033[1;32m=== LINKING OBJECTS INTO $(EXEC) ===\033[0m"
	$(CC) $(LFLAGS) $^ -o $@

clean:
	@echo -e "\033[1;31m=== CLEANING OBJECTS ===\033[0m"
	$(RM) $(OBJECTSLOC) $(EXEC)

display:
	@echo -e "SOURCES:" $(SOURCES)
	@echo -e "HEADERS:" $(HEADERS)
	@echo -e "BASENAMES:" $(BASENAMES)
	@echo -e "OBJECTS:" $(OBJECTS)
	@echo -e "OBJECTSLOC:" $(OBJECTSLOC)
	@echo -e "EXEC:" $(EXEC)

depend: $(SOURCES) $(HEADERS)
	@echo -e "\033[1;34m=== UPDATING DEPENDENCIES ===\033[0m"
	gcc $(CFLAGS) -MM $(SOURCES) | tee depend

-include depend
