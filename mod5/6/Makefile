obj-m += kernel_space.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	make user_space

user_space.o: user_space.c
	gcc -c user_space.c -o user_space.o

user_space: user_space.o
	gcc user_space.o -o user_space
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f user_space.o
