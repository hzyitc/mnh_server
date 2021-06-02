SRCS=$(wildcard *.c)
DEPS=$(wildcard *.h)
TARGET=mnh_server

FLAGS += -g -fsanitize=address

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	$(RM) $(TARGET)

$(TARGET): $(SRCS) $(DEPS)
	$(CC) $(FLAGS) -o $@ $(SRCS)
