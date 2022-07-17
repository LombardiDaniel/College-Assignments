main.out: main.c queue.h
	gcc -Wall -pedantic -Wextra -O0 -g main.c -o main.out -lpthread

asm: main.c queue.h
	gcc -Wall -pedantic -Wextra -O0 -g main.c -o main.asm -S -lpthread

clean:
	rm -r *.o *.asm *.out ./main.out.dSYM
