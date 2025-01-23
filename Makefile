all: src

src: src.cpp
	g++ src.cpp -o lab6
	
clean:
	@rm -f lab6
