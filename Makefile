CC = gcc
CFLAG = -Wall
TARGET = 20191574.out
OBJECT = $(SRC:.c=.o)
SRC = srcs/20191574.c srcs/assembler.c srcs/linkload.c srcs/execute.c

all : $(TARGET)

$(TARGET) : $(OBJECT)
	$(CC) $(CFLAG) -o $(TARGET) $(OBJECT)

.c.o :
	$(CC) $(CLFAG) -I includes -c $< -o $@

clean :
	rm -f $(OBJECT)

fclean : clean
	rm -f $(TARGET)

re : fclean all
