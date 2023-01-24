OPCIONS = -g -O0 -Wall -Wno-deprecated -std=c++14

program.exe: racional.o token.o expressio.o variables.o math_sessio.o math_io.o driver_esinmath.o
	g++ -o program.exe racional.o token.o expressio.o variables.o math_sessio.o math_io.o driver_esinmath.o -lesin
	rm *.o

racional.o: racional.cpp racional.hpp racional.rep
	g++ -c racional.cpp $(OPCIONS)

token.o: token.cpp token.hpp token.rep
	g++ -c token.cpp $(OPCIONS)

expressio.o: expressio.cpp expressio.hpp expressio.rep
	g++ -c expressio.cpp $(OPCIONS)

variables.o: variables.cpp variables.hpp variables.rep
	g++ -c variables.cpp $(OPCIONS)

math_sessio.o: math_sessio.cpp math_sessio.hpp math_sessio.rep
	g++ -c math_sessio.cpp $(OPCIONS)

math_io.o: math_io.cpp math_io.hpp
	g++ -c math_io.cpp $(OPCIONS)

driver_esinmath.o: driver_esinmath.cpp racional.hpp racional.rep token.hpp token.rep expressio.hpp expressio.rep variables.hpp variables.rep math_sessio.hpp math_sessio.rep math_io.hpp
	g++ -c driver_esinmath.cpp $(OPCIONS)

clean:
	rm *.o
	rm *.exe

test_total:
	./program.exe < jp_public_racional.in | diff - jp_public_racional.res;
	./program.exe < jp_public_token.in | diff - jp_public_token.res;
	./program.exe < jp_public_expressio.in | diff - jp_public_expressio.res;
	./program.exe < jp_public_variables.in | diff - jp_public_variables.res;
	./program.exe < jp_public_math_sessio.in | diff - jp_public_math_sessio.res;
	./program.exe < jp_public_math_io.in | diff - jp_public_math_io.res;
	./program.exe < jp_public.in | diff - jp_public.res;

test:
	./program.exe < jp_public.in | diff - jp_public.res;

test_expressio:
	./program.exe < jp_public_expressio.in | diff - jp_public_expressio.res;

test_math_io:
	./program.exe < jp_public_math_io.in | diff - jp_public_math_io.res;

test_math_sessio:
	./program.exe < jp_public_math_sessio.in | diff - jp_public_math_sessio.res;

test_racional:
	./program.exe < jp_public_racional.in | diff - jp_public_racional.res;

test_token:
	./program.exe < jp_public_token.in | diff - jp_public_token.res;

test_variables:
	./program.exe < jp_public_variables.in | diff - jp_public_variables.res;

memcheck_total:
	valgrind --leak-check=full -s ./program.exe < jp_public_racional.in | diff - jp_public_racional.res;
	valgrind --leak-check=full -s ./program.exe < jp_public_token.in | diff - jp_public_token.res;
	valgrind --leak-check=full -s ./program.exe < jp_public_expressio.in | diff - jp_public_expressio.res;
	valgrind --leak-check=full -s ./program.exe < jp_public_variables.in | diff - jp_public_variables.res;
	valgrind --leak-check=full -s ./program.exe < jp_public_math_sessio.in | diff - jp_public_math_sessio.res;
	valgrind --leak-check=full -s ./program.exe < jp_public_math_io.in | diff - jp_public_math_io.res;
	valgrind --leak-check=full -s ./program.exe < jp_public.in | diff - jp_public.res;

memcheck:
	valgrind --leak-check=full -s ./program.exe < jp_public.in | diff - jp_public.res;

memcheck_expressio:
	valgrind --leak-check=full -s ./program.exe < jp_public_expressio.in | diff - jp_public_expressio.res;

memcheck_math_io:
	valgrind --leak-check=full -s ./program.exe < jp_public_math_io.in | diff - jp_public_math_io.res;

memcheck_math_sessio:
	valgrind --leak-check=full -s ./program.exe < jp_public_math_sessio.in | diff - jp_public_math_sessio.res;
	
memcheck_racional:
	valgrind --leak-check=full -s ./program.exe < jp_public_racional.in | diff - jp_public_racional.res;

memcheck_token:
	valgrind --leak-check=full -s ./program.exe < jp_public_token.in | diff - jp_public_token.res;

memcheck_variables:
	valgrind --leak-check=full -s ./program.exe < jp_public_variables.in | diff - jp_public_variables.res;
