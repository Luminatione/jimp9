FLAGS = -Wall -pedantic -Wextra
CCC = cc -c
CCO = cc -o
DEL = rm *.o

analize: main.o alex.o fun_stack.o parser.o
	$(CCO) analize main.o alex.o fun_stack.o parser.o
	$(DEL)
main.o: main.c parser.h
	$(CCC) main.c $(FLAGS)
alex.o: alex.c alex.h
	$(CCC) alex.c $(FLAGS)
parser.o: parser.c parser.h
	$(CCC) parser.c $(FLAGS)

test: fun_stack.o teststack_main.o teststack.o
	$(CCO) test fun_stack.o teststack.o teststack_main.o
	$(DEL)
teststack.o: teststack.c teststack.h fun_stack.h
	$(CCC) teststack.c
teststack_main.o: teststack_main.c teststack.h
	$(CCC) teststack_main.c
fun_stack.o: fun_stack.c fun_stack.h
	$(CCC) fun_stack.c