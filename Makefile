all: Janken-Game

Janken-Game: 
	g++ background.cpp khernando.cpp ssantos.cpp jbowers.cpp ggregory.cpp stran.cpp -Wall -lX11 -lGL -lGLU -lm -o Janken-Game

clean:
	rm -f Janken-Game


