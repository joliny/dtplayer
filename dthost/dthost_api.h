#ifndef DTHOST_API_H
#define DTHOST_API_H

#include "dt_av.h"

#include <stdint.h>

#define AUDIO_EXTR_DATA_SIZE 4096
#define VIDEO_EXTR_DATA_SIZE 4096

typedef struct
{
    /*flag */
    int has_audio;
    int has_video;
    int has_sub;
    int sync_enable;

    /*audio */
    int audio_format;
    int audio_channel;
    int audio_samplerate;
    int audio_bitrate;
    int audio_sample_fmt;
    int audio_codec_id;
    int audio_block_align;
    int audio_num, audio_den;   //for stream pts calc
    int audio_extra_size;
    unsigned char audio_extra_data[AUDIO_EXTR_DATA_SIZE];
    int audio_filter;           //audio filter options
    int audio_output;           //audio output device select
    void *actx_priv;            //point to 

    /*video */
    int video_format;
    int video_dest_width;
    int video_dest_height;
    int video_src_width;
    int video_src_height;
    int video_dest_pixfmt;
    int video_src_pixfmt;
    int video_rate;
    int video_extr;
    int video_ratio;
    double video_fps;
    int video_num, video_den;   //for stream pts calc
    void *video_para;
    unsigned long long ratio64;
    int video_extra_size;
    unsigned char video_extra_data[VIDEO_EXTR_DATA_SIZE];
    int video_filter;
    int video_output;
    void *vctx_priv;

    /*sub */
    int sub_format;
    int sub_id;
    int sub_width;
    int sub_height;
} dthost_para_t;

typedef struct
{
    int abuf_level;
    int vbuf_level;

    int adec_err_cnt;
    int vdec_err_cnt;

    int64_t cur_apts;
    int64_t cur_vpts;
    int64_t cur_systime;

} host_state_t;

int dthost_start (void *host_priv);
int dthost_pause (void *host_priv);
int dthost_resume (void *host_priv);
int dthost_stop (void *host_priv);
int dthost_init (void **host_priv, dthost_para_t * para);
int dthost_read_frame (void *host_priv, dt_av_frame_t * frame, int type);
int dthost_write_frame (void *host_priv, dt_av_frame_t * frame, int type);
int64_t dthost_get_apts (void *host_priv);
int64_t dthost_update_apts (void *host_priv, int64_t pts);
int64_t dthost_get_vpts (void *host_priv);
void dthost_update_vpts (void *host_priv, int64_t vpts);
int dthost_get_avdiff (void *host_priv);
int64_t dthost_get_current_time (void *host_priv);
int64_t dthost_get_systime (void *host_priv);
void dthost_update_systime (void *host_priv, int64_t systime);
int dthost_get_state (void *host_priv, host_state_t * state);
int dthost_get_out_closed (void *host_priv);

#endif
