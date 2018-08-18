a.out : main.o 
	g++ -o  a.out main.o

main.o : test.cpp vector.h allocate.h construct.h default_allocator.h  initialized.h iterator.h type_traits.h 
	g++ -o main.o -c -g test.cpp

clean :
	rm a.out main.o
