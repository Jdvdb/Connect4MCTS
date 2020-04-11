CC=g++
CFLAGS=-c -Wall -O2 -std=c++11
LFLAGS=
PROGS=connectFourComputer
OBJS=connectFourComputer.o ConnectFour.o mcts.o ConnectNode.o serialport.o

$(PROGS): $(OBJS)
	$(CC) $(OBJS) -o $(PROGS) $(LFLAGS)

mcts.o: mcts.cpp mcts.h ConnectFour.h ConnectNode.h
	$(CC) mcts.cpp -o mcts.o $(CFLAGS)

serialport.o: serialport.cpp serialport.h
	$(CC) serialport.cpp -o serialport.o $(CFLAGS)

ConnectNode.o: ConnectNode.cpp ConnectNode.h ConnectFour.h
	$(CC) ConnectNode.cpp -o ConnectNode.o $(CFLAGS)

ConnectFour.o: ConnectFour.cpp ConnectFour.h
	$(CC) ConnectFour.cpp -o ConnectFour.o $(CFLAGS)

connectFourComputer.o: connectFourComputer.cpp mcts.h serialport.h ConnectNode.h ConnectFour.h
	$(CC) connectFourComputer.cpp -o connectFourComputer.o $(CFLAGS)

clean:
	@rm $(OBJS) $(PROGS)