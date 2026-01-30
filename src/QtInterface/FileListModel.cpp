#include "FileListModel.h"

#include <filesystem>
#include <QDebug>
#include <QGuiApplication>
#include <QPalette>
#include <QtConcurrent/QtConcurrent>
#include <QImageReader>

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    setupIcons();
}

void FileListModel::updateData(const std::vector<FileEntry>& newFiles)
{
    clearCache();

    beginResetModel();

    m_files = newFiles;

    endResetModel();
}

const FileEntry& FileListModel::getFile(int row) const 
{
    return m_files[row];
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
    // if (role == Qt::DecorationRole) {
    //     return getIconForFile(file);
    // }

    if (role == Qt::DecorationRole) {
        if (file.IsDirectory()) return getIconForFile(file);

        QString filePath = QString::fromStdString(file.GetPath().string());
        QString ext = QString::fromStdString(file.GetPath().extension().string()).toLower();

        bool isImage = (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".svg");

        if (isImage) {
            if (m_thumbnailCache.contains(filePath)) {          //  если в кэше есть картинка возвращаем ее
                return m_thumbnailCache[filePath];
            }

            if (!m_loadingPaths.contains(filePath)) {           //  если в кэше нет картинки - говорим загрузизь в кеш
                loadThumbnailAsync(filePath, index.row());
            }
            
            return getIconForFile(file);            //на момент загрузки картинки возвращаем дефолтную
        }

        return getIconForFile(file);              // если не картинка
    }

    // роль TextAlignmentRole какое выравнивание будет у текста
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter; // Текст по центру
    }

    return QVariant();
}

void FileListModel::setupIcons()
{
    m_iconCache.clear();

    QStringList extensions = {
        ".mp4", ".avi", ".mkv", ".mov",
        ".mp3", ".wav", ".ogg",
        ".jpg", ".jpeg", ".png", ".bmp", ".svg",
        ".txt", ".md", ".pdf",
        ".html", ".css", ".js", ".json",
        ".zip", ".rar", ".7z", ".tar", ".gz"
    };

    for (const QString& ext : extensions) {
        std::string dummyName = "dummy" + ext.toStdString();
        
        QString iconPath = getIconPath(dummyName, false); // false = это не папка
        
        m_iconCache[ext] = QIcon(iconPath);
    }

    m_folderIcon = QIcon(getIconPath("", true)); 
    
    m_fileIcon = QIcon(getIconPath("file.unknown_extension", false));

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
    // qDebug() << QString::fromStdString(file.GetPath().extension().string()) <<"\n";
    #endif
    
    // Если не нашли иконку - возвращаем дефолтную
    return m_fileIcon; 
}




QString FileListModel::getIconPath(const std::string& filename, bool isDir)
{
    QColor textColor = QGuiApplication::palette().color(QPalette::WindowText);
    bool isDark = textColor.lightness() > 128;

    if (isDir) {
        return ":/res/folder.svg";
    }

    std::filesystem::path path(filename);
    QString ext = QString::fromStdString(path.extension().string()).toLower();

    if (isDark) {// Темная тема
        if (ext == ".zip" || ext == ".rar" || ext == ".7z" || ext == ".tar" || ext == ".gz") return ":/res/archivse_dark.svg";
        if (ext == ".mp3" || ext == ".wav" || ext == ".ogg") return ":/res/audios_dark.svg";
        if (ext == ".txt" || ext == ".md") return ":/res/txt_dark.svg";
        if (ext == ".mp4" || ext == ".avi" || ext == ".mkv" || ext == ".mov") return ":/res/videos_dark.svg";
        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".svg") return ":/res/images_dark.svg";
        if (ext == ".pdf") return ":/res/pdf_dark.svg";
        if (ext == ".html" || ext == ".css" || ext == ".js" || ext == ".json") return ":/res/web_txt_dark.svg";
        return ":/res/file_dark.svg";
    } 
    else {// Светлая тема
        if (ext == ".zip" || ext == ".rar" || ext == ".7z" || ext == ".tar" || ext == ".gz") return ":/res/archivse.svg";
        if (ext == ".mp3" || ext == ".wav" || ext == ".ogg") return ":/res/audios.svg";
        if (ext == ".txt" || ext == ".md") return ":/res/txt.svg";
        if (ext == ".mp4" || ext == ".avi" || ext == ".mkv" || ext == ".mov") return ":/res/videos.svg";
        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".svg") return ":/res/images.svg";
        if (ext == ".pdf") return ":/res/pdf.svg";
        if (ext == ".html" || ext == ".css" || ext == ".js" || ext == ".json") return ":/res/web_txt.svg";
        return ":/res/file.svg";
    }
}


void FileListModel::refresh()
{
    emit layoutChanged();
}



void FileListModel::clearCache()
{
    m_thumbnailCache.clear();
    m_loadingPaths.clear();
}





void FileListModel::loadThumbnailAsync(const QString& path, int row) const
{

    m_loadingPaths.insert(path);

    QFuture<QIcon> future = QtConcurrent::run([path]() -> QIcon 
    {
        //Выполняется в отдельном потоке
        
        QImageReader reader(path);

        reader.setScaledSize(QSize(128, 128));


        QImage image = reader.read();
        if(image.isNull())
        {
            return QIcon();
        }

        return QIcon(QPixmap::fromImage(image));
    });


    QFutureWatcher<QIcon>* watcher = new QFutureWatcher<QIcon>();

    connect(watcher, &QFutureWatcher<QIcon>::finished, [=](){
        //код в главном потоке

        QIcon result = watcher->result();

        if(!result.isNull())
        {
            m_thumbnailCache[path] = result;
        }

        m_loadingPaths.remove(path);

        if (row < m_files.size()) { 
            QModelIndex idx = this->index(row, 0);
            const_cast<FileListModel*>(this)->dataChanged(idx, idx, {Qt::DecorationRole});
        }

        watcher->deleteLater();
    });


    watcher->setFuture(future);

}
