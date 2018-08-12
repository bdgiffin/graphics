CC=g++
CF=-Wall

SRCS=$(shell find . -name '*.cpp')
OBJS=$(SRCS:.cpp=.o)
EXES=$(OBJS:.o=)

all : $(OBJS) $(EXES)

.PHONY : clean

% : %.o
	$(CC) $(CF) -o $@ $< -lsfml-graphics -lsfml-window -lsfml-system

%.o : %.cpp
	$(CC) $(CF) -c $<

clean :
	rm -f $(OBJS) $(EXES)
