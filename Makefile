CC = clang
CFLAGS = -pedantic -Wall -Wextra -std=c90 -m64 -O2 -I$(GLAD_DIR)/include
LDFLAGS = -F/Library/Frameworks -framework SDL2 -framework OpenGL -W1,--strip-all

TARGET = gersemi

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

GLAD_DIR = glad

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:$(OBJ_DIR)/%.o=$(OBJ_DIR)/%.d)

GLAD_SRCS = $(GLAD_DIR)/src/glad.c
GLAD_OBJS = $(OBJ_DIR)/glad.o
GLAD_DEPS = $(OBJ_DIR)/glad.d

$(BIN_DIR)/$(TARGET): $(OBJS) $(GLAD_OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(GLAD_OBJS) -o $@ $(LDFLAGS)

$(OBJS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(GLAD_OBJS): $(OBJ_DIR)/%.o : $(GLAD_DIR)/src/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

$(DEPS): $(OBJ_DIR)/%.d : $(SRC_DIR)/%.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

-include $(GLAD_DEPS)

$(GLAD_DEPS): $(OBJ_DIR)/%.d : $(GLAD_DIR)/src/%.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f $(GLAD_OBJS)
	rm -f $(GLAD_DEPS)
	rm -f $(BIN_DIR)/$(TARGET)
