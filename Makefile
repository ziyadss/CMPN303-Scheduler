build:
	mkdir -p bin
	gcc src/process_generator.c -o bin/process_generator.out -Wall -Wextra
	gcc src/clk.c -o bin/clk.out -Wall -Wextra
	gcc src/scheduler.c -o bin/scheduler.out -Wall -Wextra -lm
	gcc src/process.c -o bin/process.out -Wall -Wextra
	gcc src/test_generator.c -o bin/test_generator.out -Wall -Wextra

debugbuild:
	mkdir -p bin
	gcc src/process_generator.c -o bin/process_generator.out -g -Wall -Wextra -fdiagnostics-color=always
	gcc src/clk.c -o bin/clk.out -g -Wall -Wextra -fdiagnostics-color=always
	gcc src/scheduler.c -o bin/scheduler.out -g -Wall -Wextra -fdiagnostics-color=always -lm
	gcc src/process.c -o bin/process.out -g -Wall -Wextra -fdiagnostics-color=always
	gcc src/test_generator.c -o bin/test_generator.out -g -Wall -Wextra -fdiagnostics-color=always

# -exec set follow-fork-mode child
# -exec set follow-fork-mode parent

clean:
	rm -fr bin  processes.txt

all: clean build

run:
	./bin/process_generator.out

hpf:
	./bin/process_generator.out 1

rr:
	./bin/process_generator.out 2

srtn:
	./bin/process_generator.out 3

generate:
	./bin/test_generator.out 5

memcheck:
	make debugbuild
	valgrind --leak-check=full --trace-children=yes --track-origins=yes ./bin/process_generator.out 3