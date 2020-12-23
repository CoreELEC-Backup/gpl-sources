CXXFLAGS := --std=c++11 -O2 -MMD -MP

SRCS := $(shell find -name '*.cpp')
OBJS := $(SRCS:%.cpp=%.o)
DEPS := $(SRCS:%.cpp=%.d)

TARGET := SSEQPlayer.a

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $@ $^

clean:
	@rm -f $(OBJS)
	@rm -f $(DEPS)

distclean: clean
	@rm -f $(TARGET)

-include $(DEPS)
