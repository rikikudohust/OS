all: foreground shell background

foreground: foreground.cpp
	clang++ -o foreground foreground.cpp

background: background.cpp
	clang++ -o background background.cpp 

shell: shell.cpp
	clang++ -o shell shell.cpp

.PHONY: clean
clean:
	rm shell foreground background
