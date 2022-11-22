main.out: main.c headers/benchmarker.h benchmarker.c
	gcc -Wall -pedantic -Wextra -O0 -g main.c benchmarker.c -o main.out -lpthread -w

asm: main.c headers/benchmarker.h benchmarker.c
	gcc -Wall -pedantic -Wextra -O0 -g main.c benchmarker.c -o main.asm -S -lpthread -w

clean:
	rm -r *.o *.asm *.out ./main.out.dSYM
