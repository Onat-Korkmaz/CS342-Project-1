//
// Created by onatk on 23.02.2022.
//

#ifndef CS342PROJ_SHAREDDEFS_H
#define CS342PROJ_SHAREDDEFS_H



struct item {
    int id;
    int intervalCount;
    int intervalWidth;
    int intervalStart;
};

struct histogramitem {
    int id;
    int *intervals;
};

#define MQNAME "/justaname"
#define MQNAME2 "/justaname2"


#endif //CS342PROJ_SHAREDDEFS_H
