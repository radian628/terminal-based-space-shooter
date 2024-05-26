tbss: main.o dynarray.o 
	gcc main.o dynarray.o -o tbss 

main.o: dynarray.h dynarray.c
	gcc main.c -c

dynarray.o: dynarray.h dynarray.c
	gcc dynarray.c -c

clean:
	rm tbss *.o