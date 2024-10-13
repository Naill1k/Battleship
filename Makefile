CC = gcc
OPTIONS = -Wall -ansi -pedantic
EXECUTABLE = main

SOURCE = $(wildcard *.c)
OBJETS = $(SOURCE:.c=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJETS)
	$(CC) $(OPTIONS) $(OBJETS) -o $(EXECUTABLE)

%.o: %.c
	$(CC) $(OPTIONS) -MMD -c $<

clean:
	rm -f *.o *.d

-include *.d