/**
 * @file main_window.hpp
 * @author Linming Pan
 * @date 12.11.2018
 * @brief Example code
 *
 */

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QPushButton>
#include <QMainWindow>
#include <QFileDialog>
#include <QPaintEvent>
#include <QPainter>

#include <memory>
#include <iostream>

#include <QUrl>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include <QSettings>
#include <exception>
#include <QtGlobal>
#include <QStringList>
#include <QCoreApplication>
#include <QListWidget>
#include <QMessageBox>
#include <QTreeWidgetItem>

#include "player.hpp"

#include "playlist_backup.hpp"
#include "meta_editor/meta_editor.hpp"
#include <ui_main_window.h>
#include <QDragEnterEvent>
#include <QMimeData>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    /**
     * @brief Construct a new Main Window object
     *
     * @param parent: set parent if have.
     */
    MainWindow(QWidget *parent=0);
    ~MainWindow();

    /**
     * @brief Initialize all required setting.
     *  including connections.
     *
     * @return true: if everyting ok
     * @return false: something wrong
     */
    bool init();

    /**
     * @brief Play button clicked. Start playing if file loaded.
     * If previous media have stopped/ended, it will restart playing the same file.
     *
     */
    void onPlayClicked();

    /**
     * @brief If playing, Change state to Pause.
     *
     */
    void onPauseClicked();

    /**
     * @brief Stop button clicked. Stops currently playing media file if there is
     *
     */
    void onStopClicked();

    /**
     * @brief Delete selected item from playlist, if anying selected.
     *
     */
    void onDeleteClicked();

    /**
     * @brief Forwards play by 3 seconds. Continues playing, if state is Playing.
     *
     */
    void onForwardClicked();

    /**
     * @brief Backwards play by 3 seconds. Continues playing, if state is Playing.
     *
     */
    void onBackwardClicked();
    
    /**
     * @brief Opens Meta Editor window for selected file. The file information is saved 
     *  current_item_.  If current_item_ points to nullptr, nothing happens.
     * 
     */
    void onEditClicked();

    /**
     * @brief TODO: Not used. ignore this
     * 
     */
    void onXButtonClicked();

    /**
     * @brief update playerStatus
     * 
     * @param state
     */
    void updatePlayerState(Player::PlayerState state);


    /**
     * @brief If seek slided is moved, changed playing time to the value
     *
     * @param sec: The time in seconds, where seek request is done.
     */
    void onSeekSliderChanged(qint64 sec);


    /**
     * @brief Detect if seek slider range have changed (max time changed). If changed,
     * update maximun time in UI.
     *
     * @param u_sec
     */
    void onSeekSliderRangeChanged(qint64 u_sec);


    /**
     * @brief Opens file and start playing. Also adds the opened file to playlist.
     * If file is not playable media file, it should not be added to playlist.
     *
     */
    void onOpenFileClicked();


    /**
     * @brief Process drag event and add file to the playlist. Also start playing.
     * 
     * @param file_name 
     */
    void openDragFile(QString file_name);

    /**
     * @brief Used to paint frames to the screen. This is overide functinon.
     *
     * @param event: --
     */
    void paintEvent(QPaintEvent *event);

    /**
     * @brief Connect to this method, when new frame available.
     * Set up image to draw and calls update()-methods.
     *
     * @param image: image to draw.
     */
    void painter(QImage image);


    /**
     * @brief On volule slider changed. Change volume to desired value.
     *
     * @param volume: volume where to change. (0-100)
     */
    void onVolumeSliderChanged(qint16 volume);

    /**
     * @brief Change slider time and position based on audio time.
     * 
     */
    void onPlaySliderChanged();


    /**
     * @brief Update current selected item in the treeWidget.
     * 
     * @param current: pointer to current selected item
     * @param previousÖ: not used
     */
    void onTreeItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    /**
     * @brief On treeItem double clicked, start playing it
     * 
     * @param item: the item clicked
     * @param column: the column of clicked item. (not used)
     */
    void onTreeItemDoubleClicked(QTreeWidgetItem * item, int column);

    //void onSortActivated(const QString &text);


    /**
     * @brief When add button is clicked. User can select multiple file at same time.
     *  Validity of all file should be checked. If valid, add to the playlist.
     * 
     */
    void onAddButtonClicked();

    /**
     * @brief Update playlist. Clear and repopulate QTreeWidget. Should be 
     *  called when tree is edited.
     * 
     */
    void updatePlaylist();
    

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    Ui::MainWindow ui_;

    // Used to render image to the widget
    QImage image_;

    Player* player_;

    bool play_state_;

    PlaylistBackup playlist_;

    // save current selecter item in playlist
    QTreeWidgetItem *current_item_ = nullptr;

};

#endif
