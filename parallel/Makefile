PROJECT			=	main

# everything depends on headers
HEADERS			=	$(wildcard *.h)

# dynamically generate a list of the required object files
SOURCES			=	$(wildcard *.c)
OBJECTS			=	$(SOURCES:%.c=%.o)

COMPILER		=	gcc -std=c99
CFLAGS			=	-fopenmp -Wall -pedantic -Werror

$(PROJECT) : $(OBJECTS)
	$(COMPILER) $(CFLAGS) -o $(PROJECT) $(OBJECTS)

# note: also added $(wildcard %.h) to support recompilation of a file if the
# header of the same name exists and has changed - e.g. execute.h
# changes -> update execute.o
%.o : %.c $(wildcard %.h) $(HEADERS)
	$(COMPILER) $(CFLAGS) -c $<

clean:
	rm -f $(PROJECT) $(OBJECTS)
