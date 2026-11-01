#include "BuildRunner.hpp"

#include <QProcess>
#include <QStringList>

BuildRunner::BuildRunner(QObject* parent)
    : QObject(parent)
{
    m_process = new QProcess(this);
    connect(m_process, &QProcess::readyReadStandardOutput, this, [this]() {
        emit outputReady(QString::fromLocal8Bit(m_process->readAllStandardOutput()));
    });
    connect(m_process, &QProcess::readyReadStandardError, this, [this]() {
        emit outputReady(QString::fromLocal8Bit(m_process->readAllStandardError()));
    });
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, [this](int code, QProcess::ExitStatus) {
            emit finished(code);
        });
}

void BuildRunner::runBuild(const QString& projectRoot, const QString& buildDir)
{
    const QString fullBuildDir = projectRoot + QStringLiteral("/") + buildDir;
    m_process->setWorkingDirectory(projectRoot);
    m_process->start("cmake", QStringList{"-S", ".", "-B", fullBuildDir});
    if (!m_process->waitForStarted()) {
        emit outputReady("Failed to start build process.\n");
        emit finished(-1);
        return;
    }

    if (!m_process->waitForFinished()) {
        emit outputReady("Build process did not finish cleanly.\n");
        emit finished(-1);
        return;
    }

    m_process->start("cmake", QStringList{"--build", fullBuildDir, "-j2"});
    if (!m_process->waitForStarted()) {
        emit outputReady("Failed to start compiler phase.\n");
        emit finished(-1);
        return;
    }

    if (!m_process->waitForFinished()) {
        emit outputReady("Compiler phase did not finish cleanly.\n");
        emit finished(-1);
        return;
    }

    emit outputReady("Build finished successfully.\n");
    emit finished(m_process->exitCode());
}
