#pragma once

#include <QDialog>
#include <vector>
#include <string>
#include "Types.h"

class QListWidget;
class QPushButton;
class QLabel;

/// @brief класс диалога выбора приложения для открытия файла
/// @param m_filePath путь к файлу
/// @param m_apps вектор приложений
/// @param m_selectedAppIndex индекс выбранного приложения
/// @param m_appList виджет отбразающий список приложений
/// @param m_openBtn кнопка открытия
/// @param m_cancelBtn кнопка отмены
class OpenWithDialog : public QDialog
{
    Q_OBJECT

public:

    explicit OpenWithDialog(const std::string& filePath, const std::vector<AppInfo>& apps, QWidget *parent = nullptr);

    /// @brief Возвращает готовую команду для запуска приложения на примере: "/usr/bin/gimp '/home/user/image.png'"
    std::string getFinalCommand() const;

private:
    std::string m_filePath;
    std::vector<AppInfo> m_apps;
    
    int m_selectedAppIndex = -1;          //Индекс выбранного приложения в векторе m_apps

    QListWidget* m_appList;
    QPushButton* m_openBtn;
    QPushButton* m_cancelBtn;

    void setupUI();
    void setupConnections();
    
    /// @brief Возвращает готовую команду для запуска приложения
    /// @param rawCmd имя приложения
    /// @param path путь к файлу
    /// @return команда в виде строки
    QString processExecCommand(const std::string& rawCmd, const std::string& path);
};