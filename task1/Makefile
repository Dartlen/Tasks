
all:
	make libs
	gcc hello.c -L./obj/libs -lhello -o test -lgoodbye 
	
make libs:
	@mkdir -p obj
	gcc -c -fPIC libgoodbye.c -o obj/libgoodbye.o
	gcc -c libhello.c -o obj/libhello.o
	@mkdir -p obj/libs
	gcc obj/libgoodbye.o -shared -o obj/libs/libgoodbye.so
	ar rcs obj/libs/libhello.a obj/libhello.o
	
clean:
	@rm -rf obj
	@rm test
