#pragma once

#include <QObject>
#include <QString>

class QProcess;

class BuildRunner : public QObject {
    Q_OBJECT

public:
    explicit BuildRunner(QObject* parent = nullptr);

public slots:
    void runBuild(const QString& projectRoot, const QString& buildDir = "build");

signals:
    void outputReady(const QString& text);
    void finished(int exitCode);

private:
    QProcess* m_process = nullptr;
};
