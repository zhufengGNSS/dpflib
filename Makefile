dpf: dpf.c
	gcc -o dpf dpf.c -lm 
cpf: cpf.c
	gcc -o cpf cpf.c -lm 
tests: tests.c
	gcc -o tests tests.c -lm 
clean: 
	rm -fr cpf dpf *.dat
