# target goals
prog_stat : prog_stat.o libfuncs.a
	gcc prog_stat.o -L. -lfuncs -o prog_stat
	make clean
prog_dyn : prog_dyn.o libfuncsd.so
	gcc prog_dyn.o -L. -lfuncsd -o prog_dyn
	make clean

# object files
prog_stat.o : prog_stat.c
	gcc -c prog_stat.c
prog_dyn.o : prog_dyn.c
	gcc -c prog_dyn.c
functions.o : functions.c
	gcc -c -fPIC functions.c

# libraries
libfuncs.a : functions.o
	ar rc libfuncs.a functions.o
	ranlib libfuncs.a
libfuncsd.so : functions.o
	gcc -shared functions.o -o libfuncsd.so

# cleanup after compilation
clean :
	rm -f *.o *.a