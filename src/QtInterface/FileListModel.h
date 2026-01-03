#pragma once

#include <QAbstractListModel>
#include <vector>
#include <QIcon>
#include <QHash>

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

    /// @brief Вспомогательный метод для заполнения словаря
    void setupIcons(); 
};