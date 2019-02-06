#ifndef MEDIA_PROCESS_H
#define MEDIA_PROCESS_H
namespace MediaProcess
{
    /**
     * @brief init the media queue for multiple threads
     * 
     * @param queue 
     */
    void packet_queue_init(PacketQueue *queue) 
    {
        memset(queue, 0, sizeof(PacketQueue));
        queue->mutex = SDL_CreateMutex();
        queue->cond = SDL_CreateCond();
        queue->size = 0;
        queue->nb_packets = 0;
        queue->first_pkt = NULL;
        queue->last_pkt = NULL;
    }
    /**
     * @brief put a pkt into a queue
     * 
     * @param queue 
     * @param pkt 
     * @return int 
     */
    int packet_queue_put(PacketQueue *queue, AVPacket *pkt) 
    {

        AVPacketList *pkt1;
        if (av_dup_packet(pkt) < 0) 
        {
            return -1;
        }
        pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
        if (!pkt1)
        {
            return -1;
        }
            
        pkt1->pkt = *pkt;
        pkt1->next = NULL;

        SDL_LockMutex(queue->mutex);

        if (!queue->last_pkt)
        {
            queue->first_pkt = pkt1;
        }       
        else
        {
            queue->last_pkt->next = pkt1;
        }       
        queue->last_pkt = pkt1;
        queue->nb_packets++;
        queue->size += pkt1->pkt.size;
        SDL_CondSignal(queue->cond);
        SDL_UnlockMutex(queue->mutex);
        return 0;

    }
    /**
     * @brief get a pkt out of the queue
     * 
     * @param queue 
     * @param pkt 
     * @param block 
     * @return int 
     */
    static int packet_queue_get(PacketQueue *queue, AVPacket *pkt, int block) 
    {
        AVPacketList *pkt1;
        int ret;

        SDL_LockMutex(queue->mutex);

        while(1) 
        {

            pkt1 = queue->first_pkt;
            if (pkt1) 
            {
                queue->first_pkt = pkt1->next;
                if (!queue->first_pkt)
                {
                    queue->last_pkt = NULL;
                }               
                queue->nb_packets--;
                queue->size -= pkt1->pkt.size;
                *pkt = pkt1->pkt;
                av_free(pkt1);
                ret = 1;
                break;
            } 
            else if (!block) 
            {
                ret = 0;
                break;
            } 
            else 
            {
                SDL_CondWait(queue->cond, queue->mutex);
            }

        }

        SDL_UnlockMutex(queue->mutex);
        return ret;
    }
    /**
     * @brief queue flush
     * 
     * @param queue 
     */
    static void packet_queue_flush(PacketQueue *queue)
    {
        AVPacketList *pkt, *pkt1;

        SDL_LockMutex(queue->mutex);
        for(pkt = queue->first_pkt; pkt != NULL; pkt = pkt1)
        {
            pkt1 = pkt->next;

            if(pkt1->pkt.data != (uint8_t *)"FLUSH")
            {
            }
            av_packet_unref(&pkt->pkt);
            av_freep(&pkt);

        }
        queue->last_pkt = NULL;
        queue->first_pkt = NULL;
        queue->nb_packets = 0;
        queue->size = 0;
        SDL_UnlockMutex(queue->mutex);
    }
    /**
     * @brief 
     * decode the audio
     * @param is 
     * @param pts_ptr 
     * @return int 
     */
    static int audio_decode_frame(VideoState *is, double *pts_ptr)
    {
        int len1, len2, decoded_data_size;
        AVPacket *pkt = &is->audio_pkt;
        int got_frame = 0;
        int64_t dec_channel_layout;
        int wanted_nb_samples, resampled_data_size, n;

        double pts;

        while(1)
        {

            while (is->audio_pkt_size > 0) 
            {

                if (is->isPause == true) //if pause
                {
                    SDL_Delay(10);
                    continue;
                }

                if (!is->audio_frame)
                {
                    if (!(is->audio_frame = av_frame_alloc())) 
                    {
                        return AVERROR(ENOMEM);
                    }
                } 
                else
                {
                    av_frame_unref(is->audio_frame);
                }
                    

                len1 = avcodec_decode_audio4(is->audio_st->codec, is->audio_frame, &got_frame, pkt);
                
                if (len1 < 0) 
                {
                    // error, skip the frame
                    is->audio_pkt_size = 0;
                    break;
                }

                is->audio_pkt_data += len1;
                is->audio_pkt_size -= len1;

                if (!got_frame)
                {
                    continue;
                }
                    
                /* calculate the needed buffer size */
                decoded_data_size = av_samples_get_buffer_size(NULL,
                        is->audio_frame->channels, is->audio_frame->nb_samples,
                        (AVSampleFormat)is->audio_frame->format, 1);

                dec_channel_layout =
                        (is->audio_frame->channel_layout
                                && is->audio_frame->channels
                                        == av_get_channel_layout_nb_channels(
                                                is->audio_frame->channel_layout)) ?
                                is->audio_frame->channel_layout :
                                av_get_default_channel_layout(
                                        is->audio_frame->channels);

                wanted_nb_samples = is->audio_frame->nb_samples;

                if (is->audio_frame->format != is->audio_src_fmt
                        || dec_channel_layout != is->audio_src_channel_layout
                        || is->audio_frame->sample_rate != is->audio_src_freq
                        || (wanted_nb_samples != is->audio_frame->nb_samples
                                && !is->swr_ctx)) 
                {
                    if (is->swr_ctx)
                        swr_free(&is->swr_ctx);
                    is->swr_ctx = swr_alloc_set_opts(NULL,
                            is->audio_tgt_channel_layout, (AVSampleFormat)is->audio_tgt_fmt,
                            is->audio_tgt_freq, dec_channel_layout,
                            (AVSampleFormat)is->audio_frame->format, is->audio_frame->sample_rate,
                            0, NULL);
                    if (!is->swr_ctx || swr_init(is->swr_ctx) < 0) 
                    {
                        //fprintf(stderr,"swr_init() failed\n");
                        break;
                    }
                    is->audio_src_channel_layout = dec_channel_layout;
                    is->audio_src_channels = is->audio_st->codec->channels;
                    is->audio_src_freq = is->audio_st->codec->sample_rate;
                    is->audio_src_fmt = is->audio_st->codec->sample_fmt;
                }

                /* we can adjust the number of sample to make the sound and visual sync */
                if (is->swr_ctx) 
                {
                    const uint8_t **in = (const uint8_t **) is->audio_frame->extended_data;
                    uint8_t *out[] = { is->audio_buf2 };
                    if (wanted_nb_samples != is->audio_frame->nb_samples) 
                    {
                        if (swr_set_compensation(is->swr_ctx,
                                (wanted_nb_samples - is->audio_frame->nb_samples)
                                        * is->audio_tgt_freq
                                        / is->audio_frame->sample_rate,
                                wanted_nb_samples * is->audio_tgt_freq
                                        / is->audio_frame->sample_rate) < 0) 
                        {
                            //fprintf(stderr,"swr_set_compensation() failed\n");
                            break;
                        }
                    }

                    len2 = swr_convert(is->swr_ctx, out,
                            sizeof(is->audio_buf2) / is->audio_tgt_channels
                                    / av_get_bytes_per_sample(is->audio_tgt_fmt),
                            in, is->audio_frame->nb_samples);
                    if (len2 < 0) 
                    {
                        //fprintf(stderr,"swr_convert() failed\n");
                        break;
                    }
                    if (len2
                            == sizeof(is->audio_buf2) / is->audio_tgt_channels
                                    / av_get_bytes_per_sample(is->audio_tgt_fmt)) 
                    {
                        //fprintf(stderr,"warning: audio buffer is probably too small\n");
                        swr_init(is->swr_ctx);
                    }
                    is->audio_buf = is->audio_buf2;
                    resampled_data_size = len2 * is->audio_tgt_channels
                            * av_get_bytes_per_sample(is->audio_tgt_fmt);
                } 
                else 
                {
                    resampled_data_size = decoded_data_size;
                    is->audio_buf = is->audio_frame->data[0];
                }

                pts = is->audio_clock;
                *pts_ptr = pts;
                //std::cout<<"audio time:2"<<is->audio_clock<<std::endl;
                n = 2 * is->audio_st->codec->channels;
                is->audio_clock += (double) resampled_data_size
                        / (double) (n * is->audio_st->codec->sample_rate);


                if (is->seek_flag_audio)
                {
                    //if there is seek, then jump to the target frame
                if (is->audio_clock < is->seek_time)
                {
                    break;
                }
                else
                {
                    is->seek_flag_audio = 0;
                }
                }


                // We have data, return it and come back for more later
                return resampled_data_size;
            }

            if (is->isPause == true) //if pause
            {
                SDL_Delay(10);
                continue;
            }

            if (pkt->data)
            {
                av_packet_unref(pkt);
            }

            memset(pkt, 0, sizeof(*pkt));

            if (is->quit)
            {
                return -1;
            }


            if (packet_queue_get(&is->audioq, pkt, 0) <= 0)
            {
                return -1;
            }

            //if got the string means we just jump and seek so we need to clean the buffer.
            if(strcmp((char*)pkt->data,FLUSH_DATA) == 0)
            {
                avcodec_flush_buffers(is->audio_st->codec);
                av_packet_unref(pkt);
                continue;
            }

            is->audio_pkt_data = pkt->data;
            is->audio_pkt_size = pkt->size;

            /* if update, update the audio clock w/pts */
            if (pkt->pts != AV_NOPTS_VALUE) 
            {
                is->audio_clock = av_q2d(is->audio_st->time_base) * pkt->pts;
            }
        }

        return 0;
    }

    /**
     * @brief 
     * audio callback function
     * @param userdata 
     * @param stream 
     * @param len 
     */
    static void audio_callback(void *userdata, Uint8 *stream, int len) 
    {
        VideoState *is = (VideoState *) userdata;

        int len1, audio_data_size;

        double pts;
        SDL_memset(stream, 0, len);
        /*   len is the sdl buffer */
        while (len > 0) 
        {
            /*  audio_buf_index and  audio_buf_size indicate the data buffer we put the decode data，*/
            if (is->audio_buf_index >= is->audio_buf_size) 
            {

                audio_data_size = audio_decode_frame(is, &pts);

                /* audio_data_size < 0 mean cannot decode the audio then we mute */
                if (audio_data_size < 0) 
                {
                    /* silence */
                    is->audio_buf_size = 1024;
                    /* no sound */
                    if (is->audio_buf == NULL)
                    {
                        return;
                    } 
                    memset(is->audio_buf, 0, is->audio_buf_size);
                } 
                else 
                {
                    is->audio_buf_size = audio_data_size;
                }
                is->audio_buf_index = 0;
            }
            /*  查看stream可用空间，决定一次copy多少数据，剩下的下次继续copy */
            len1 = is->audio_buf_size - is->audio_buf_index;
            if (len1 > len) 
            {
                len1 = len;
            }

            if (is->audio_buf == NULL) return;

            // memcpy(stream, (uint8_t *) is->audio_buf + is->audio_buf_index, len1);
            // SDL_MixAudio(stream, (uint8_t * )is->audio_buf + is->audio_buf_index, len1, 50);

            SDL_MixAudioFormat(stream, (uint8_t * )is->audio_buf + is->audio_buf_index, AUDIO_S16SYS, len1, is->volume);

            len -= len1;
            stream += len1;
            is->audio_buf_index += len1;
        }

    }
    /**
     * @brief Get the audio clock object
     * 
     * @param is 
     * @return double 
     */
    static double get_audio_clock(VideoState *is)
    {
        double pts;
        int hw_buf_size, bytes_per_sec, n;

        pts = is->audio_clock; /* maintained in the audio thread */
        hw_buf_size = is->audio_buf_size - is->audio_buf_index;
        bytes_per_sec = 0;
        n = is->audio_st->codec->channels * 2;
        if(is->audio_st)
        {
            bytes_per_sec = is->audio_st->codec->sample_rate * n;
        }
        if(bytes_per_sec)
        {
            pts -= (double)hw_buf_size / bytes_per_sec;
        }
        return pts;
    }
    /**
     * @brief calculate the pts of the decoded video.
     * 
     * @param is 
     * @param src_frame 
     * @param pts 
     * @return double pts
     */
    static double synchronize_video(VideoState *is, AVFrame *src_frame, double pts) 
    {

        double frame_delay;

        if (pts != 0) 
        {
            /* if we have pts, set video clock to it */
            is->video_clock = pts;
        } 
        else 
        {
            /* if we aren't given a pts, set it to the clock */
            pts = is->video_clock;
        }
        /* update the video clock */
        frame_delay = av_q2d(is->video_st->codec->time_base);
        /* if we are repeating a frame, adjust clock accordingly */
        frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
        is->video_clock += frame_delay;
        return pts;
    }
    /**
     * @brief 
     * play the audio thread
     * 
     * @param is 
     * @param stream_index 
     * @return int 
     */
    int audio_stream_component_open(VideoState *is, int stream_index)
    {
        AVFormatContext *ic = is->ic;
        AVCodecContext *codecCtx;
        AVCodec *codec;
        SDL_AudioSpec wanted_spec, spec;
        int64_t wanted_channel_layout = 0;
        int wanted_nb_channels;
        /*  SDLsuport audio channel 1, 2, 4, 6 */
        /*  we will adjust the channel with the number */
        const int next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };

        if (stream_index < 0 || stream_index >= ic->nb_streams) 
        {
            return -1;
        }

        codecCtx = ic->streams[stream_index]->codec;
        wanted_nb_channels = codecCtx->channels;
        if (!wanted_channel_layout
                || wanted_nb_channels
                        != av_get_channel_layout_nb_channels(
                                wanted_channel_layout)) 
        {
            wanted_channel_layout = av_get_default_channel_layout(
                    wanted_nb_channels);
            wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
        }

        wanted_spec.channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
        wanted_spec.freq = codecCtx->sample_rate;

        if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) 
        {
            //fprintf(stderr,"Invalid sample rate or channel count!\n");
            return -1;
        }
        wanted_spec.format = AUDIO_S16SYS; // SDL marco
        wanted_spec.silence = 0;            // 0mean silence
        wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;  // set the sdl buffer size
        wanted_spec.callback = audio_callback;        // audio callback
        wanted_spec.userdata = is;                    // give audio callback data


        do 
        {
            is->audioID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0,0),0,&wanted_spec, &spec,0);

    //        qDebug()<<"audioID"<<audioID;

    //        if (audioID >= 1) break;

            fprintf(stderr,"SDL_OpenAudio (%d channels): %s\n",wanted_spec.channels, SDL_GetError());
            qDebug()<<QString("SDL_OpenAudio (%1 channels): %2").arg(wanted_spec.channels).arg(SDL_GetError());
            wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
            if (!wanted_spec.channels) 
            {
                fprintf(stderr,"No more channel combinations to tyu, audio open failed\n");
    //            return -1;
                break;
            }
            wanted_channel_layout = av_get_default_channel_layout(
                    wanted_spec.channels);
        }
        while(is->audioID == 0);

        /* check the config SDL_OpenAudio*/
        if (spec.format != AUDIO_S16SYS) 
        {
            fprintf(stderr,"SDL advised audio format %d is not supported!\n",spec.format);
            return -1;
        }

        if (spec.channels != wanted_spec.channels) 
        {
            wanted_channel_layout = av_get_default_channel_layout(spec.channels);
            if (!wanted_channel_layout) 
            {
                fprintf(stderr,"SDL advised channel count %d is not supported!\n",spec.channels);
                return -1;
            }
        }

        is->audio_hw_buf_size = spec.size;

        /* put the config into the structure */
        is->audio_src_fmt = is->audio_tgt_fmt = AV_SAMPLE_FMT_S16;
        is->audio_src_freq = is->audio_tgt_freq = spec.freq;
        is->audio_src_channel_layout = is->audio_tgt_channel_layout = wanted_channel_layout;
        is->audio_src_channels = is->audio_tgt_channels = spec.channels;

        codec = avcodec_find_decoder(codecCtx->codec_id);
        if (!codec || (avcodec_open2(codecCtx, codec, NULL) < 0)) 
        {
            fprintf(stderr,"Unsupported codec!\n");
            return -1;
        }
        ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
        switch (codecCtx->codec_type) 
        {
            case AVMEDIA_TYPE_AUDIO:
                is->audio_st = ic->streams[stream_index];
                is->audio_buf_size = 0;
                is->audio_buf_index = 0;
                memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));
                packet_queue_init(&is->audioq);
                SDL_PauseAudioDevice(is->audioID,0);
                break;
            default:
                break;
        }

        return 0;
    }
    /**
     * @brief 
     * video decode thread and draw to the qt
     * @param arg 
     * @return int 
     */
    int video_thread(void *arg)
    {
        VideoState *is = (VideoState *) arg;
        AVPacket pkt1, *packet = &pkt1;

        int ret, got_picture, numBytes;

        double video_pts = 0; // video pts
        double audio_pts = 0; //audio pts


        ///decode video
        AVFrame *pFrame, *pFrameRGB;
        uint8_t *out_buffer_rgb; //the rgb data after decoding
        struct SwsContext *img_convert_ctx;  //the format transfer after decoding

        AVCodecContext *pCodecCtx = is->video_st->codec; //video decoder

        pFrame = av_frame_alloc();
        pFrameRGB = av_frame_alloc();

        ///change YUV to RGB32
        img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

        numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width,pCodecCtx->height);

        out_buffer_rgb = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
        avpicture_fill((AVPicture *) pFrameRGB, out_buffer_rgb, AV_PIX_FMT_RGB32,
                pCodecCtx->width, pCodecCtx->height);

        while(1)
        {
            if (is->quit)
            {
                break;
            }

            if (is->isPause == true) //if pause
            {
                SDL_Delay(10);
                continue;
            }

            if (packet_queue_get(&is->videoq, packet, 0) <= 0)
            {
                if (is->readFinished)
                {
                    break;
                }
                else
                {
                    SDL_Delay(1); 
                    continue;
                }
            }

        
            if(strcmp((char*)packet->data,FLUSH_DATA) == 0)
            {
                avcodec_flush_buffers(is->video_st->codec);
                av_packet_unref(packet);
                continue;
            }

            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,packet);

            if (ret < 0) {
                qDebug()<<"decode error.\n";
                av_packet_unref(packet);
                continue;
            }

            if (packet->dts == AV_NOPTS_VALUE && pFrame->opaque&& *(uint64_t*) pFrame->opaque != AV_NOPTS_VALUE)
            {
                video_pts = *(uint64_t *) pFrame->opaque;
            }
            else if (packet->dts != AV_NOPTS_VALUE)
            {
                video_pts = packet->dts;
            }
            else
            {
                video_pts = 0;
            }

            video_pts *= av_q2d(is->video_st->time_base);
            video_pts = synchronize_video(is, pFrame, video_pts);

            if (is->seek_flag_video)
            {
                
            if (video_pts < is->seek_time)
            {
                av_packet_unref(packet);
                continue;
            }
            else
            {
                is->seek_flag_video = 0;
            }
            }

            while(1)
            {
                if (is->quit)
                {
                    break;
                }

                audio_pts = is->audio_clock;

                //when seek we change the video clock to 0 
                //so we must update video_pts
                //otherwise if it jump from back to front which will be stuck
                video_pts = is->video_clock;


                if (video_pts <= audio_pts) 
                {
                    break;
                }

                int delayTime = (video_pts - audio_pts) * 1000;

                delayTime = delayTime > 5 ? 5:delayTime;

                SDL_Delay(delayTime);
            }

            if (got_picture) 
            {
                sws_scale(img_convert_ctx,
                        (uint8_t const * const *) pFrame->data,
                        pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                        pFrameRGB->linesize);

                //update the RGB with QImage
                QImage tmpImg((uchar *)out_buffer_rgb,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                QImage image = tmpImg.copy(); //copy the image to the interface
                is->player->disPlayVideo(image); //
            }

            av_packet_unref(packet);

        }

        av_free(pFrame);
        av_free(pFrameRGB);
        av_free(out_buffer_rgb);

        if (!is->quit)
        {
            is->quit = true;
        }

        is->videoThreadFinished = true;

        return 0;
    }
}
#endif