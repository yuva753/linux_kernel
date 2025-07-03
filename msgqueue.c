// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define QUEUE_NAME  "/data_queue"
#define MAX_SIZE    128
#define MSG_COUNT   10

int main() {
    mqd_t mq;
    char buffer[MAX_SIZE];
    struct mq_attr attr;

    // Set message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;        // Max number of messages in queue
    attr.mq_msgsize = MAX_SIZE; // Max message size
    attr.mq_curmsgs = 0;

    // Create message queue
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("Sender: mq_open");
        exit(1);
    }

    for (int i = 1; i <= MSG_COUNT; i++) {
        snprintf(buffer, MAX_SIZE, "Data Block %d", i);

        if (mq_send(mq, buffer, strlen(buffer) + 1, 0) == -1) {
            perror("Sender: mq_send");
            exit(1);
        }

        printf("Sender: Sent -> %s\n", buffer);
        sleep(1);  // simulate delay
    }

    mq_close(mq);
    return 0;
}




// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define QUEUE_NAME  "/data_queue"
#define MAX_SIZE    128
#define MSG_COUNT   10

int main() {
    mqd_t mq;
    char buffer[MAX_SIZE];

    // Open message queue
    mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("Receiver: mq_open");
        exit(1);
    }

    for (int i = 1; i <= MSG_COUNT; i++) {
        ssize_t bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
        if (bytes_read >= 0) {
            buffer[bytes_read] = '\0'; // null-terminate
            printf("Receiver: Received -> %s\n", buffer);
        } else {
            perror("Receiver: mq_receive");
            exit(1);
        }
        sleep(1);  // simulate processing delay
    }

    mq_close(mq);
    mq_unlink(QUEUE_NAME);  // delete the queue after use
    return 0;
}

