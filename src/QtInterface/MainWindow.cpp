#include "MainWindow.h"

#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QStyleHints>
#include <QGuiApplication>



MainWindow::MainWindow(AppCore& core,QWidget* parent) :
    QMainWindow(parent),
    m_core(core),
    m_model(nullptr),
    m_sideBarModel(nullptr),
    m_backBtn(nullptr),
    m_forwardBtn(nullptr),
    m_menuBtn(nullptr)
    // m_starBtn(nullptr)
{
    m_showHiddenFiles = false;
    m_currentIconSize = 100;
    m_isDark = isSystemThemeDark();

    setUI();
    setupActions();

    connect(m_backBtn, &QPushButton::clicked, this, &MainWindow::onBackClicked);
    connect(m_addressBar, &QLineEdit::returnPressed,this, &MainWindow::onAddressReturnPressed);
    connect(m_fileView, &QListView::doubleClicked, this, &MainWindow::onFileDoubleClicked);
    connect(m_sideBar, &QListView::clicked, this, &MainWindow::goToDirectory);
    connect(m_fileView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);
    connect(m_fileView, &QListView::customContextMenuRequested, this, &MainWindow::onContextMenuRequested);

}

