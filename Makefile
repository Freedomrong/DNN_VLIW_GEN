CXX = g++
CXXFLAGS = -Wall -O2 -I.

SRCS = A109bit_trans.cpp B74bit_trans.cpp C73bit_trans.cpp E11bit_trans.cpp \
       Dotherbit_trans.cpp connect.cpp main.cpp VLIW_1024bit.cpp base_math.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = vliw

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
