run: main.c pre_processor.o  functionsP.o front_end.o first_pass.o second_pass.o backend.o
	gcc -ansi -pedantic -Wall main.c pre_processor.o functionsP.o front_end.o first_pass.o second_pass.o backend.o -o run

main.o: main.c main.h passes.h 
	gcc -c -ansi -pedantic -Wall main.c -o pre_processor.o

pre_processor.o: pre_processor.c main.h pre_processor.h
	gcc -c -ansi -pedantic -Wall pre_processor.c -o pre_processor.o

functionsP.o: functionsP.c main.h pre_processor.h
	gcc -c -ansi -pedantic -Wall functionsP.c -o functionsP.o

front_end.o: front_end.c main.h front_end.h pre_processor.h
	gcc -c -ansi -pedantic -Wall front_end.c -o front_end.o

first_pass.o: first_pass.c main.h passes.h front_end.h
	gcc -c -ansi -pedantic -Wall first_pass.c -o first_pass.o

second_pass.o: second_pass.c passes.h
	gcc -c -ansi -pedantic -Wall second_pass.c -o second_pass.o

backend.o: backend.c main.h passes.h
	gcc -c -ansi -pedantic -Wall backend.c -o backend.o