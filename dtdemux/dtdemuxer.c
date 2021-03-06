#include "dtdemuxer.h"

#define TAG "DEMUXER"

#define REGISTER_DEMUXER(X,x) \
    if(ENABLE_DEMUXER_##X)    \
    {                         \
        extern demuxer_wrapper demuxer_##x; \
        register_demuxer(&demuxer_##x);     \
    }
static demuxer_wrapper *g_demuxer = NULL;

static void register_demuxer (demuxer_wrapper * wrapper)
{
    demuxer_wrapper **p;
    p = &g_demuxer;
    while (*p != NULL)
        p = &((*p)->next);
    *p = wrapper;
    wrapper->next = NULL;
}

static void demuxer_register_all ()
{
    REGISTER_DEMUXER (FFMPEG, ffmpeg);
}

static void demuxer_unregister_all ()
{
    g_demuxer = NULL;
    return;
}

static int demuxer_select (dtdemuxer_context_t * dem_ctx)
{
    if (!g_demuxer)
        return -1;
    dem_ctx->demuxer = g_demuxer; // select the only one
    return 0;
}

static void dump_media_info (dt_media_info_t * info)
{
    dt_info (TAG, "|====================MEDIA INFO====================|\n", info->file_name);
    dt_info (TAG, "|file_name:%s\n", info->file_name);
    dt_info (TAG, "|file_size:%d \n", info->file_size);
    dt_info (TAG, "|duration:%d bitrate:%d\n", info->duration, info->bit_rate);
    dt_info (TAG, "|has video:%d has audio:%d has sub:%d\n", info->has_video, info->has_audio, info->has_sub);
    dt_info (TAG, "|video stream info,num:%d\n", info->vst_num);
    int i = 0;
    for (i = 0; i < info->vst_num; i++)
    {
        dt_info (TAG, "|video stream:%d index:%d id:%d\n", i, info->vstreams[i]->index, info->vstreams[i]->id);
        dt_info (TAG, "|bitrate:%d width:%d height:%d duration:%lld \n", info->vstreams[i]->bit_rate, info->vstreams[i]->width, info->vstreams[i]->height, info->vstreams[i]->duration);
    }
    dt_info (TAG, "|audio stream info,num:%d\n", info->ast_num);
    for (i = 0; i < info->ast_num; i++)
    {
        dt_info (TAG, "|audio stream:%d index:%d id:%d\n", i, info->astreams[i]->index, info->astreams[i]->id);
        dt_info (TAG, "|bitrate:%d sample_rate:%d channels:%d bps:%d duration:%lld \n", info->astreams[i]->bit_rate, info->astreams[i]->sample_rate, info->astreams[i]->channels, info->astreams[i]->bps, info->astreams[i]->duration);
    }

    dt_info (TAG, "|subtitle stream num:%d\n", info->sst_num);
    dt_info (TAG, "|================================================|\n", info->file_name);
}

int demuxer_open (dtdemuxer_context_t * dem_ctx)
{
    int ret = 0;
    /*register demuxer */
    demuxer_register_all ();
    if (demuxer_select (dem_ctx) == -1)
    {
        dt_error (TAG, "select demuxer failed \n");
        return -1;
    }
    demuxer_wrapper *wrapper = dem_ctx->demuxer;
    dt_info (TAG, "select demuxer:%s\n", wrapper->name);
    ret = wrapper->open (wrapper, dem_ctx->file_name, dem_ctx);
    if (ret < 0)
    {
        dt_error (TAG, "demuxer open failed\n");
        return -1;
    }
    dt_info (TAG, "demuxer open ok\n");
    dt_media_info_t *info = &(dem_ctx->media_info);
    wrapper->setup_info (wrapper, info);
    dump_media_info (info);
    dt_info (TAG, "demuxer setup info ok\n");
    return 0;
}

int demuxer_read_frame (dtdemuxer_context_t * dem_ctx, dt_av_frame_t * frame)
{
    demuxer_wrapper *wrapper = dem_ctx->demuxer;
    return wrapper->read_frame (wrapper, frame);
}

int demuxer_seekto (dtdemuxer_context_t * dem_ctx, int timestamp)
{
    demuxer_wrapper *wrapper = dem_ctx->demuxer;
    return wrapper->seek_frame (wrapper, timestamp);
}

int demuxer_close (dtdemuxer_context_t * dem_ctx)
{
    int i = 0;
    demuxer_wrapper *wrapper = dem_ctx->demuxer;
    wrapper->close (wrapper);
    demuxer_unregister_all ();
    /*free media info */
    dt_media_info_t *info = &(dem_ctx->media_info);
    if (info->has_audio)
        for (i = 0; i < info->ast_num; i++)
        {
            if (info->astreams[i] == NULL)
                continue;
            if (info->astreams[i]->extradata_size)
                free (info->astreams[i]->extradata);
            free (info->astreams[i]);
            info->astreams[i] = NULL;
        }
    if (info->has_video)
        for (i = 0; i < info->vst_num; i++)
        {
            if (info->vstreams[i] == NULL)
                continue;
            if (info->vstreams[i]->extradata_size)
                free (info->vstreams[i]->extradata);
            free (info->vstreams[i]);
            info->vstreams[i] = NULL;
        }
    if (info->has_sub)
        for (i = 0; i < info->sst_num; i++)
        {
            if (info->sstreams[i] == NULL)
                continue;
            if (info->sstreams[i]->extradata_size)
                free (info->sstreams[i]->extradata);
            free (info->sstreams[i]);
            info->sstreams[i] = NULL;
        }

    return 0;
}
