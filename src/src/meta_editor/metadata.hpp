/**
 * @file metadata.hpp
 * @author 
 * @brief 
 * @date 2018-11-24
 * 
 * 
 */

#ifndef MELLON_METADATA_HPP
#define MELLON_METADATA_HPP


#include <QString>
#include <string>
#include <sstream>
#include <iostream>


#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
//#include "libavutil/time.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
 #include "time1.h"


namespace Mellon
{
    typedef struct Metadata 
    {
        std::string file_path;
        std::map<std::string, std::string> data;
        std::string print_dump;
        
    } Metadata;

    /**
     * @brief Get the Metadata from media file
     * 
     * @param file_path: path to the media file
     * @param md: Metadata save here
     * @return true: Metadata get sccessfully
     * @return false: filed to load metadata from file
     */
    inline bool getMetadata(QString file_path, Metadata &md)
    {
        AVFormatContext *fmt_ctx = NULL;
        AVDictionaryEntry *tag = NULL;
        int ret;

        std::map<std::string, std::string> meta;

        ret = avformat_open_input(&fmt_ctx, file_path.toStdString().c_str(), NULL, NULL);
        if ( ret != 0)
        {
            std::cout << "open file failed" << std::endl;
            return 0;
        }
        md.file_path = file_path.toStdString();

        std::stringstream ss;
        ss << "Metadata" << std::endl 
        << "  file path:" << md.file_path << std::endl;
        
        while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
        {
            //printf("%s=%s\n", tag->key, tag->value);
            ss << "    " << tag->key <<  "=" << tag->value << std::endl;
            md.data[tag->key] = tag->value;

        }
        // add duration which is not in metadata
        quint64 sec = fmt_ctx->duration / 1000000;
        std::cout << "-total time: " <<  sec << std::endl;
        QString min_str = QString("00%1").arg( sec / 60 );
        QString sec_str = QString("00%1").arg( sec % 60 );
        QString time_str = QString("%1:%2").arg(min_str.right(2)).arg(sec_str.right(2));

        md.data["duration"] = time_str.toStdString();
        avformat_close_input(&fmt_ctx);

        md.print_dump = ss.str();

        return true;
    }

}

#endif