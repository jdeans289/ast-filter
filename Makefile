CC = c++

all: ast-filter

ast-filter: main.cpp ast_filter.a
	$(CC) -o $@ $^ 

ast_filter.a: ast_filter.o
	ar rcs $@ $^

ast_filter.o: ast_filter.cpp
	$(CC) -c -o $@ $^

clean:
	rm -f *.o *.a ast-filter