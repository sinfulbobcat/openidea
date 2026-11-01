#include "ReactAppSchemeHandler.hpp"

#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QUrl>
#include <QWebEngineUrlRequestJob>

ReactAppSchemeHandler::ReactAppSchemeHandler(QString root, QObject* parent)
    : QWebEngineUrlSchemeHandler(parent)
    , m_root(QDir(std::move(root)).canonicalPath())
{
}

void ReactAppSchemeHandler::requestStarted(QWebEngineUrlRequestJob* job)
{
    QString relativePath = QUrl::fromPercentEncoding(job->requestUrl().path().toUtf8());
    while (relativePath.startsWith('/')) {
        relativePath.remove(0, 1);
    }
    if (relativePath.isEmpty() || relativePath.endsWith('/')) {
        relativePath += "index.html";
    }

    QString filePath = QDir(m_root).absoluteFilePath(relativePath);
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() && QFileInfo(relativePath).suffix().isEmpty()) {
        filePath = QDir(m_root).filePath("index.html");
        fileInfo.setFile(filePath);
    }

    const QString canonicalPath = fileInfo.canonicalFilePath();
    const QString relativeCanonicalPath = QDir(m_root).relativeFilePath(canonicalPath);
    if (canonicalPath.isEmpty() || relativeCanonicalPath == ".." || relativeCanonicalPath.startsWith("../") ||
        !fileInfo.isFile()) {
        job->fail(QWebEngineUrlRequestJob::UrlNotFound);
        return;
    }

    QFile file(canonicalPath);
    if (!file.open(QIODevice::ReadOnly)) {
        job->fail(QWebEngineUrlRequestJob::RequestFailed);
        return;
    }

    QByteArray contents = file.readAll();
    if (QFileInfo(canonicalPath).fileName() == "index.html") {
        const QByteArray bootstrap = R"(<script src="qrc:///qtwebchannel/qwebchannel.js"></script>
<script>window.openideaNativeReady = new Promise((resolve, reject) => {
  if (!window.qt || !window.qt.webChannelTransport) { reject(new Error("Qt WebChannel is unavailable")); return; }
  new QWebChannel(window.qt.webChannelTransport, channel => resolve(channel.objects.nativeBridge));
});</script>)";
        contents.replace("</head>", bootstrap + "</head>");
    }

    auto* response = new QBuffer(job);
    response->setData(contents);
    response->open(QIODevice::ReadOnly);
    const QByteArray mimeType = QMimeDatabase().mimeTypeForFile(canonicalPath).name().toUtf8();
    job->reply(mimeType, response);
}