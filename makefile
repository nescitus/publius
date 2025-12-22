SRCS=$(wildcard *.cpp)

publius: $(SRCS)
	g++ -o publius $(SRCS) -O3 -flto

clean:
	- rm *.o publius
