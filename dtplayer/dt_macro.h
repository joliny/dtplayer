#ifndef DT_MACRO_H
#define DT_MACRO_H

 /*COMMON*/
#define TRUE 1
#define FALSE 0
#define  MIN(x,y)       ((x)<(y)?(x):(y))
#define  MAX(x,y)       ((x)>(y)?(x):(y))
     /*PLAYER*/
#define FILE_NAME_MAX_LENGTH 1024
#define DT_PTS_FREQ    90000
#define DT_PTS_FREQ_MS 90
#define DT_SYNC_THRESHOLD  PTS_FREQ_MS*150
#define INT64_0     INT64_C(0x8000000000000000)
     /*CODEC*/
#define AV_DROP_THRESHOLD 20*1000 //20*1000ms
     /*PORT*/ /*AUDIO*/ /*VIDEO*/
#endif
