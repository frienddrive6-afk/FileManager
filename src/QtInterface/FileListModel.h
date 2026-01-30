#pragma once

#include <QAbstractListModel>
#include <vector>
#include <QIcon>
#include <QHash>
#include <QMap>
#include <QSet>
#include <QFutureWatcher> // Для потоков

#include "FileEntry.h"
#include "NavigationState.h"

/// @brief класс модели списка файлов для отображения в m_fileView
/// @param m_files массив FileEntry с данными о файлах
/// @param m_folderIcon иконка папки
/// @param m_fileIcon иконка файла
/// @param m_iconCache словарь для хранения иконок
class FileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FileListModel(QObject *parent = nullptr);

    /// @brief Принимает массив FileEntry и обновляет список
    /// @param newFiles массив FileEntry с новыми данными
    void updateData(const std::vector<FileEntry>& newFiles);

    /// @brief возвращает FileEntry по индексу
    /// @param row индекс
    const FileEntry& getFile(int row) const;

    /// @brief Вспомогательный метод для заполнения словаря
    void setupIcons(); 

    /// @brief Вспомогательный метод для получения пути к иконке
    static QString getIconPath(const std::string& filename, bool isDir);

    /// @brief Принудительно обновляет модель
    void refresh();

    // --- переопределенные методы ---
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    std::vector<FileEntry> m_files;

    QIcon m_folderIcon;
    QIcon m_fileIcon;

    QHash<QString, QIcon> m_iconCache;
    
    /// @brief возвращает иконку для указанного FileEntry
    /// @param file нужен для извленияня разширения файла
    QIcon getIconForFile(const FileEntry& file) const;




    /// @brief Кэш иконок
    mutable QMap<QString, QIcon> m_thumbnailCache;

    /// @brief Список файлов которые прямо сейчас грузятся 
    mutable QSet<QString> m_loadingPaths;

    /// @brief Вспомогательный метод для запуска потока
    void loadThumbnailAsync(const QString& path, int row) const;
    
    /// @brief Очистка кэша при смене папки
    void clearCache();

};