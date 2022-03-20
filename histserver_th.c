
#include <stdlib.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>

#include "shareddefs.h"
void *runner (void *param);

int inc;
int inw;
int ins;
int *hist;

int main(int argc, char *argv[])
{
    time_t begin = time(NULL);
    int count;
    mqd_t mq1;
    struct mq_attr mq_attr;
    struct item *itemptr;
    int n;
    char *bufptr;
    int buflen;
    hist = malloc(inc * sizeof(int));

    mq1 = mq_open(MQNAME, O_RDWR | O_CREAT, 0666, NULL);
    if (mq1 == -1) {
        perror("can not create msg queue\n");
        exit(1);
    }
    //printf("mq created, mq id = %d\n", (int) mq1);

    mq_getattr(mq1, &mq_attr);
    //printf("mq maximum msgsize = %d\n", (int) mq_attr.mq_msgsize);

    /* allocate large enough space for the buffer to store
        an incoming message */
    buflen = mq_attr.mq_msgsize;
    bufptr = (char *) malloc(buflen);
    n = mq_receive(mq1, (char *) bufptr, buflen, NULL);
    if (n == -1) {
        perror("mq_receive failed\n");
        exit(1);
    }


    itemptr = (struct item *) bufptr;
    free(bufptr);
    mq_close(mq1);
    mq_unlink(MQNAME);


    count = atoi(argv[1]);
    inc = itemptr->intervalCount;
    inw = itemptr->intervalWidth;
    ins = itemptr->intervalStart;




    pthread_t tid[count]; /* id of the created thread */
    pthread_attr_t attr;  /* set of thread attributes */

    if (argc < 3) {
        fprintf (stderr, "usage: a.out <value>\n");
        return -1;
    }
    /*if (atoi(argv[1]) < 0) {
        fprintf (stderr, "%d must be >= 0\n", atoi(argv[1]));
        return -1;
    }*/

    for (int i = 0; i < count; ++i) {
        pthread_attr_init(&attr);
        pthread_create(&tid[i], &attr, runner, (void *)hist);
    }

    for (int i = 0; i < count; ++i) {
        pthread_join(tid[i], NULL);
    }
    for (int i = 0; i < inc; ++i) {
        printf("%d\n",hist[i]);
    }
    /*int finaldata[1000];

    for(int i = 0;i < inc; ++i){
        finaldata[i] = 0;
    }*/
    mqd_t mq4;
    struct histogramitem item4;
    int z;

    mq4 = mq_open(MQNAME2, O_RDWR, 0666, NULL);
    if (mq4 == -1) {
        perror("can not open msg queue\n");
        exit(1);
    }
    //printf("mq opened, mq id = %d\n", (int) mq4);


    for (int p = 0; p < inc; ++p) {
        item4.intervals[p] = hist[p];
    }


    z = mq_send(mq4, (char *) &item4, sizeof(struct histogramitem), 0);

    sleep(1);

    if (z == -1) {
        perror("mq_send failed\n");
        exit(1);
    }

    mq_close(mq4);
    mq_unlink(MQNAME2);

    time_t end = time(NULL);
    printf("The elapsed time is %d seconds", (end - begin));

    return 0;
}
void *runner (void *param){
    int *histarr = (int *)param;
    int num;
    char *fname;
    FILE *fp = fopen(fname,"r");
    char line[20];
    while(!feof(fp)) {
        fgets(line,20,fp);
        num = atoi(line);
        for (int i = 0; i < inc; ++i) {
            if ((num < (ins + (i + 1) * inw)) && (num >= (ins + i * inw))) {
                histarr[i]++;
            }
        }
    }

    pthread_exit(0);
}