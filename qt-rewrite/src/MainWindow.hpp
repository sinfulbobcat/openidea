#pragma once

#include <QMainWindow>
#include <QString>

class AppSettings;
class NativeBridge;
class QWebChannel;
class QWebEngineView;
class ReactAppSchemeHandler;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QString webRoot, QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void restoreSettings();
    void saveSettings();

    AppSettings* m_settings = nullptr;
    QWebChannel* m_webChannel = nullptr;
    NativeBridge* m_nativeBridge = nullptr;
    QWebEngineView* m_webView = nullptr;
    ReactAppSchemeHandler* m_schemeHandler = nullptr;
};
