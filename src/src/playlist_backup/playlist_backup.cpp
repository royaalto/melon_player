/**
 * @file playlist_backup.cpp
 * @author Linming Pan
 * @brief 
 * @version 0.1
 * @date 2018-11-28
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include "playlist_backup/playlist_backup.hpp"


PlaylistItemBackup::PlaylistItemBackup(QString file_path)
{
    
    file_path_ = file_path.toStdString();
    QFileInfo fi(file_path);
    file_name_ = fi.fileName().toStdString();
    if(!getMetadata(file_path, metadata_))
    {
        std::cout << "no metada" << std::endl;
    }

}


PlaylistItemBackup::~PlaylistItemBackup()
{

}

QString PlaylistItemBackup::getMetadataByKey(QString key)
{
    
    std::string std_key = (key.toLower()).toStdString();
    if("name" == key.toLower() )
    {
        //std::cout << "name " << file_name_ << std::endl;
        return QString::fromStdString(file_name_);
    }
    auto value = metadata_.data[std_key];
    return QString::fromStdString(value);
}

QString PlaylistItemBackup::getQStrName()
{
    return QString::fromStdString(file_name_);
}


QString PlaylistItemBackup::getQStrFilePath()
{
    return QString::fromStdString(file_path_);
}



// *****************************************************************


PlaylistBackup::PlaylistBackup()
{

}

PlaylistBackup::~PlaylistBackup()
{

}

PlaylistItemBackup PlaylistBackup::getItemByName(QString name)
{
    for(auto it=playlist_.begin(); it != playlist_.end(); it++)
    {
        if(it->getQStrName() == name)
        {
            return *it;
        }
    }
}

void PlaylistBackup::addToPlaylist(PlaylistItemBackup item)
{
    playlist_.push_back(item);
}

bool PlaylistBackup::deleteFromPlaylistByName(QString name)
{
    for(auto it=playlist_.begin(); it != playlist_.end(); it++)
    {
        if(it->getQStrName() == name)
        {
            playlist_.erase(it);
            break;
        }
    }
}

QList<QTreeWidgetItem *> PlaylistBackup::getTreeWidgeItemList(QList<QString> params)
{
    QList<QTreeWidgetItem *> item_list;
    
    for (int i = 0; i < (int)playlist_.size(); ++i)
    {
        playlist_.at(i);
        QTreeWidgetItem * item = new QTreeWidgetItem();
        for (int j = 0; j < params.size(); j++)
        {
            QString value = playlist_.at(i).getMetadataByKey(params.at(j));
            if(!value.isEmpty())
            {
                item->setText(j,value);
                //std::cout << "[" << j <<  "]" << "=" << value.toStdString() << std::endl;
            }
            //else
            //std::cout << "empty value " << j << "=" << value.toStdString() <<  std::endl;
            
        }
        //std::cout << "add item " << i << std::endl;
        item_list.append(item);
    }
    return item_list;
}

