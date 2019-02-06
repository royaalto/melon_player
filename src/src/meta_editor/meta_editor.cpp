/**
 * @file meta_editor.cpp
 * @author Linming Pan
 * @brief 
 * @version 0.1
 * @date 2018-11-25
 * 
 * 
 */

#include "meta_editor.hpp"


MetaEditor::MetaEditor(QWidget *parent) 
    : QWidget(parent)
{
    // Setup ui from file q_test.ui
    ui_.setupUi(this);
    setWindowTitle("file name");

    init();
}

bool MetaEditor::init()
{
    connect(ui_.buttonBox, &QDialogButtonBox::accepted, this, &MetaEditor::onAcceptClicked);//save
    
    connect(ui_.buttonBox, &QDialogButtonBox::rejected, this, &MetaEditor::onCancelClicked);

    connect(ui_.tableWidget, &QTableWidget::itemChanged, this, &MetaEditor::onItemChanged);

    //connect(ui_.buttonBox, &QDialogButtonBox::rejected, this, &MetaEditor::onCancelClicked);

    return 1;
}


bool MetaEditor::setMetadata(Mellon::Metadata md)
{
    std::map<std::string, std::string> meta = md.data;
    meta_data_=md;
    std::vector<std::string> metadata_list = 
    {
        "album", "composer","genre","copyright", 
        "encoded_by","title","language","artist", 
        "album_artist","performer", "disc",
        "publisher","track","encoder","lyrics"
    };


    std::cout << "size of metadata: " <<  meta.size() << std::endl;

    std::map<std::string, std::string>::iterator it;

    for ( it = meta.begin(); it != meta.end(); it++ )
    {   

        int pos = std::distance(metadata_list.begin(), std::find(metadata_list.begin(), metadata_list.end(), it->first));

        if(pos < metadata_list.size())
        {
            QTableWidgetItem *qtw_item = new QTableWidgetItem(QString::fromStdString(it->second));
            ui_.tableWidget->setItem(pos, 1, qtw_item);
        }
        //std::cout << it->first << ':' << it->second << std::endl;
    }

}

void MetaEditor::onAcceptClicked()
{
        // Get total count of rows it the table.
    int row_count = ui_.tableWidget->rowCount();
    std::vector<std::string> metadata_list = 
    {
        "album", "composer","genre","copyright", 
        "encoded_by","title","language","artist", 
        "album_artist","performer", "disc",
        "publisher","track","encoder","lyrics"
    };
    int key_column = 0;
    int value_column = 1;
    std::string ffmpeg_head = "ffmpeg -y -i " + meta_data_.file_path;
    std::string ffmpeg_body;

    for (int row = 0; row < row_count; row++)
    {
        QTableWidgetItem * ptr_to_table_item_key = nullptr;  
        QTableWidgetItem * ptr_to_table_item_value = nullptr; 

        ptr_to_table_item_key = ui_.tableWidget->item(row,key_column);
        QString key = ptr_to_table_item_key->text();

        ptr_to_table_item_value = ui_.tableWidget->item(row,value_column);
        QString value = "";
        // check if item exist
        if(ptr_to_table_item_value != nullptr)
        {
            value = ptr_to_table_item_value->text(); 
        }
        //std::cout << key.toStdString() << "=" << value.toStdString() << std::endl;
        ffmpeg_body=ffmpeg_body + " -metadata " + metadata_list[row] + "="  +"\""+ value.toStdString()+"\"";

    }
    
    std::string ffmpeg_full = ffmpeg_head + ffmpeg_body + " -codec copy "+ "tmp.mp4";
    system(ffmpeg_full.c_str());

    std::string cmd_delete = "rm "+  meta_data_.file_path;
    system(cmd_delete.c_str());
    
    std::string ffmpeg_full2 = "ffmpeg -y -i tmp.mp4" + ffmpeg_body + " "+meta_data_.file_path;
    system(ffmpeg_full2.c_str());
    // std::cout<<"ffmpeg_full2 "<<ffmpeg_full2<<std::endl;
    
    std::string cmd_delete2 = "rm tmp.mp4";
    system(cmd_delete2.c_str());
    // std::cout<<"row: "<<ui_.tableWidget->rowCount();
    this->close();
 
}

void MetaEditor::onCancelClicked()
{
    std::cout << "cancel" << std::endl;
    this->close();
}

void MetaEditor::onButtonClicked(QAbstractButton * button)
{
    
}

void MetaEditor::onItemChanged(QTableWidgetItem *item)
{
    std::cout << "edited" << std::endl;
}