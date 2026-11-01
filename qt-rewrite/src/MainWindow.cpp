#include "MainWindow.hpp"

#include "AppSettings.hpp"
#include "NativeBridge.hpp"
#include "ReactAppSchemeHandler.hpp"

#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <QUrl>

MainWindow::MainWindow(QString webRoot, QWidget* parent)
    : QMainWindow(parent)
    , m_settings(new AppSettings())
{
    setWindowTitle("OpenIDEA");
    setObjectName("openideaMainWindow");
    m_webView = new QWebEngineView(this);
    m_schemeHandler = new ReactAppSchemeHandler(std::move(webRoot), m_webView);
    m_webView->page()->profile()->installUrlSchemeHandler("openidea", m_schemeHandler);
    m_webChannel = new QWebChannel(this);
    m_nativeBridge = new NativeBridge(this);
    m_webChannel->registerObject("nativeBridge", m_nativeBridge);
    m_webView->page()->setWebChannel(m_webChannel);
    setCentralWidget(m_webView);
    m_webView->load(QUrl("openidea://app/"));
    restoreSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete m_settings;
}

void MainWindow::restoreSettings()
{
    const QByteArray geometry = m_settings->windowGeometry();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }

}

void MainWindow::saveSettings()
{
    m_settings->setWindowGeometry(saveGeometry());
}
