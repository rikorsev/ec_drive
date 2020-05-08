CC=gcc

UNITY_ROOT=../../../unity
UNITY_INCLUDE=-I$(UNITY_ROOT)/src -I$(UNITY_ROOT)/extras/fixture/src -I$(UNITY_ROOT)/extras/memory/src
UNITY_SRC=$(UNITY_ROOT)/src/unity.c $(UNITY_ROOT)/extras/fixture/src/unity_fixture.c
CFLAGS=-g -DUNITY_INCLUDE_PRINT_FORMATTED

EGL_INCLUDE=-I../result

build:
	$(CC) $(CFLAGS) $(UNITY_INCLUDE) $(EGL_INCLUDE) $(UNITY_SRC) egl_chunk.c egl_chunk_test.c -o test

run: build
	./test 

clean:
	rm ./test