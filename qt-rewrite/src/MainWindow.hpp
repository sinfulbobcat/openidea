#pragma once

#include <QMainWindow>

class QDockWidget;
class QPlainTextEdit;
class QSplitter;
class QStatusBar;
class ProjectExplorer;
class EditorPanel;
class AppSettings;
class BuildRunner;
class TerminalPane;
class StatusConsole;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void buildLayout();
    void buildMenuBar();
    void buildStatusBar();
    void restoreSettings();
    void saveSettings();

    QSplitter* m_splitter = nullptr;
    ProjectExplorer* m_projectExplorer = nullptr;
    EditorPanel* m_editorPanel = nullptr;
    QDockWidget* m_outputDock = nullptr;
    QStatusBar* m_statusBar = nullptr;
    AppSettings* m_settings = nullptr;
    BuildRunner* m_buildRunner = nullptr;
    TerminalPane* m_terminalPane = nullptr;
    StatusConsole* m_statusConsole = nullptr;
};
