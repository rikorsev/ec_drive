CC=gcc

UNITY_ROOT=../../../unity
UNITY_INCLUDE=-I$(UNITY_ROOT)/src
UNITY_SRC=$(UNITY_ROOT)/src/unity.c
CFLAGS=-g

build:
	$(CC) $(CFLAGS) $(UNITY_INCLUDE) $(UNITY_SRC) egl_ringbuf.c egl_ringbuf_test.c -o test

run: build
	./test 

clean:
	rm ./test