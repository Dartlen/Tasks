
all:
	make libs
	gcc hello.c -L./obj/libs -lhello -lgoodbye -o test 
	
make libs:
	@mkdir -p obj
	gcc -c libhello.c -o obj/libhello.o
	gcc -c libgoodbye.c -o obj/libgoodbye.o
	@mkdir -p obj/libs
	ar rcs obj/libs/libhello.a obj/libhello.o
	ar rcs obj/libs/libgoodbye.a obj/libgoodbye.o
clean:
	@rm -rf obj
	@rm test
