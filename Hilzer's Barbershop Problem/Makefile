main.out: main.c
	gcc -Wall -pedantic -Wextra -O2 -g main.c -o main.out -lpthread

asm: main.c
	gcc -Wall -pedantic -Wextra -O0 -g main.c -o main.asm -lpthread -S

clean:
	rm -r *.o *.asm *.out ./main.out.dSYM
