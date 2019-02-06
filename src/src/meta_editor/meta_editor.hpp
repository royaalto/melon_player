/**
 * @file meta_editor.hpp
 * @author Linming Pan
 * @brief 
 * @version 0.1
 * @date 2018-11-25
 * 
 * 
 */

#ifndef META_EDITOR_HPP
#define META_EDITOR_HPP

#include <QString>
#include <QFileDialog>
#include <QPushButton>
#include <QWidget>


#include "metadata.hpp"

#include <ui_meta_editor.h>
//#include <ui_main_window.h>


class MetaEditor : public QWidget
{
    Q_OBJECT
public:
    MetaEditor(QWidget *parent=0);

    /**
     * @brief Init all required connections
     * 
     * @return true: on success
     * @return false: on failure
     */
    bool init();


    /**
     * @brief Set the Metadata to MetaEditor
     * 
     * @param md: metadata
     * @return true: on succeess
     * @return false: on failure
     */
    bool setMetadata(Mellon::Metadata md);

    /**
     * @brief When Save button clicked. Saved new metadata to the media file.
     *      After saving. meta editor window should be closed.
     * 
     */
    void onAcceptClicked();

    /**
     * @brief Close meta editor window without changing anyting
     * 
     */
    void onCancelClicked();

    /**
     * @brief NOTHING
     * 
     *  NOTICE: not implemented.
     * 
     * @param button 
     */
    void onButtonClicked(QAbstractButton * button);

    /**
     * @brief If any item is changed in meta editor
     * 
     *   NOTICE: not implemented.
     * 
     * @param item 
     */
    void onItemChanged(QTableWidgetItem *item);

private:
    Ui::MetaUI ui_;

    Mellon::Metadata meta_data_;

    bool metadata_edited = false;
};

#endif // META_EDITOR_HPP