assembler: output_writer.o transitions.o input_reader.o data_structures.o utils.o assembler.o
	gcc -g -ansi -Wall -pedantic output_writer.o transitions.o input_reader.o data_structures.o utils.o assembler.o -o assembler -lm

assembler.o: assembler.c input_reader.h definitions.h transitions.h output_writer.h utils.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

output_writer.o: input_reader.h definitions.h utils.h
	gcc -c -ansi -Wall -pedantic output_writer.c -o output_writer.o -lm

transitions.o: input_reader.h utils.h
	gcc -c -ansi -Wall -pedantic transitions.c -o transitions.o -lm

input_reader.o: data_structures.h utils.h
	gcc -c -ansi -Wall -pedantic input_reader.c -o input_reader.o

data_structures.o: data_structures.c definitions.h
	gcc -c -ansi -Wall -pedantic data_structures.c -o data_structures.o

utils.o: utils.c
	gcc -c -ansi -Wall -pedantic utils.c -o utils.o -lm

