#include "FileListModel.h"
#include <filesystem>
#include <QDebug>

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    setupIcons();
}

void FileListModel::updateData(const std::vector<FileEntry>& newFiles)
{
    beginResetModel();

    m_files = newFiles;

    endResetModel();
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_files.size());
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_files.size() || index.row() < 0)
        return QVariant();

    const FileEntry& file = m_files[index.row()];

    // роль DisplayRole это текст возвращает отформатированый текст для отобрадения имени и размера
    if (role == Qt::DisplayRole) {
        QString name = QString::fromStdString(file.GetName());
        
        QString size = QString::fromStdString(file.GetFormattedSize());

        if(file.IsDirectory())
        {
            return name;
        }else
        {
            //this is костыль
            // упрощения для нужного мне отображения имени и размера
            return name + "\n" + size;
        }
    }

    // роль DecorationRole возвращает иконку для файла 
    if (role == Qt::DecorationRole) {
        return getIconForFile(file);
    }

    // роль TextAlignmentRole какое выравнивание будет у текста
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter; // Текст по центру
    }

    return QVariant();
}

void FileListModel::setupIcons()
{
    // Загружаем картинки 
    m_folderIcon = QIcon(":/res/folder.svg");
    m_fileIcon = QIcon(":/res/file.svg");

    QIcon iconArchive(":/res/archivse.svg");
    QIcon iconVideo(":/res/videos.svg");
    QIcon iconAudio(":/res/audios.svg");
    QIcon iconImage(":/res/images.svg");
    QIcon iconPdf(":/res/pdf.svg");
    QIcon iconTxt(":/res/txt.svg");
    QIcon iconWeb(":/res/web_txt.svg");

    // Заполняем словарь
 
    // Видео
    m_iconCache[".mp4"] = iconVideo;
    m_iconCache[".avi"] = iconVideo;
    m_iconCache[".mkv"] = iconVideo;
    m_iconCache[".mov"] = iconVideo;

    // Аудио
    m_iconCache[".mp3"] = iconAudio;
    m_iconCache[".wav"] = iconAudio;
    m_iconCache[".ogg"] = iconAudio;

    // Картинки
    m_iconCache[".jpg"] = iconImage;
    m_iconCache[".jpeg"] = iconImage;
    m_iconCache[".png"] = iconImage;
    m_iconCache[".bmp"] = iconImage;
    m_iconCache[".svg"] = iconImage;

    // Текст и документы
    m_iconCache[".txt"] = iconTxt;
    m_iconCache[".md"]  = iconTxt;
    m_iconCache[".pdf"] = iconPdf;

    // Веб
    m_iconCache[".html"] = iconWeb;
    m_iconCache[".css"]  = iconWeb;
    m_iconCache[".js"]   = iconWeb;
    m_iconCache[".json"] = iconWeb;

    // Архивы
    m_iconCache[".zip"] = iconArchive;
    m_iconCache[".rar"] = iconArchive;
    m_iconCache[".7z"]  = iconArchive;
    m_iconCache[".tar"] = iconArchive;
    m_iconCache[".gz"]  = iconArchive;
}



QIcon FileListModel::getIconForFile(const FileEntry& file) const
{
    // Если это папка - возвращаем папку
    if (file.IsDirectory()) {
        return m_folderIcon;
    }

    // Иначе берем иконку по расширению
    QString extension = QString::fromStdString(file.GetPath().extension().string()).toLower();
    if (m_iconCache.contains(extension)) {
        return m_iconCache[extension]; 
    }

    #ifdef LOG_ENABLED
    // cout<< file.GetPath().extension().string() <<endl;
    qDebug() << QString::fromStdString(file.GetPath().extension().string()) <<"\n";
    #endif
    
    // Если не нашли иконку - возвращаем дефолтную
    return m_fileIcon; 
}