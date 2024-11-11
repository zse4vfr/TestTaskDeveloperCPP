#pragma once
#ifndef _MAIN_WINDOW_H__
#define _MAIN_WINDOW_H__


// Константы.
#define start_brightness 0
#define min_brightness -255
#define max_brightness 255
#define max_color 255
#define min_color 0


#include <vector>
#include <omp.h>
#include <QMainWindow>
#include <QDir>
#include <QImage>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>


const QString description("This is a simple image viewer application built using Qt.\n"
    "You can choose 'File->Open' to select a directory filled with images (.bmp, .jpg, .png). "
    "After that, you can view the images in that directory.\n"
    "Navigation is performed using the 'Next Image' and 'Previous Image' buttons.\n"
    "Brightness can be adjusted with the slider below the image.");


QT_BEGIN_NAMESPACE
namespace UI { class ImageViewer; }
QT_END_NAMESPACE


class ImageViewer : public QMainWindow
{
public:
    ImageViewer(QWidget* parent = nullptr);
    ~ImageViewer();

private slots:
    void next_image();
    void previous_image();
    void slider_index_image(const int value);
    void change_brightness(int value);
    void load_images_from_directory();
    void show_about_dialog();

private:
    void update_image();
    void set_brightness_label_value(const int value);
    void set_image_index_label(const int index, const int quantity);
    void set_info_label(const QString& name, const QString& extension, const QString& absolute_file_path, const QString& date_created, const qint64& size);

    
    QMenu* file_menu;
    QAction* open_action;
    QAction* about_action;
    
    QLabel* image_label;
    QLabel* info_label; 
    QLabel* image_index_label; 
    std::vector<std::pair<QFileInfo, QImage>> images;
    QSlider* images_slider;
    int current_index;
    
    QLabel* brightness_label;
    QSlider* brightness_slider;
    
    QPushButton* next_button;
    QPushButton* prev_button;
};


#endif // _MAIN_WINDOW_H__