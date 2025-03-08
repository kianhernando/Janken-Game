all: Janken-Game

Janken-Game: background.cpp khernando.cpp ssantos.cpp jbowers.cpp ggregory.cpp stran.cpp 
	g++ background.cpp khernando.cpp ssantos.cpp jbowers.cpp ggregory.cpp stran.cpp -Wall -o Janken-Game ./libggfonts.a -lX11 -lGL -lGLU -lm

clean:
	rm -f Janken-Game


