all: scheduler

scheduler: src.cpp
	g++ src.cpp -o scheduler
	
clean:
	@rm -f scheduler
