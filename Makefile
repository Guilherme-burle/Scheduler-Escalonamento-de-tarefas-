CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

TARGET = scheduler
SOURCE = main.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET) rate_gbm.out edf_gbm.out