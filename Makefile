COMPILER=gcc
OUTPUT_FILE=imdb
FILES=mediaFront.c mediaADT.c

all:
	$(COMPILER) -pedantic -std=c99 -Wall -fsanitize=address -o $(OUTPUT_FILE) $(FILES)

clean:
	rm $(OUTPUT_FILE)