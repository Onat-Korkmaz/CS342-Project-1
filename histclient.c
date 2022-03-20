#include <stdlib.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "shareddefs.h"

int main(int argc, char *argv[])
{

    mqd_t mq;
    struct item item1;
    int n;

    mq = mq_open(MQNAME1, O_RDWR, 0666, NULL);
    if (mq == -1) {
        perror("can not open msg queue\n");
        exit(1);
    }
    printf("mq opened, mq id = %d\n", (int) mq);


    item1.id = 1;
    /*strcpy(item1.intervalCount, argv[1]);
    strcpy(item1.intervalWidth, argv[2]);
    strcpy(item1.intervalStart, argv[3]);*/

    item1.intervalCount = atoi(argv[1]);
    item1.intervalWidth = atoi(argv[2]);
    item1.intervalStart = atoi(argv[3]);

    n = mq_send(mq, (char *) &item1, sizeof(struct item), 0);

    sleep(1);

    if (n == -1) {
        perror("mq_send failed\n");
        exit(1);
    }

    printf("mq_send success, item size = %d\n",
           (int) sizeof(struct item));
    printf("item1->id   = %d\n", item1.id);
    printf("item1->astr = %d\n", item1.intervalCount);
    printf("item1->astr = %d\n", item1.intervalWidth);
    printf("item1->astr = %d\n", item1.intervalStart);
    printf("\n");

    mq_close(mq);

    sleep(5);

    mqd_t mq1;
    struct mq_attr mq_attr;
    struct histogramitem *itemptr;
    int h;
    char *bufptr;
    int buflen;


    mq1 = mq_open(MQNAME4, O_RDWR | O_CREAT, 0666, NULL);
    if (mq1 == -1) {
        perror("can not create msg queue\n");
        exit(1);
    }
    printf("mq created, mq id = %d\n", (int) mq1);

    mq_getattr(mq1, &mq_attr);
    printf("mq maximum msgsize = %d\n", (int) mq_attr.mq_msgsize);

    /* allocate large enough space for the buffer to store
        an incoming message */
    buflen = mq_attr.mq_msgsize;
    bufptr = (char *) malloc(buflen);
    h = mq_receive(mq1, (char *) bufptr, buflen, NULL);
    if (h == -1) {
        perror("mq_receive failed\n");
        exit(1);
    }

    printf("mq_receive success, message size = %d\n", n);

    itemptr = (struct histogramitem *) bufptr;
    printf("item->data %ls\n", itemptr->intervals);


    free(bufptr);
    mq_close(mq1);

    return 0;
}