#include "player.hpp"
#include <stdio.h>
#include <iostream>
#include <QDebug>
#include "media_process.h"
using namespace MediaProcess;

//************************player**********************************/

Player::Player()
{
    player_state_ = Stop;
    volume_ = 50;
}

Player::~Player()
{
}
/**
 * @brief 
 * set the file path and play
 * @param path 
 * @return true 
 * @return false 
 */
bool Player::setFileName(QString path)
{
    if (player_state_ != Stop)
    {
        return false;
    }

    if(isValidFile(path) == false)
    {
        return false;
    }
    file_name_ = path;

    // Start the thread (run-method)
    this->start();
    player_state_ = Playing;

    return true;
}

bool Player::isValidFile(QString path)
{
    AVFormatContext *av_format_ctx;
    av_format_ctx = avformat_alloc_context();
    if ( avformat_open_input(&av_format_ctx, path.toStdString().c_str(), NULL, NULL) < 0)
    {
        std::cout << "can't open the file."  << std::endl;
        return false;
    }
    if (avformat_find_stream_info(av_format_ctx, NULL) < 0) 
    {
        std::cout << "cannot find stream information"  << std::endl;
        return false;
    }

    int video_stream = -1;
    int audio_stream = -1;

    // Check video formats
    for (int i = 0; i < av_format_ctx->nb_streams; i++)
    {
        if (av_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream = i;
        }
        if (av_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audio_stream = i;
        }
    }

    if(audio_stream < 1 && video_stream < 1 )
    {
        std::cout << "cannot find available streams"  << std::endl;
        return false;
    }

    std::cout << "streams ok: audio stream: " << audio_stream << " video stream: " << video_stream << std::endl;
    avformat_close_input(&av_format_ctx);
    return true;
}

/**
 * @brief 
 * Start to play the file and sent a statechanged signal
 * @return true 
 * @return false 
 */
bool Player::startPlay()
{
    video_state_.isPause = false;

    if (player_state_ == Playing)
    {
        return false;
    }
    else if(player_state_ == Stop)
    {
        if(file_name_.size() == 0 )
        {
            return false;
        }
        setFileName(file_name_);
    }
    player_state_ = Playing;
    emit stateChangedSignal(Playing);

    return true;
}

bool Player::pausePlay()
{
    video_state_.isPause = true;

    if (player_state_ != Playing)
    {
        return false;
    }

    player_state_ = Pause;

    emit stateChangedSignal(Pause);

    return true;
}

bool Player::stopPlay(bool wait_thread_end)
{
    if (player_state_ == Stop)
    {
        return false;
    }

    video_state_.quit = 1;

    if(player_state_ == Pause)
    {
        Player::startPlay();
    }
    
    if (wait_thread_end)
    {
        while(!video_state_.readThreadFinished || !video_state_.videoThreadFinished)
        {
            SDL_Delay(10);
        }
    }

    // Stop SDL audio
    if (video_state_.audioID != 0)
    {
        SDL_LockAudio();
        SDL_PauseAudioDevice(video_state_.audioID,1);
        SDL_UnlockAudio();

        video_state_.audioID = 0;
    }
    player_state_ = Stop;
    emit stateChangedSignal(Stop);
    return true;
}

void Player::seekInMilliseconds(int64_t pos)
{
    if(!video_state_.seek_req)
    {
        //std::cout << " seek request to: " << pos << std::endl;
        video_state_.seek_pos = pos;
        video_state_.seek_req = 1;
    }
}

double Player::getCurrentTime()
{
    //std::cout<<"audio time:1"<<video_state_.audio_clock<<std::endl;
    return video_state_.audio_clock;
}

int64_t Player::getTotalTime()
{
    return video_state_.ic->duration;
}

void Player::disPlayVideo(QImage img)
{
    emit getOneFrameSignal(img);
}

bool Player::setVolume(int volume)
{
    volume_ = volume;
}
/**
 * @brief read video thread and read audio thread
 * 
 */
void Player::run()
{
    char file_path[1280] = {0};
    std::string file__path= file_name_.toStdString();

    std::cout << "file: " << file__path << std::endl;


    memset(&video_state_,0,sizeof(VideoState));

    VideoState *is = &video_state_;

    AVFormatContext *av_format_ctx;

    // v_codec_ctx_old is used in old version 2.6
    AVCodecContext *v_codec_ctx_old;
    AVCodecContext *v_codec_ctx;
    AVCodec *v_codec;
    AVCodecParameters *v_codec_params;

    AVCodecContext *a_codec_ctx_old;
    AVCodecContext *a_codec_ctx;
    AVCodec *a_codec;
    AVCodecParameters *a_codec_params;

    int audio_stream ,video_stream, i;


    //Allocate an AVFormatContext.
    av_format_ctx = avformat_alloc_context();
    int av = avformat_open_input(&av_format_ctx, file__path.c_str(), NULL, NULL);
    if ( av!= 0)
    {
        std::cout << "av number: "<< av << std::endl;
        std::cout << "can't open the file."  << std::endl;
        return;
    }

    if (avformat_find_stream_info(av_format_ctx, NULL) < 0)
    {
        std::cout << "Could't find stream infomation."  << std::endl;
        return;
    }

    video_stream = -1;
    audio_stream = -1;

    // Check video formats
    for (i = 0; i < av_format_ctx->nb_streams; i++)
    {
        //if (av_format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        if (av_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream = i;
            std::cout << "video_stream codec_type: " << video_stream << std::endl;
        }
        //if (av_format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO  && audio_stream < 0)
        if (av_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audio_stream = i;
            std::cout << "audio_stream codec_type: " << video_stream << std::endl;
        }
    }

    // If not available, show error messages

    if (video_stream == -1)
    {
        std::cout << "Didn't find a video stream." << std::endl;
        //return;
    }
    else
    {
        is->videoStream = video_stream;
    }


    if (audio_stream == -1)
    {
        std::cout << "Didn't find a audio stream." << std::endl;
        //return;
    }
    else
    {
        is->audioStream = audio_stream;
    }

    is->ic = av_format_ctx;


    emit totalTimeChangedSignal(getTotalTime());

    if (audio_stream >= 0)
    {
        audio_stream_component_open(&video_state_, audio_stream);
    }

    //std::cout << "------------ PREP CODEC ----------" << std::endl;

    // find audio decoder
    //a_codec_ctx_old = av_format_ctx->streams[audio_stream]->codec;
    a_codec_params = av_format_ctx->streams[audio_stream]->codecpar;
    a_codec = avcodec_find_decoder(a_codec_params->codec_id);
    a_codec_ctx = avcodec_alloc_context3(a_codec);

    if( avcodec_parameters_to_context(a_codec_ctx, a_codec_params) < 0)
    {
        std::cout << "failed to get audio codecContent" << std::endl;
    }

    if (a_codec == NULL)
    {
        std::cout << "a_codec not found." << std::endl;
        return;
    }

    // open audio decoder
    if (avcodec_open2(a_codec_ctx, a_codec, NULL) < 0)
    {
        std::cout << "Could not open audio codec." << std::endl;
        return;
    }

    is->audio_st = av_format_ctx->streams[audio_stream];


    if(video_stream != -1)
    {
        // find video decoder
        v_codec_ctx_old = av_format_ctx->streams[video_stream]->codec;

        v_codec_params = av_format_ctx->streams[video_stream]->codecpar;
        v_codec = avcodec_find_decoder(v_codec_params->codec_id);

        /*
        //v_codec_ctx = avcodec_alloc_context3(v_codec);
        if( avcodec_parameters_to_context(v_codec_ctx, v_codec_params) < 0)
        {
            std::cout << "failed to get audio codecContent" << std::endl;
        }*/

        if (v_codec == NULL)
        {
            std::cout << "v_codec not found." << std::endl;
            return;
        }

        // open video decoder

        if (avcodec_open2(v_codec_ctx_old, v_codec, NULL) < 0)
        {
            std::cout << "Could not open video codec." << std::endl;
            return;
        }

        is->video_st = av_format_ctx->streams[video_stream];
        packet_queue_init(&is->videoq);

        // Set thread for decoding videos
        is->video_tid = SDL_CreateThread(video_thread, "video_thread", &video_state_);
    }

    std::cout << "------------ END OF CODEC PREP ----------" << std::endl;

    if(video_stream == -1 && audio_stream == -1)
    {
        std::cout << "No video stream or audio stream." << std::endl;
        return;
    }

    is->player = this;


    AVPacket *packet = (AVPacket *) malloc(sizeof(AVPacket)); //molloc a memory for video


    // Print file data
    av_dump_format(av_format_ctx, 0, file__path.c_str(), 0);

    while (1)
    {
        is->volume=volume_;
        double x = get_audio_clock(&video_state_);
        emit decodeRunning();


        //is->volume = 50;
        if (is->quit)
        {
            break;
        }

        if (is->seek_req)
        {
            int stream_index = -1;
            int64_t seek_target = is->seek_pos;

            if (is->videoStream >= 0)
            {
                stream_index = is->videoStream;
            }
            else if (is->audioStream >= 0)
            {
                stream_index = is->audioStream;
            }

            AVRational aVRational = {1, AV_TIME_BASE};
            if (stream_index >= 0)
            {
                seek_target = av_rescale_q
                (
                    seek_target,
                    aVRational,
                    av_format_ctx->streams[stream_index]->time_base
                );
            }

            if (av_seek_frame(is->ic, stream_index, seek_target, AVSEEK_FLAG_BACKWARD) < 0)
            {
                std::cerr << is->ic->url << ":error while seeking" << std::endl;
            }
            else
            {
                if (is->audioStream >= 0)
                {
                    AVPacket *packet = (AVPacket *) malloc(sizeof(AVPacket));
                    av_new_packet(packet, 10);
                    strcpy((char*)packet->data,FLUSH_DATA);
                    packet_queue_flush(&is->audioq);
                    packet_queue_put(&is->audioq, packet);
                }
                if (is->videoStream >= 0)
                {
                    AVPacket *packet = (AVPacket *) malloc(sizeof(AVPacket));
                    av_new_packet(packet, 10);
                    strcpy((char*)packet->data,FLUSH_DATA);
                    packet_queue_flush(&is->videoq);
                    packet_queue_put(&is->videoq, packet);
                    is->video_clock = 0;
                }
            }
            is->seek_req = 0;
            is->seek_time = is->seek_pos / 1000000.0;
            is->seek_flag_audio = 1;
            is->seek_flag_video = 1;
        }


        // If audioq.size exeed max queue size, wait for some time.
        if (is->audioq.size > MAX_AUDIO_SIZE || is->videoq.size > MAX_VIDEO_SIZE)
        {
            SDL_Delay(10);
            continue;
        }

        if (is->isPause == true)
        {
            SDL_Delay(10);
            continue;
        }

        if (av_read_frame(av_format_ctx, packet) < 0)
        {
            //std::cout<<"current time"<<is->audio_clock<<std::endl;
            //std::cout<<"totaol time "<< is->ic->duration/1000000<<std::endl;
            double current = (double)is->audio_clock;
            double total = (double)is->ic->duration/1000000.0;
            if(current+0.0030>total)
            {
                std::cout<<"end read"<<std::endl;
                is->readFinished = true;

            }
            
            if (is->quit)
            {
                // End of decoding, stop the thread
                break;
            }

            SDL_Delay(10);
            continue;
        }

        if (packet->stream_index == video_stream)
        {
            // put packet to video queue
            packet_queue_put(&is->videoq, packet);
        }

        else if( packet->stream_index == audio_stream )
        {
            // put packet to audio queue
            packet_queue_put(&is->audioq, packet);
        }
        else
        {
            // Free the packet that was allocated by av_read_frame
            av_packet_unref(packet);
        }

    }// close while loop


    // Wait for player to stop
    while (!is->quit)
    {
        SDL_Delay(100);
    }

    std::cout << "thread end" << std::endl;
    stopPlay();
    //stopPlay();

    //avcodec_close(a_codec_ctx);
    //avcodec_close(v_codec_ctx);
    avcodec_close(a_codec_ctx);
    avcodec_close(v_codec_ctx_old);
    avformat_close_input(&av_format_ctx);

    is->readThreadFinished = true;
}
