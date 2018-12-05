CFLAGS = -O3 --std=c++11 -g
CC = clang++

popTest: distributeART.o distributeCondoms.o addBirths.o subtractDeaths.o agePop.o progressDisease.o transmit.o csvUtil.o
	$(CC) $(CFLAGS) -o popTest main.cpp distributeART.o distributeCondoms.o addBirths.o subtractDeaths.o agePop.o progressDisease.o transmit.o csvUtil.o

distributeART.o: distributeART.cpp
	$(CC) $(CFLAGS) -c distributeART.cpp

distributeCondoms.o: distributeCondoms.cpp
	$(CC) $(CFLAGS) -c distributeCondoms.cpp

addBirths.o: addBirths.cpp
	$(CC) $(CFLAGS) -c addBirths.cpp

subtractDeaths.o: subtractDeaths.cpp
	$(CC) $(CFLAGS) -c subtractDeaths.cpp

agePop.o: agePop.cpp
	$(CC) $(CFLAGS) -c agePop.cpp

progressDisease.o: progressDisease.cpp
	$(CC) $(CFLAGS) -c progressDisease.cpp

transmit.o: transmit.cpp
	$(CC) $(CFLAGS) -c transmit.cpp

csvUtil.o: csvUtil.cpp
	$(CC) $(CFLAGS) -c csvUtil.cpp


clean:
	rm -f popTest *.o