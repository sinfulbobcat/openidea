#include "AppTheme.hpp"
#include "MainWindow.hpp"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QWebEngineUrlScheme>

namespace {

void registerOpenideaScheme()
{
    QWebEngineUrlScheme scheme("openidea");
    scheme.setSyntax(QWebEngineUrlScheme::Syntax::HostAndPort);
    scheme.setDefaultPort(443);
    scheme.setFlags(QWebEngineUrlScheme::SecureScheme);
    QWebEngineUrlScheme::registerScheme(scheme);
}

QString findWebRoot()
{
    const QString configuredRoot = qEnvironmentVariable("OPENIDEA_WEB_ROOT");
    const QStringList candidates = {
        configuredRoot,
        QDir(QCoreApplication::applicationDirPath()).filePath("webui"),
        QDir::current().filePath("dist"),
        QDir(QCoreApplication::applicationDirPath()).filePath("../../dist"),
    };

    for (const QString& candidate : candidates) {
        if (!candidate.isEmpty() && QFileInfo::exists(QDir(candidate).filePath("index.html"))) {
            return QDir(candidate).absolutePath();
        }
    }
    return {};
}

} // namespace

int main(int argc, char** argv)
{
    registerOpenideaScheme();
    QApplication app(argc, argv);
    app.setOrganizationName("OpenIDEA");
    app.setApplicationName("OpenIDEA");
    app.setApplicationVersion(QStringLiteral(OPENIDEA_VERSION));

    AppTheme::apply(app);

    MainWindow window(findWebRoot());
    window.resize(1400, 900);
    window.show();

    return app.exec();
}
