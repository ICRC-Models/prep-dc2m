CFLAGS = -O3 --std=c++11 -g
CC = clang++

popTest: distributeART.o distributeCondoms.o addBirths.o subtractDeaths.o csvUtil.o
	$(CC) $(CFLAGS) -o popTest main.cpp distributeART.o distributeCondoms.o addBirths.o subtractDeaths.o csvUtil.o

distributeART.o: distributeART.cpp
	$(CC) $(CFLAGS) -c distributeART.cpp

distributeCondoms.o: distributeCondoms.cpp
	$(CC) $(CFLAGS) -c distributeCondoms.cpp

addBirths.o: addBirths.cpp
	$(CC) $(CFLAGS) -c addBirths.cpp

subtractDeaths.o: subtractDeaths.cpp
	$(CC) $(CFLAGS) -c subtractDeaths.cpp

csvUtil.o: csvUtil.cpp
	$(CC) $(CFLAGS) -c csvUtil.cpp


clean:
	rm -f popTest *.o