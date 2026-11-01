#pragma once

#include <QSettings>
#include <QString>
#include <QStringList>

class AppSettings {
public:
    AppSettings();

    void setTheme(const QString& themeName);
    QString theme() const;

    void setLastProject(const QString& path);
    QString lastProject() const;

    void setWindowGeometry(const QByteArray& geometry);
    QByteArray windowGeometry() const;

    void setRecentProjects(const QStringList& paths);
    QStringList recentProjects() const;

private:
    QSettings m_settings;
};
