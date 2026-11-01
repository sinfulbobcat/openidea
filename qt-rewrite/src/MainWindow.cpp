#include "MainWindow.hpp"

#include "AppSettings.hpp"
#include "BuildRunner.hpp"
#include "EditorPanel.hpp"
#include "ProjectExplorer.hpp"
#include "StatusConsole.hpp"
#include "TerminalPane.hpp"

#include <QAction>
#include <QCloseEvent>
#include <QDockWidget>
#include <QDir>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_settings(new AppSettings())
    , m_buildRunner(new BuildRunner(this))
    , m_statusConsole(new StatusConsole(this))
{
    setWindowTitle("OpenIDEA Qt Rewrite");
    setObjectName("openideaMainWindow");
    buildMenuBar();
    buildLayout();
    buildStatusBar();
    restoreSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete m_settings;
}

void MainWindow::buildMenuBar()
{
    auto* fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Open Project");
    fileMenu->addAction("Open File");
    fileMenu->addSeparator();
    fileMenu->addAction("Quit");

    auto* viewMenu = menuBar()->addMenu("View");
    viewMenu->addAction("Project");
    viewMenu->addAction("Output");

    auto* toolsMenu = menuBar()->addMenu("Tools");
    auto* buildAction = toolsMenu->addAction("Build");
    connect(buildAction, &QAction::triggered, this, [this]() {
        if (m_projectExplorer) {
            const QString projectRoot = m_projectExplorer->property("rootPath").toString();
            if (!projectRoot.isEmpty()) {
                auto* outputEditor = qobject_cast<QPlainTextEdit*>(m_outputDock->widget());
                if (outputEditor) {
                    outputEditor->appendPlainText("Starting build for " + projectRoot + "\n");
                }
                connect(m_buildRunner, &BuildRunner::outputReady, this, [outputEditor](const QString& text) {
                    if (outputEditor) {
                        outputEditor->appendPlainText(text);
                    }
                }, Qt::UniqueConnection);
                m_buildRunner->runBuild(projectRoot);
            }
        }
    });

    toolsMenu->addAction("Run");
}

void MainWindow::buildStatusBar()
{
    m_statusBar = statusBar();
    m_statusBar->showMessage("Ready");
}

void MainWindow::buildLayout()
{
    auto* centralWidget = new QWidget(this);
    auto* layout = new QHBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    m_splitter = new QSplitter(Qt::Horizontal, this);

    m_projectExplorer = new ProjectExplorer(this);
    const QString projectRoot = m_settings->lastProject().isEmpty() ? QDir::homePath() : m_settings->lastProject();
    m_projectExplorer->setRootPath(projectRoot);

    m_editorPanel = new EditorPanel(this);

    m_splitter->addWidget(m_projectExplorer);
    m_splitter->addWidget(m_editorPanel);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setSizes({220, 1180});

    m_outputDock = new QDockWidget("Output", this);
    auto* outputEditor = new QPlainTextEdit(this);
    outputEditor->setReadOnly(true);
    outputEditor->setPlainText("Build output will appear here.\n");
    m_outputDock->setWidget(outputEditor);
    addDockWidget(Qt::BottomDockWidgetArea, m_outputDock);

    m_terminalPane = new TerminalPane(this);
    auto* terminalDock = new QDockWidget("Terminal", this);
    terminalDock->setWidget(m_terminalPane);
    addDockWidget(Qt::BottomDockWidgetArea, terminalDock);

    layout->addWidget(m_splitter);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::restoreSettings()
{
    const QByteArray geometry = m_settings->windowGeometry();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }

    const QString lastProject = m_settings->lastProject();
    if (!lastProject.isEmpty()) {
        m_projectExplorer->setRootPath(lastProject);
    }
}

void MainWindow::saveSettings()
{
    m_settings->setWindowGeometry(saveGeometry());
    if (m_projectExplorer) {
        m_settings->setLastProject(m_projectExplorer->property("rootPath").toString());
    }
}
