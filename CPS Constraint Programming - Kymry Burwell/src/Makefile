DIR=/usr/local
LIBS= \
   -lgecodeflatzinc  -lgecodedriver \
   -lgecodesearch  \
   -lgecodeminimodel -lgecodeset    \
   -lgecodefloat     -lgecodeint    \
   -lgecodekernel    -lgecodesupport

box: box.cpp
	g++ -std=c++11 -Wall -O3 -I$(DIR)/include -c box.cpp
	g++ -std=c++11 -Wall -O3 -L$(DIR)/lib -o box box.o $(LIBS)

checker: checker.cpp
	g++ -std=c++11 -Wall -I$(DIR)/include -c checker.cpp
	g++ -std=c++11 -Wall -L$(DIR)/lib -o checker checker.o $(LIBS)
