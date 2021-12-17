build:
	mkdir -p bin
	gcc src/process_generator.c -o bin/process_generator.out -O3 -Wall -Wextra
	gcc src/clk.c -o bin/clk.out -O3 -Wall -Wextra
	gcc src/scheduler.c -o bin/scheduler.out -O3 -Wall -Wextra
	gcc src/process.c -o bin/process.out -O3 -Wall -Wextra
	gcc src/test_generator.c -o bin/test_generator.out -O3 -Wall -Wextra

debugbuild:
	mkdir -p bin
	gcc src/process_generator.c -o bin/process_generator.out -g -Wall -Wextra -fdiagnostics-color=always
	gcc src/clk.c -o bin/clk.out -g -Wall -Wextra -fdiagnostics-color=always
	gcc src/scheduler.c -o bin/scheduler.out -g -Wall -Wextra -fdiagnostics-color=always
	gcc src/process.c -o bin/process.out -g -Wall -Wextra -fdiagnostics-color=always
	gcc src/test_generator.c -o bin/test_generator.out -g -Wall -Wextra -fdiagnostics-color=always

# -exec set follow-fork-mode child
# -exec set follow-fork-mode parent

clean:
	rm -fr bin  processes.txt

all: clean build

run:
	./bin/process_generator.out

generate:
	./bin/test_generator.out 5