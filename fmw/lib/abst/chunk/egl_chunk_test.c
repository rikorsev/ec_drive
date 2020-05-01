
#include "egl_chunk.h"
#include "unity.h"

#define NUMBER_OF_CHUNKS 8
#define CHUNK_SIZE       8

char chunk_buffer[NUMBER_OF_CHUNKS * CHUNK_SIZE];
EGL_DECLARE_CHUNK(chunks, NUMBER_OF_CHUNKS, CHUNK_SIZE);

TEST_GROUP(init);

TEST_SETUP(init)
{
    /* Do nothing */
}

TEST_TEAR_DOWN(init)
{
    /* Do nothing */
}

TEST(init, wrong_buffer_size)
{
    egl_result_t result = EGL_SUCCESS;
    char buffer[63] = {0};

    EGL_DECLARE_CHUNK(chunks, 8, 8);

    result = egl_chunk_init(chunks, buffer, sizeof(buffer));
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_INVALID_PARAM,  result, "Wrong result");
}

TEST_GROUP(read_write_get);

TEST_SETUP(read_write_get)
{
    egl_chunk_init(chunks, chunk_buffer, sizeof(chunk_buffer));
    memset(chunk_buffer, 0, sizeof(chunk_buffer));
}

TEST_TEAR_DOWN(read_write_get)
{
    /* Do nothing */
}

TEST(read_write, normal)
{
    egl_result_t result = EGL_FAIL;
    size_t size = 0;
    unsigned int in_index = -1;
    unsigned int out_index = -1;

    char data_in[][] = 
    {
        {1},
        {1, 2},
        {1, 2, 3},
        {1, 2, 3, 4},
        {1, 2, 3, 4, 5}, 
        {1, 2, 3, 4, 5, 6},
        {1, 2, 3, 4, 5, 6, 7},
        {1, 2, 3, 4, 5, 6, 7, 8}
    };

    char data_out[8] = {0};

    for(int i = 0; i < 16; i++)
    {
        result = egl_chunk_write(chunks, data[i % 8], sizeof(data[i % 8]));
        TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");

        in_index = egl_chunk_in_index_get(chunks);
        TEST_ASSERT_EQUAL_INT_MESSAGE(i % 8, in_index, "WRITE: Wrong status");

        size = sizeof(data_out);
        result = egl_chunk_read(chunks, data_out, &size);
        TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "READ: Wrong status");

        out_index = egl_chunk_out_index_get(chunks);
        TEST_ASSERT_EQUAL_INT_MESSAGE(i % 8, out_index, "WRITE: Wrong status");

        TEST_ASSERT_EQUAL_INT_MESSAGE(sizeof(data[i]), size, "Input and output data LENGTH missmatch");

        TEST_ASSERT_MESSAGE(memcmp(data_in[i % 8], data_out, size), "Input and output DATA missmatch");
    }
}

int main(void)
{
    UnityBegin("EGL:CHUNK");

    RUN_TEST_GROUP(init);
    RUN_TEST_GROUP(read_write);

    return UnityEnd();
}