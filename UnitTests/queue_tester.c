#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}


/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Callback function that increments items */
static void iterator_inc(queue_t q, void *data)
{
	int *a = (int*)data;
	printf("a: %d\n", *a);

	if (*a == 42){
		queue_delete(q, data);
		//  printf("q: %p", q->head->data);
    }
		
		else
		*a += 1;
	
}

static void iterator_incTest(queue_t q, void *data)
{
    int *a = (int*)data;
    printf("a: %d\n", *a);

    if (*a == 45){
         queue_delete(q, data);
        //  printf("q: %p", q->head->data);
    }
}

// int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    fprintf(stderr, "*** TEST test_iterator ***\n");

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
	printf("%d\n", queue_length(q));
    queue_iterate(q, iterator_inc);
    printf("%d\n", queue_length(q));
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);
}
void test_iterator_first(void)
{
    queue_t q;
    int data[] = {42, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t i;

    fprintf(stderr, "*** TEST test_iterator ***\n");

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
	printf("%d\n", queue_length(q));
    queue_iterate(q, iterator_inc);
    printf("%d\n", queue_length(q));
    TEST_ASSERT(data[1] == 3);
    printf("%d\n",data[0]);
    TEST_ASSERT(queue_length(q) == 8);

     queue_iterate(q, iterator_incTest);

     for (int i = 0; i < 9; i++)
     {
       printf("data: %d i: %d\n", data[i], i);
     }
     
}
void test_iterator_last(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 42};
    size_t i;

    fprintf(stderr, "*** TEST test_iterator ***\n");

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
	printf("%d\n", queue_length(q));
    queue_iterate(q, iterator_inc);
    printf("%d\n", queue_length(q));
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);

    queue_iterate(q, iterator_incTest);

    //  for (int i = 0; i < 10; i++)
    //  {
    //    printf("data: %d i: %d\n", data[i], i);
    //  }
     
}
void test_destroy(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    fprintf(stderr, "*** TEST test_destroy ***\n");

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    // check the length before destroyin
    TEST_ASSERT(queue_length(q) == 10);
    /* Destroy our queue */
    queue_destroy(q);
    // now length should be 0
    TEST_ASSERT(queue_length(q) == 0);
}
void test_destroy_one_obj(void)
{
     queue_t q;
    int data[] = {1, 2};
    q = queue_create();
    queue_enqueue(q, &data[0]);
    TEST_ASSERT(queue_length(q) == 1);
    queue_delete(q, &data[0]);
    TEST_ASSERT(queue_length(q) == 0);
    queue_enqueue(q, &data[1]);
    TEST_ASSERT(queue_length(q) == 1);
}


int main(void)
{
    // create test
	test_create();
    // enqueue/dequeue
	test_queue_simple();
    // iterate
	test_iterator(); // also uses create, delete, enqueue, length, iterate 
    test_iterator_first();
    test_iterator_last();
    // destroy
    test_destroy_one_obj();
    test_destroy();
	return 0;
}