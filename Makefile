SRCS	= $(wildcard *.cpp) $(wildcard *.c)
TARGETS	= $(basename $(SRCS))

#CFLAGS += -g
#CXXFLAGS += -g

all : $(TARGETS)

clean :
	rm -f $(TARGETS)
