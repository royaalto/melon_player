/**
 * @file main_window.cpp
 * @author Linming Pan
 * @date 12.11.2018
 * @brief Example code
 *
 */
#include "main_window.h"
#include <ui_main_window.h>
QString GetQssStr(QString strQssPath)
{
    QString strQss;
    QFile FileQss(strQssPath);
    if (FileQss.open(QIODevice::ReadOnly))
    {
        strQss = FileQss.readAll();
        FileQss.close();
    }
    else
    {
        //qDebug() << "读取样式表失败" << strQssPath;
    }
    return strQss;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //drag and drop
    this->setAcceptDrops(true);
    // Setup ui from file q_test.ui
    ui_.setupUi(this);
    setWindowTitle("Melön Player");
    // Set up visuals and text labels
    QPixmap play_image("../icon/start.png");
    QPixmap stop_image("../icon/stop.png");
    QPixmap pause_image("../icon/pause.png");
    QPixmap add_image("../icon/add.png");
    QPixmap open_image("../icon/open.png");
    QPixmap remove_image("../icon/remove.png");

    QIcon play_icon, pause_icon, stop_icon, forward_icon, backward_icon, 
    volume_icon, add_icon, open_icon, remove_icon, edit_icon;
    play_icon.addPixmap(play_image);
    stop_icon.addPixmap(stop_image);
    pause_icon.addPixmap(pause_image);
    forward_icon.addPixmap(QPixmap("../icon/fastforward.png"));
    backward_icon.addPixmap(QPixmap("../icon/fastbackwards.png"));
    volume_icon.addPixmap(QPixmap("../icon/volume.png"));
    add_icon.addPixmap(add_image);
    open_icon.addPixmap(open_image);
    remove_icon.addPixmap(remove_image);
    edit_icon.addPixmap(QPixmap("../icon/edit.png"));

    ui_.play_button->setIcon(play_icon);
    ui_.pause_button->setIcon(pause_icon);
    ui_.stop_button->setIcon(stop_icon);
    ui_.forward_button->setIcon(forward_icon);
    ui_.backward_button->setIcon(backward_icon);
    ui_.volume_label->setIcon(volume_icon);
    ui_.add_button->setIcon(add_icon);
    ui_.open_button->setIcon(open_icon);
    ui_.delete_button->setIcon(remove_icon);
    ui_.edit_button->setIcon(edit_icon);

    ui_.play_button->setIconSize(QSize(50,50));
    ui_.pause_button->setIconSize(QSize(50,50));
    ui_.stop_button->setIconSize(QSize(50,50));
    ui_.forward_button->setIconSize(QSize(50,50));
    ui_.backward_button->setIconSize(QSize(50,50));
    ui_.volume_label->setIconSize(QSize(30,30));

    ui_.add_button->setIconSize(QSize(80,45));
    ui_.open_button->setIconSize(QSize(80,45));
    ui_.delete_button->setIconSize(QSize(80,45));
    ui_.edit_button->setIconSize(QSize(80,45));


    // Make buttons lines transparency
    ui_.play_button->setStyleSheet("background:transparent");
    ui_.pause_button->setStyleSheet("background:transparent");
    ui_.stop_button->setStyleSheet("background:transparent");
    ui_.forward_button->setStyleSheet("background:transparent");
    ui_.backward_button->setStyleSheet("background:transparent");
    ui_.volume_label->setStyleSheet("background:transparent");
    ui_.add_button->setStyleSheet("background:transparent");
    ui_.open_button->setStyleSheet("background:transparent");
    ui_.delete_button->setStyleSheet("background:transparent");
    ui_.edit_button->setStyleSheet("background:transparent");
    QString qss = GetQssStr("./qss/mainwid.css");
    setStyleSheet(qss);

    // hide not used widgets
    ui_.x_button->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete(player_);
}

bool MainWindow::init()
{
    // setup ffmpeg
    avformat_network_init();

    // setup SDL
    if (SDL_Init(SDL_INIT_AUDIO)) {
        fprintf(stderr,"Could not initialize SDL - %s. \n", SDL_GetError());
        exit(1);
    }
    player_ = new Player;


    // Set up all the connections
    connect(ui_.play_button, &QPushButton::released, this, &MainWindow::onPlayClicked);
    connect(ui_.pause_button, &QPushButton::released, this, &MainWindow::onPauseClicked);
    connect(ui_.stop_button, &QPushButton::released, this, &MainWindow::onStopClicked);

    connect(ui_.open_button, &QPushButton::released, this, &MainWindow::onOpenFileClicked);
    connect(ui_.delete_button, &QPushButton::released, this, &MainWindow::onDeleteClicked);

    connect(ui_.forward_button, &QPushButton::released, this, &MainWindow::onForwardClicked);
    connect(ui_.backward_button, &QPushButton::released, this, &MainWindow::onBackwardClicked);

    connect(ui_.add_button, &QPushButton::released, this, &MainWindow::onAddButtonClicked);
  

    connect(ui_.seek_slider, &QSlider::sliderMoved, this, &MainWindow::onSeekSliderChanged);
    connect(ui_.volume_slider, &QSlider::sliderMoved, this, &MainWindow::onVolumeSliderChanged);
    connect(ui_.x_button, &QPushButton::released, this, &MainWindow::onXButtonClicked);
    connect(ui_.edit_button, &QPushButton::released, this, &MainWindow::onEditClicked);
    //connect(ui_.seek_slider, &QSlider::sliderPressed, this, &MainWindow::onPauseClicked);
    //connect(ui_.seek_slider, &QSlider::sliderReleased, this, &MainWindow::onPlayClicked);



    connect(player_, &Player::getOneFrameSignal, this, &MainWindow::painter );
    connect(player_, &Player::totalTimeChangedSignal, this, &MainWindow::onSeekSliderRangeChanged );
    connect(player_, &Player::decodeRunning, this, &MainWindow::onPlaySliderChanged );
    connect(player_, &Player::stateChangedSignal, this, &MainWindow::updatePlayerState );

   

    connect(ui_.treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::onTreeItemDoubleClicked);
    connect(ui_.treeWidget, &QTreeWidget::currentItemChanged, this, &MainWindow::onTreeItemChanged);
    //connect(ui_.sort_comboBox, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::activated), this, &MainWindow::onSortActivated);


    // When a signal has multiple options
    //connect(ui_.spinBox, static_cast<void(QSpinBox::*)(int)>                (&QSpinBox::valueChanged), this, &MainWindow::onValueChanged);
    //connect(ui_.spinBox, static_cast<void(QSpinBox::*)(const QString &text)>(&QSpinBox::valueChanged), this, &MainWindow::onValueChanged); <- this parameter need change

    
    return true;
}


void MainWindow::onPauseClicked()
{
    std::cout << "Pause Cliked" <<  std::endl;
    player_->pausePlay();
}
void MainWindow::onPlayClicked()
{
    std::cout << "Play Cliked" <<  std::endl;
    player_->startPlay();
    updatePlaylist();

}

void MainWindow::onStopClicked()
{
    std::cout << "Stop Cliked" <<  std::endl;
    player_->stopPlay(true);
}

void MainWindow::onXButtonClicked()
{
    std::cout << "x button" << std::endl;
}

void MainWindow::onOpenFileClicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, "", getenv("HOME"),
                    "Videos (*.flv *.rmvb *.avi *.mp4 *.mkv);; All (*.*) ");


    // if file ok, setFile and start play
    if (player_->isValidFile(file_name))
    {
        playlist_.addToPlaylist(PlaylistItemBackup(file_name));
        player_->stopPlay(true);

        player_->setFileName(file_name);
        player_->start();

        updatePlaylist();
    }
}


void MainWindow::openDragFile(QString file_name)
{
    if (player_->isValidFile(file_name))
    {
        playlist_.addToPlaylist(PlaylistItemBackup(file_name));
        player_->stopPlay(true);

        player_->setFileName(file_name);
        player_->start();

        updatePlaylist();
    }
}
void MainWindow::onDeleteClicked()
{
    std::cout  << "delete clicked" << std::endl;
    if(current_item_ != nullptr)
    {
        std::cout  << "current item: " << current_item_->text(0).toStdString() << std::endl;
        playlist_.deleteFromPlaylistByName(current_item_->text(0));
        current_item_ = nullptr;

        updatePlaylist();
    }
}

void MainWindow::onForwardClicked()
{
    double current_time = player_->getCurrentTime();
    int sec = (int)current_time;
    player_->seekInMilliseconds( (sec+3) * 1000000);
}

void MainWindow::onBackwardClicked()
{
    double current_time = player_->getCurrentTime();
    int sec = (int)current_time;
    player_->seekInMilliseconds( (sec-3) * 1000000);
}

void MainWindow::onAddButtonClicked()
{
    QStringList file_list = QFileDialog::getOpenFileNames(this, "", getenv("HOME"),
                   "Videos (*.flv *.rmvb *.avi *.mp4 *.mkv);; All (*.*) ");

    for (  auto item : file_list  )
    {
        if(player_->isValidFile(item))
        {
            playlist_.addToPlaylist(PlaylistItemBackup(item));
        }
    }
    updatePlaylist();
}


void MainWindow::onSeekSliderChanged(qint64 sec)
{
    //std::cout << "Seek moved to " << (sec * 1000000) << std::endl;

    // seek time in milliseconds,
    player_->seekInMilliseconds(sec * 1000000);
}

void MainWindow::onSeekSliderRangeChanged(qint64 u_sec)
{
    std::cout << "Seek moved to " << u_sec << std::endl;

    qint64 sec = u_sec / 1000000;
    //ui_.seek_slider->setValue(sec);
    ui_.seek_slider->setRange(0,sec);
    std::cout << "total time: " <<  sec << std::endl;

    QString min_str = QString("00%1").arg(sec/60);
    QString sec_str = QString("00%1").arg(sec%60);

    QString time_str = QString("%1:%2").arg(min_str.right(2)).arg(sec_str.right(2));

    ui_.seek_max_time_label->setText(time_str);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    // if player stopped 
    //player_.
    painter.drawRect(ui_.frame->x(), ui_.frame->y(), ui_.frame->width(), ui_.frame->height());

    // if no image, no paiting required
    if (image_.size().width() <= 0) return;

    QImage img = image_.scaled(
        ui_.frame->width() -2,
        ui_.frame->height() -2,
        Qt::KeepAspectRatio
    );

    int x = ui_.frame->width() - img.width();
    int y = ui_.frame->height() - img.height();

    x /= 2;
    y /= 2;

    x += ui_.frame->x() +1;
    y += ui_.frame->y() +1;

    painter.drawImage(QPoint(x,y),img);
}

void MainWindow::painter(QImage image)
{
    image_ = image;
    update();
}

void MainWindow::onVolumeSliderChanged(qint16 volume)
{
    int volume1 = (int)volume;
    player_->setVolume(volume1);
}


void MainWindow::onEditClicked()
{
    std::cout  << "Edit clicked" << std::endl;

    if(current_item_ != nullptr)
    {
        std::cout  << "current item: " << current_item_->text(0).toStdString() << std::endl;

        PlaylistItemBackup playlist_item = playlist_.getItemByName(current_item_->text(0));
        QString file_name = playlist_item.getQStrFilePath();

        Mellon::Metadata md;
        if(Mellon::getMetadata(file_name, md))
        {
            std::cout << md.print_dump << std::endl;
            MetaEditor *md_editor = new MetaEditor();
            md_editor->setMetadata(md);
            md_editor->show();
        }
    }
}

void MainWindow::updatePlayerState(Player::PlayerState state)
{
    std::cout << "stade update" << std::endl;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{

    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{

    QString name = event->mimeData()->urls().first().toString();
    std::cout<<"name :"<<name.toUtf8().constData()<<std::endl;
    openDragFile(name);
}


void MainWindow::updatePlaylist()
{
    QList<QString> params = {"name","title", "album","duration"};
    QList<QTreeWidgetItem *> liiist = playlist_.getTreeWidgeItemList(params);
    ui_.treeWidget->clear();
    ui_.treeWidget->addTopLevelItems(liiist);
}

void MainWindow::onPlaySliderChanged()
{
    double current_time = player_->getCurrentTime();
    int sec = (int)current_time;
    QString min_str = QString("00%1").arg( sec / 60 );
    QString sec_str = QString("00%1").arg( sec % 60 );
    QString time_str = QString("%1:%2").arg(min_str.right(2)).arg(sec_str.right(2));
    ui_.seek_time_label->setText(time_str);
    ui_.seek_slider->setValue(sec); 
}

void MainWindow::onTreeItemDoubleClicked(QTreeWidgetItem * item, int column)
{
    std::cout  << "tree item double click" << std::endl;
    int nameColumn = 0;
    PlaylistItemBackup playlist_item = playlist_.getItemByName(item->text(nameColumn));

    QString file_name = playlist_item.getQStrFilePath();

    player_->stopPlay(true);
    player_->setFileName(file_name);
    player_->start();
}

void MainWindow::onTreeItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    std::cout  << "current tree item changed" << std::endl;
    current_item_ = current;
}