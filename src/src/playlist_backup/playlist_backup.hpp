/**
 * @file playlist_backup.hpp
 * @author Linming Pan
 * @brief 
 * @version 0.1
 * @date 2018-11-27
 * 
 * @copyright Copyright (c) 2018
 * 
 */


#ifndef PLAYLIST_BACKUP_HPP
#define PLAYLIST_BACKUP_HPP


#include <string>
#include <vector>
#include <functional>
#include <QString>
#include <QList>
#include <QFileInfo>
#include <QTreeWidgetItem>
#include "meta_editor/meta_editor.hpp"

class PlaylistItemBackup
{
public:

    PlaylistItemBackup(QString file_path);

    ~PlaylistItemBackup();

    /**
     * @brief Get the Metadata By key
     * 
     * @param key: key of metadata
     * @return QString: value of the metadata
     */
    QString getMetadataByKey(QString key);

    /**
     * @brief Get name of the object. (file_name)
     * 
     * @return QString: file_name_
     */
    QString getQStrName();


    /**
     * @brief get filePath of the object
     * 
     * @return QString: file_path_ 
     */
    QString getQStrFilePath();

private:

    std::string file_name_;

    std::string file_path_;

public:
    Mellon::Metadata metadata_;
};


class PlaylistBackup
{


public:

    PlaylistBackup();

    ~PlaylistBackup();

    /**
     * @brief Get PlaylistItem by Name
     * 
     * @param name: name of the playlistitem
     * @return PlaylistItemBackup 
     */
    PlaylistItemBackup getItemByName(QString name);


    /**
     * @brief Add playlistitem to the playlist
     * 
     * @param item: item to add
     */
    void addToPlaylist(PlaylistItemBackup item);

    //bool deleteFromPlaylist(PlaylistItemBackup item);

    /**
     * @brief Delete playlistitem from playlist by the name of playlistitem
     * 
     * @param name: playlistitem name
     * @return true: if name found
     * @return false: if name not found. 
     */
    bool deleteFromPlaylistByName(QString name);

    /**
     * @brief Get list of TreeWidgeItem. Used for populating QTreeWidget
     * params contains all 
     * 
     * @param params: Each QList item as column in QTreeWidgetItem
     * @return QList<QTreeWidgetItem *> 
     */
    QList<QTreeWidgetItem *> getTreeWidgeItemList(QList<QString> params);


private:

    std::vector<PlaylistItemBackup> playlist_;

    
};



#endif //PLAYLIST_BACKUP_HPP