CC=g++
CF=-Wall
INCLUDES=-L/usr/lib/x86_64-linux-gnu/

SRCS=$(shell find . -name '*.cpp')
OBJS=$(SRCS:.cpp=.o)
EXES=$(OBJS:.o=)

all : $(OBJS) $(EXES)

.PHONY : clean

% : %.o
	$(CC) $(CF) -o $@ $< $(INCLUDES) -lGL -lGLU -lglut

%.o : %.cpp
	$(CC) $(CF) -c $<

clean :
	rm -f $(OBJS) $(EXES)
