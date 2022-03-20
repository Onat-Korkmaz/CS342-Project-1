
#include <stdlib.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "shareddefs.h"

int main(int argc, char *argv[])
{
    int count;
    mqd_t mq1;
    struct mq_attr mq_attr;
    struct item *itemptr;
    int n;
    char *bufptr;
    int buflen;

    mq1 = mq_open(MQNAME1, O_RDWR | O_CREAT, 0666, NULL);
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
    n = mq_receive(mq1, (char *) bufptr, buflen, NULL);
    if (n == -1) {
        perror("mq_receive failed\n");
        exit(1);
    }

    printf("mq_receive success, message size = %d\n", n);

    itemptr = (struct item *) bufptr;
    free(bufptr);
    mq_close(mq1);
    mq_unlink(MQNAME1);


    count = atoi(argv[1]);

    printf("this is the parent process that you started, pid = %d\n",
           (int) getpid());
    printf("%d\n",itemptr->intervalCount);
    printf("%d\n",itemptr->intervalWidth);
    printf("%d\n",itemptr->intervalStart);

    mqd_t mq2;
    struct histogramitem item2;
    int t;
    mq2 = mq_open(MQNAME2, O_RDWR | O_CREAT, 0666, NULL);
    if (mq2 == -1) {
        perror("can not open msg queue\n");
        exit(1);
    }
    printf("mq opened, mq id = %d\n", (int) mq2);

    int inc = itemptr->intervalCount;
    int inw = itemptr->intervalWidth;
    int ins = itemptr->intervalStart;

    pid_t parentID = getpid();
    pid_t children[count];
    char *fname;
    int *hist = malloc(inc * sizeof(int));

    for (int i = 0; i < count; ++i) {
        if(getpid()==parentID && fork()==0) {
            children[i] = getpid();
            //printf("%d\n",children[i]);
        }
    }

    for (int i = 0; i < count; ++i) {
        if(getpid()==children[i]) {
            fname = argv[i+2];
            for (int i = 0; i < inc; ++i) {
                hist[i] = 0;
            }
        }
    }

    //printf("child id:%d\n",(int)getpid());

    /*for (int i = 0; i < inc; ++i) {
        printf("%d\n",hist[i]);
    }*/

    if(getpid() != parentID) {
        int num;
        FILE *fp = fopen(fname,"r");
        char line[20];
        //printf("test4");
        while(!feof(fp)) {
            fgets(line,20,fp);
            num = atoi(line);
            for (int i = 0; i < inc; ++i) {
                //printf("%d\n",ins + (i + 1) * inw);
                if ((num < (ins + (i + 1) * inw)) && (num >= (ins + i * inw))) {
                    hist[i]++;
                }
            }
        }

        item2.intervals = hist;
        item2.id = getpid();



        t = mq_send(mq2, (char *) &item2, sizeof(struct histogramitem), 0);

        sleep(1);

        if (t == -1) {
            perror("mq_send failed\n");
            exit(1);
        }

        printf("mq_send success, child pid = %d\n",
               getpid());
        for (int i = 0; i < inc; ++i) {
            printf("item2->astr(%d) = %d\n",i, item2.intervals[i]);
        }
        //printf("item1->astr = %ls\n", item2.intervals);

        printf("\n");

        mq_close(mq2);
        exit(0);


    }

    sleep(5);
    struct histogramitem *itemptr1;
    char *bufptr1;
    int buflen1;
    mqd_t mq3;

    mq3 = mq_open(MQNAME2, O_RDWR | O_CREAT, 0666, NULL);
    if (mq3 == -1) {
        perror("can not create msg queue\n");
        exit(1);
    }
    printf("mq created, mq id = %d\n", (int) mq3);

    mq_getattr(mq3, &mq_attr);
    printf("mq maximum msgsize = %d\n", (int) mq_attr.mq_msgsize);

    for(int x = 0; x < count; ++x) {
        //printf("%d\n",x);
        /* allocate large enough space for the buffer to store
            an incoming message */
        buflen1 = mq_attr.mq_msgsize;
        bufptr1 = (char *) malloc(buflen);
        n = mq_receive(mq3, (char *) bufptr1, buflen1, NULL);
        if (n == -1) {
            perror("mq_receive failed\n");
            exit(1);
        }

        printf("mq_receive success, message size = %d\n", n);

        itemptr1 = (struct histogramitem *) bufptr1;
        printf("item->pid = %d\n", itemptr1->id);
        for (int i = 0; i < inc; ++i) {
            printf("item2->astr(%d) = %d\n",i, itemptr1->intervals[i]);

        }
        //printf("item->pid%d\n", itemptr1->id);
        //histdata[x] = itemptr1->intervals;
        free(bufptr1);
    }

    mq_close(mq3);

    mq_unlink(MQNAME2);

    /*for(int u = 0;u < count; ++u){
        printf("%d\n",histdata[u]);
    }*/

    sleep(5);

    mqd_t mq4;
    struct histogramitem item4;
    int z;

    mq4 = mq_open(MQNAME4, O_RDWR, 0666, NULL);
    if (mq4 == -1) {
        perror("can not open msg queue\n");
        exit(1);
    }
    printf("mq opened, mq id = %d\n", (int) mq4);



    for(int p = 0;p < count; ++p) {
        item4.intervals = 18031999;
    }


    z = mq_send(mq4, (char *) &item4, sizeof(struct histogramitem), 0);

    sleep(1);

    if (z == -1) {
        perror("mq_send failed\n");
        exit(1);
    }

    printf("mq_send success, item size = %d\n",
           (int) sizeof(struct item));
    if(getpid()==parentID) {
        printf("item1->astr = %d\n", item4.intervals);

    }

    printf("\n");

    mq_close(mq4);

    for (int i = 0; i < inc; ++i) {
        if(getpid()!=parentID)
            printf("%d\n",hist[i]);
    }
    //exit(0);



    return 0;
}