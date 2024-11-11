#include "main_window.h"


// Конструктор.
ImageViewer::ImageViewer(QWidget* parent) : QMainWindow(parent), current_index(0)
{
    // Создание графических элементов.
    image_label = new QLabel(this);
    image_label->setAlignment(Qt::AlignCenter);
    image_label->setBackgroundRole(QPalette::Base);
    image_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    brightness_slider = new QSlider(Qt::Horizontal, this);
    brightness_slider->setRange(min_brightness, max_brightness);
    brightness_slider->setValue(start_brightness); 
    brightness_label = new QLabel(this);
    set_brightness_label_value(brightness_slider->value());
    
    next_button = new QPushButton("Next image", this);
    prev_button = new QPushButton("Previos image", this);
    images_slider = new QSlider(Qt::Horizontal, this);
    images_slider->setValue(current_index);
    
    info_label = new QLabel(this); 
    info_label->setAlignment(Qt::AlignLeft);
    set_info_label("No image", "no extension", "No path", "No created", 0);

    image_index_label = new QLabel(this);
    image_index_label->setAlignment(Qt::AlignCenter);
    
    file_menu = menuBar()->addMenu("File");
    open_action = new QAction("Open", this);
    file_menu->addAction(open_action);
    about_action = new QAction("About", this);
    file_menu->addAction(about_action);
    

    // Подключение сигналов к соответствующим слотам.
    connect(prev_button, &QPushButton::clicked, this, &ImageViewer::previous_image);
    connect(next_button, &QPushButton::clicked, this, &ImageViewer::next_image);
    connect(brightness_slider, &QSlider::valueChanged, this, &ImageViewer::change_brightness);
    connect(open_action, &QAction::triggered, this, &ImageViewer::load_images_from_directory);
    connect(about_action, &QAction::triggered, this, &ImageViewer::show_about_dialog);
    connect(images_slider, &QSlider::valueChanged, this, &ImageViewer::slider_index_image);
    

    // Расположение на окне.
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(image_label);
    
    QHBoxLayout* info_brightness_layout = new QHBoxLayout;
    info_brightness_layout->addWidget(info_label);
    info_brightness_layout->addWidget(brightness_slider);
    info_brightness_layout->addWidget(brightness_label);
    layout->addLayout(info_brightness_layout); 
    
    QHBoxLayout* button_layout = new QHBoxLayout;
    button_layout->addWidget(prev_button);
    button_layout->addWidget(images_slider);
    button_layout->addWidget(image_index_label);
    button_layout->addWidget(next_button);
    layout->addLayout(button_layout);
    
    QWidget* central_widget = new QWidget(this);
    central_widget->setLayout(layout);
    setCentralWidget(central_widget);
}

// Деструктор.
ImageViewer::~ImageViewer() 
{
    images.clear();
}

// Метод для изменения индекса изображения на следующий.
void ImageViewer::next_image()
{
    if (!images.empty())
    {
        current_index = (current_index + 1) % images.size();
        images_slider->setValue(current_index + 1);
        update_image();
    }
}

// Метод для изменения индекса изображения на предыдущий.
void ImageViewer::previous_image()
{
    if (!images.empty())
    {
        current_index = (current_index - 1 + images.size()) % images.size();
        images_slider->setValue(current_index + 1);
        update_image();
    }
}

// Метод для изменения индекса изображения слайдером.
void ImageViewer::slider_index_image(const int value)
{
    if (!images.empty())
    {
        current_index = (value - 1) % images.size();
        update_image();
    }
}

// Метод для изменения яркости изображения.
void ImageViewer::change_brightness(int value)
{
    if (images.empty()) return;
    set_brightness_label_value(value);
    QImage new_image = images[current_index].second.convertToFormat(QImage::Format_ARGB32);
    const int width = new_image.width();
    const int height = new_image.height();
    uchar* data = new_image.bits();
#pragma omp parallel for 
    for (int y = 0; y < height; ++y)
    {
#pragma omp parallel for 
        for (int x = 0; x < width; ++x)
        {
            int idx = (y * width + x) * 4; 
            int new_red = qBound(min_color, data[idx + 2] + value, max_color); 
            int new_green = qBound(min_color, data[idx + 1] + value, max_color); 
            int new_blue = qBound(min_color, data[idx] + value, max_color); 
            data[idx] = new_blue;
            data[idx + 1] = new_green;
            data[idx + 2] = new_red;
        }
    }
    image_label->setPixmap(QPixmap::fromImage(new_image));
    image_label->adjustSize();
}

// Метод для загрузки изображений из директории.
void ImageViewer::load_images_from_directory()
{
    if (!images.empty()) images.clear();
    QString dir_path = QFileDialog::getExistingDirectory(this, "Select Directory");
    if (dir_path.isEmpty()) return;

    QDir dir(dir_path);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp";
    dir.setNameFilters(filters);
    
    for (const QString& name : dir.entryList(QDir::Files))
    {
        images.push_back(std::make_pair(QFileInfo(dir_path + "/" + name), QImage(dir_path + "/" + name).scaled(image_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    }
    
    current_index = 0;
    images_slider->setRange(1, images.size());
    update_image();
}

// Метод для показа окна about.
void ImageViewer::show_about_dialog()
{
    QMessageBox::about(this, "About Image Viewer", description);
}

// Метод для обновления отображаемого изображения.
void ImageViewer::update_image()
{
    if (images.empty()) 
    {
        image_label->clear();
        set_image_index_label(0, 0);
        set_brightness_label_value(start_brightness);
        set_info_label("No image", "No extension", "No path", "No created", 0);
        return;
    }
    set_image_index_label(current_index + 1, images.size());
    set_brightness_label_value(brightness_slider->value());
    
    QFileInfo image_info(images[current_index].first);
    set_info_label(image_info.fileName(), image_info.suffix(), image_info.absoluteFilePath(), image_info.birthTime().toString(), image_info.size());
    
    //QImage scaled_image = images[current_index].second.scaled(image_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    image_label->setPixmap(QPixmap::fromImage(images[current_index].second));
    image_label->adjustSize();
}

// Метод для установки значения яркости в метку.
void ImageViewer::set_brightness_label_value(const int value)
{
    brightness_label->setText(QString("Brightness from %1 to %2: %3").arg(min_brightness).arg(max_brightness).arg(value));
}

// Метод для установки текущего индекса изображения в метку.
void ImageViewer::set_image_index_label(const int index, const int quantity)
{
    image_index_label->setText(QString("Image %1 of %2").arg(index).arg(quantity));
}

// Метод для установки информации об изображении в метку.
void ImageViewer::set_info_label(const QString& name, const QString& extension, const QString& absolute_file_path, const QString& date_created, const qint64& size)
{
    info_label->setText(QString("Name: %1\n"
        "Extension: %2\n"
        "Creation Date: %3\n"
        "Size: %4 bytes\n"
        "Path: %5\n")
        .arg(name)                 // Имя файла
        .arg(extension)            // Расширение файла
        .arg(date_created)         // Дата создания
        .arg(size)                 // Размер файла); 
        .arg(absolute_file_path)); // Полный путь к файлу
}