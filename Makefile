run: main
	./main 

main: 
	g++ -std=c++14 -g -o main main.cc

.PHONY:
	clean 

clean:
	rm -f main
