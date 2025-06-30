#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int num1;
    int num2;
} ThreadData;

void* multiply(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int result = data->num1 * data->num2;
    printf("Multiplication of %d and %d is %d\n", data->num1, data->num2, result);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        printf("Usage: %s num1 num2 num3 num4 num5 num6\n", argv[0]);
        return 1;
    }

    pthread_t threads[3];
    ThreadData threadData[3];

    // Creating three threads
    for (int i = 0; i < 3; i++) {
        threadData[i].num1 = atoi(argv[2 * i + 1]);
        threadData[i].num2 = atoi(argv[2 * i + 2]);

        if (pthread_create(&threads[i], NULL, multiply, &threadData[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Waiting for threads to finish
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

