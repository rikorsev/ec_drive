#include <stdlib.h>
#include <string.h>

#include "egl_chunk.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(init);

TEST_SETUP(init)
{
    /* Do nothing */
}

TEST_TEAR_DOWN(init)
{
    /* Do nothing */
}

TEST(init, normal)
{
    egl_result_t result = EGL_FAIL;
    egl_chunk_t *chunk = NULL;
    char buffer[64] = {0};

    EGL_DECLARE_CHUNKS(chunks, 8);

    result = egl_chunk_init(&chunks, buffer, sizeof(buffer));
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS,  result, "Wrong result");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8, chunks.size, "Wrong chunks size");

    for(int i = 0; i < 8; i++)
    {
        chunk = egl_chunk_by_index_get(&chunks, i);
        TEST_ASSERT_EQUAL_PTR_MESSAGE(chunk->buf, &buffer[i * 8], "Wring pointer");
        TEST_ASSERT_EQUAL_size_t_MESSAGE(0, chunk->size, "Wrong chunk size");
    }
}

TEST(init, buffer_size_is_zero)
{
    egl_result_t result = EGL_FAIL;
    char buffer[64] = {0};

    EGL_DECLARE_CHUNKS(chunks, 8);

    result = egl_chunk_init(&chunks, buffer, 0);
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_INVALID_PARAM,  result, "Wrong result");
}

TEST(init, buffer_size_less_then_chunks_number)
{
    egl_result_t result = EGL_FAIL;
    char buffer[7] = {0};

    EGL_DECLARE_CHUNKS(chunks, 8);

    result = egl_chunk_init(&chunks, buffer, sizeof(buffer));
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_INVALID_PARAM,  result, "Wrong result");
}

TEST(init, buffer_size_and_chunks_number_is_not_aligned)
{
    egl_result_t result = EGL_FAIL;
    char buffer[63] = {0};

    EGL_DECLARE_CHUNKS(chunks, 8);

    result = egl_chunk_init(&chunks, buffer, sizeof(buffer));
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_INVALID_PARAM,  result, "Wrong result");
}

TEST(init, after_read_write)
{
    egl_result_t result = EGL_FAIL;
    char buffer[64] = {0};
    char data = 1;
    unsigned int in_index  = -1;
    unsigned int out_index = -1;
    size_t size = sizeof(data);

    EGL_DECLARE_CHUNKS(chunks, 8);

    result = egl_chunk_init(&chunks, buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS,  result, "Wrong result");

    result = egl_chunk_write(&chunks, &data, sizeof(data));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS,  result, "Wrong result");

    result = egl_chunk_read(&chunks, &data, &size);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS,  result, "Wrong result");

    in_index = egl_chunk_in_index_get(&chunks);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, in_index, "Wrong index");

    out_index = egl_chunk_out_index_get(&chunks);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, out_index, "Wrong index");

    result = egl_chunk_init(&chunks, buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS,  result, "Wrong result");
    
    in_index = egl_chunk_in_index_get(&chunks);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, in_index, "Wrong index");

    out_index = egl_chunk_out_index_get(&chunks);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, out_index, "Wrong index");
}

TEST_GROUP_RUNNER(init)
{
    RUN_TEST_CASE(init, normal);
    RUN_TEST_CASE(init, buffer_size_is_zero);
    RUN_TEST_CASE(init, buffer_size_less_then_chunks_number);   
    RUN_TEST_CASE(init, buffer_size_and_chunks_number_is_not_aligned);
    RUN_TEST_CASE(init, after_read_write);
}

TEST_GROUP(read_write);

TEST_SETUP(read_write)
{

}

TEST_TEAR_DOWN(read_write)
{
    /* Do nothing */
}

void test_read_write(egl_chunks_t *chunks, char *data_in, size_t size_in, int index)
{
    egl_result_t result    = EGL_FAIL;
    char data_out[8]       = {0};
    size_t size_out        = sizeof(data_out);
    unsigned int in_index  = -1;
    unsigned int out_index = -1;

    TEST_PRINTF("Read/Write %d", index);

    /* Read/write 1 */
    memset(data_out, 0, sizeof(data_out));

    result = egl_chunk_write(chunks, data_in, size_in);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");

    in_index = egl_chunk_in_index_get(chunks);
    TEST_ASSERT_EQUAL_INT_MESSAGE(index % chunks->number, in_index, "WRITE: Wrong in index");

    result = egl_chunk_read(chunks, data_out, &size_out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "READ: Wrong out index");

    out_index = egl_chunk_out_index_get(chunks);
    TEST_ASSERT_EQUAL_INT_MESSAGE(index % chunks->number, out_index, "WRITE: Wrong status");

    TEST_ASSERT_EQUAL_INT_MESSAGE(size_in, size_out, "RW: Input and output data LENGTH missmatch");

    for(int i = 0; i < size_out; i++)
    {
        TEST_PRINTF("In[%d]: %d, Out[%d]: %d", i, data_in[i], i, data_out[i]);
    }

    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(data_in, data_out, size_out, "RW1: Input and output DATA missmatch");
}

TEST(read_write, normal1)
{
    EGL_DECLARE_CHUNKS(chunks, 8);
    unsigned int in_index  = -1;
    unsigned int out_index = -1;
    char buffer[64]        = {0};

    char data_in1[] = {1};
    char data_in2[] = {1, 2};
    char data_in3[] = {1, 2, 3};
    char data_in4[] = {1, 2, 3, 4};
    char data_in5[] = {1, 2, 3, 4, 5};
    char data_in6[] = {1, 2, 3, 4, 5, 6};
    char data_in7[] = {1, 2, 3, 4, 5, 6, 7};
    char data_in8[] = {1, 2, 3, 4, 5, 6, 7, 8};

    egl_chunk_init(&chunks, buffer, sizeof(buffer));

    /* Before read/write */
    in_index = egl_chunk_in_index_get(&chunks);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, in_index, "WRITE1: Wrong in index");

    out_index = egl_chunk_out_index_get(&chunks);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, out_index, "WRITE1: Wrong status");

    test_read_write(&chunks, data_in1, sizeof(data_in1), 1);
    test_read_write(&chunks, data_in2, sizeof(data_in2), 2);
    test_read_write(&chunks, data_in3, sizeof(data_in3), 3);
    test_read_write(&chunks, data_in4, sizeof(data_in4), 4);
    test_read_write(&chunks, data_in5, sizeof(data_in5), 5);
    test_read_write(&chunks, data_in6, sizeof(data_in6), 6);
    test_read_write(&chunks, data_in7, sizeof(data_in7), 7);
    test_read_write(&chunks, data_in8, sizeof(data_in8), 8);
    test_read_write(&chunks, data_in1, sizeof(data_in1), 9);
    test_read_write(&chunks, data_in2, sizeof(data_in2), 10);
    test_read_write(&chunks, data_in3, sizeof(data_in3), 11);
    test_read_write(&chunks, data_in4, sizeof(data_in4), 12);
    test_read_write(&chunks, data_in5, sizeof(data_in5), 13);
    test_read_write(&chunks, data_in6, sizeof(data_in6), 14);
    test_read_write(&chunks, data_in7, sizeof(data_in7), 15);
    test_read_write(&chunks, data_in8, sizeof(data_in8), 16);
}

TEST(read_write, normal2)
{
    EGL_DECLARE_CHUNKS(chunks, 4);
    egl_result_t result    = EGL_FAIL;
    unsigned int in_index  = -1;
    unsigned int out_index = -1;
    char buffer[32]        = {0};
    char data_out[8]       = {0};
    size_t size_out        = sizeof(data_out);

    char data_in1[] = {1};
    char data_in2[] = {1, 2};
    char data_in3[] = {1, 2, 3};
    char data_in4[] = {1, 2, 3, 4};
    char data_in5[] = {1, 2, 3, 4, 5};
    char data_in6[] = {1, 2, 3, 4, 5, 6};
    char data_in7[] = {1, 2, 3, 4, 5, 6, 7};
    char data_in8[] = {1, 2, 3, 4, 5, 6, 7, 8};

    egl_chunk_init(&chunks, buffer, sizeof(buffer));

    /* Before read/write */
    in_index = egl_chunk_in_index_get(&chunks);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, in_index, "WRITE1: Wrong in index");

    out_index = egl_chunk_out_index_get(&chunks);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, out_index, "WRITE1: Wrong status");

    /* Write data */
    result = egl_chunk_write(&chunks, data_in1, sizeof(data_in1));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");
    
    result = egl_chunk_write(&chunks, data_in2, sizeof(data_in2));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");
    
    result = egl_chunk_write(&chunks, data_in3, sizeof(data_in3));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");
    
    result = egl_chunk_write(&chunks, data_in4, sizeof(data_in4));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");
    
    result = egl_chunk_write(&chunks, data_in5, sizeof(data_in5));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");
    
    result = egl_chunk_write(&chunks, data_in6, sizeof(data_in6));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");
    
    result = egl_chunk_write(&chunks, data_in7, sizeof(data_in7));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");
    
    result = egl_chunk_write(&chunks, data_in8, sizeof(data_in8));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");

    /* Read data */
    size_out = sizeof(data_out);
    result = egl_chunk_read(&chunks, data_out, &size_out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "READ: Wrong out index");
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(data_in5, data_out, size_out, "RW1: Input and output DATA missmatch");

    size_out = sizeof(data_out);
    result = egl_chunk_read(&chunks, data_out, &size_out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "READ: Wrong out index");
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(data_in6, data_out, size_out, "RW1: Input and output DATA missmatch");

    size_out = sizeof(data_out);
    result = egl_chunk_read(&chunks, data_out, &size_out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "READ: Wrong out index");
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(data_in7, data_out, size_out, "RW1: Input and output DATA missmatch");

    size_out = sizeof(data_out);
    result = egl_chunk_read(&chunks, data_out, &size_out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "READ: Wrong out index");
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(data_in8, data_out, size_out, "RW1: Input and output DATA missmatch");

}

TEST(read_write, write_serial)
{   
    EGL_DECLARE_CHUNKS(chunks, 4);
    char buffer[32]        = {0};
    char data_out[8]       = {0};
    egl_result_t result    = EGL_FAIL;
    size_t size_out        = sizeof(data_out);

    char data_in1[] = {1};
    char data_in2[] = {1, 2};
    char data_in3[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

    egl_chunk_init(&chunks, buffer, sizeof(buffer));

    /* Let's perform regular read write first */
    
    /* Write data */
    result = egl_chunk_write(&chunks, data_in1, sizeof(data_in1));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");
    
    /* Read data */
    size_out = sizeof(data_out);
    result = egl_chunk_read(&chunks, data_out, &size_out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "READ: Wrong out index");
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(data_in1, data_out, size_out, "RW1: Input and output DATA missmatch");

    /* Write data */
    result = egl_chunk_write(&chunks, data_in2, sizeof(data_in1));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");

    /* Read data */
    size_out = sizeof(data_out);
    result = egl_chunk_read(&chunks, data_out, &size_out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "READ: Wrong out index");
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(data_in2, data_out, size_out, "RW1: Input and output DATA missmatch");

    /* Now lets perfom serial write */
    /* Write several chunks with one call data */
    result = egl_chunk_serial_write(&chunks, data_in3, sizeof(data_in3));
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "WRITE: Wrong status");

    /* Check result. It should be written 3 chunks (2 full chunks and one non full)*/

    /* Read data 1*/
    size_out = sizeof(data_out);
    result = egl_chunk_read(&chunks, data_out, &size_out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "READ: Wrong out index");
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(data_in3, data_out, size_out, "RW1: Input and output DATA missmatch");
    
    /* Read data 2*/
    size_out = sizeof(data_out);
    result = egl_chunk_read(&chunks, data_out, &size_out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "READ: Wrong out index");
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(data_in3 + 8, data_out, size_out, "RW1: Input and output DATA missmatch");

    /* Read data 3*/
    size_out = sizeof(data_out);
    result = egl_chunk_read(&chunks, data_out, &size_out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(EGL_SUCCESS, result, "READ: Wrong out index");
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(data_in3 + 16, data_out, size_out, "RW1: Input and output DATA missmatch");
}

TEST_GROUP_RUNNER(read_write)
{
    RUN_TEST_CASE(read_write, normal1);
    RUN_TEST_CASE(read_write, normal2);
    RUN_TEST_CASE(read_write, write_serial);
}

TEST_GROUP(get);

TEST_SETUP(get)
{
    /* Do nothing */
}

TEST_TEAR_DOWN(get)
{
    /* Do nothing */
}

TEST(get, last_written_read_with_zero_in_out_indexes)
{
    EGL_DECLARE_CHUNKS(chunks, 4);
    egl_chunk_t *chunk = NULL;
    char buffer[32] = {0};

    TEST_ASSERT(egl_chunk_init(&chunks, buffer, sizeof(buffer)) == EGL_SUCCESS);

    chunk = egl_chunk_in_previous_get(&chunks);
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&chunks.chunk[3], chunk, "Wring pointer");

    chunk = egl_chunk_out_previous_get(&chunks);
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&chunks.chunk[3], chunk, "Wring pointer");

}

TEST_GROUP_RUNNER(get)
{
    RUN_TEST_CASE(get, last_written_read_with_zero_in_out_indexes);
}

// TEST_GROUP(write);

// TEST_SETUP(write)
// {
//     /* Do nothing */
// }

// TEST_TEAR_DOWN(write)
// {
//     /* Do nothing */
// }

// TEST(write, wrong_index)
// {
//     EGL_DECLARE_CHUNKS(chunks, 4);
//     char buffer[64] = {0};
    
//     egl_chunk_init(&chunks, buffer, sizeof(buffer));
// }

static void RunAllTests(void)
{
    RUN_TEST_GROUP(init);
    RUN_TEST_GROUP(read_write);
    RUN_TEST_GROUP(get);
}


int main(int argc, const char * argv[])
{
    return UnityMain(argc, argv, RunAllTests);
}