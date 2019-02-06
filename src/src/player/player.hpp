
#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "common.h"
class Player;

typedef struct VideoState
{
    AVFormatContext *ic;
    int videoStream, audioStream;
    AVFrame *audio_frame;// audio decode buffer memory
    PacketQueue audioq;
    AVStream *audio_st; // audio stream
    unsigned int audio_buf_size;
    unsigned int audio_buf_index;
    AVPacket audio_pkt;
    uint8_t *audio_pkt_data;
    int audio_pkt_size;
    uint8_t *audio_buf;
    int volume;
    DECLARE_ALIGNED(16,uint8_t,audio_buf2) [AVCODEC_MAX_AUDIO_FRAME_SIZE * 4];
    enum AVSampleFormat audio_src_fmt;
    enum AVSampleFormat audio_tgt_fmt;
    int audio_src_channels;
    int audio_tgt_channels;
    int64_t audio_src_channel_layout;
    int64_t audio_tgt_channel_layout;
    int audio_src_freq;
    int audio_tgt_freq;
    struct SwrContext *swr_ctx; //audio format transfer
    int audio_hw_buf_size;
    double audio_clock; /// audio clock
    double video_clock; ///<pts of last decoded frame / predicted pts of next decoded frame
    AVStream *video_st;
    PacketQueue videoq;
    /// ffmpeg seek
    int             seek_req; //seek request
    int64_t         seek_pos; //seek position m-seconds
    int             seek_flag_audio;// audio seek flag
    int             seek_flag_video;// vidio seek flag
    double          seek_time; // the actual seek time
    ///play control
    bool isPause;  // pause or not
    bool quit;  // quit
    bool readFinished; // read file finish
    bool readThreadFinished;
    bool videoThreadFinished;
    SDL_Thread *video_tid;  //video thread id
    SDL_AudioDeviceID audioID;
    Player *player; //
} VideoState;

class Player : public QThread
{
    Q_OBJECT

public:

    enum PlayerState
    {
        Playing,
        Pause,
        Stop
    };

    explicit Player();
    ~Player();

    /**
     * @brief Set file_name (file_path) to the player. 
     * file_name is saved for replaying if required
     * TODO: If not valid file.
     * 
     * @param path: path to file.
     * @return true: if file valid.
     * @return false: if not valid file.
     */
    bool setFileName(QString path);

    /**
     * @brief Set the Volume to the player
     * 
     * @param volume: volume to set, (0-100)
     * @return true: ? 
     * @return false: ? 
     */
    bool setVolume(int volume);

   /**
    * @brief Check if the file is valid media file
    * 
    * @param path: path to the file
    * @return true: if valid
    * @return false: if not valid
    */
    bool isValidFile(QString path);

    /**
     * @brief Start Playing. If player have stopped previous media,
     *      it will start again on call of this method
     *
     * @return true: play success
     * @return false: if player is not paused
     */
    bool startPlay();

    /**
     * @brief Pauses playback.
     *
     * @return true: pause success
     * @return false: if player is not playing
     */
    bool pausePlay();

    /**
     * @brief Stops the playing of the media file
     *
     * @param wait_thread_end: If need to wait for threads to end
     * @return true: stopping success
     * @return false: stopping failed
     */
    bool stopPlay(bool wait_thread_end = false);

    /**
     * @brief Seek play from certain position
     *
     * @param pos: position in millisecond
     */
    void seekInMilliseconds(int64_t pos);

    /**
     * @brief Get the Total Time of the media file. (length)
     *
     * @return int64_t: time in milliseconds
     */
    int64_t getTotalTime();

    /**
     * @brief Get the Current Time the player is playing. time is converted to seconds.
     *
     * @return double: time in seconds
     */
    double getCurrentTime();


    /**
     * @brief emit signal that frame available. Used in media_process.h
     * 
     * @param img 
     */
    void disPlayVideo(QImage img);

signals:
    /**
     * @brief Send signal if new frame available
     * 
     */
    void getOneFrameSignal(QImage); 

    /**
     * @brief Send signal if PlayerState changed
     * 
     * @param state: the new state 
     */
    void stateChangedSignal(Player::PlayerState state);

    /**
     * @brief Send signal if maximun time is changed.
     *  (when new file is loaded)
     * 
     * @param uSec 
     */
    void totalTimeChangedSignal(qint64 uSec);
    /**
     * @brief 
     * when the decode is running, emit signal.
     */
    void decodeRunning();

protected:

    /**
     * @brief QThread function. The starting point of the thread.
     *  Setup player_states and start decoding. 
     * 
     */
    void run();

private:
    int volume_;

    QString file_name_;

    VideoState video_state_;

    PlayerState player_state_;

};

#endif // PLAYER_HPP
