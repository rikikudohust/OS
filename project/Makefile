all: test shell

test: test.cpp
	g++ -o test test.cpp

shell: shell.cpp
	g++ -o shell shell.cpp

.PHONY: clean
clean:
	rm test shell
