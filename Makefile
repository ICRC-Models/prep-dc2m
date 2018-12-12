CFLAGS = -O3 --std=c++11 -g
CC = clang++

runModel: distributeART.o distributeCondoms.o addBirths.o subtractDeaths.o agePop.o progressDisease.o transmit.o endPop.o riskAdjust.o csvUtil.o globals.o
	$(CC) $(CFLAGS) -o runModel runModel.cpp distributeART.o distributeCondoms.o addBirths.o subtractDeaths.o agePop.o progressDisease.o transmit.o endPop.o riskAdjust.o csvUtil.o globals.o

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

endPop.o: endPop.cpp
	$(CC) $(CFLAGS) -c endPop.cpp

riskAdjust.o: riskAdjust.cpp
	$(CC) $(CFLAGS) -c riskAdjust.cpp

csvUtil.o: csvUtil.cpp
	$(CC) $(CFLAGS) -c csvUtil.cpp

globals.o: globals.cpp
	$(CC) $(CFLAGS) -c globals.cpp


clean:
	rm -f runModel *.o