#include "AppSettings.hpp"

#include <QByteArray>
#include <QString>
#include <QStringList>

AppSettings::AppSettings()
    : m_settings("OpenIDEA", "QtRewrite")
{
}

void AppSettings::setTheme(const QString& themeName)
{
    m_settings.setValue("theme", themeName);
}

QString AppSettings::theme() const
{
    return m_settings.value("theme", "Fusion").toString();
}

void AppSettings::setLastProject(const QString& path)
{
    m_settings.setValue("lastProject", path);
}

QString AppSettings::lastProject() const
{
    return m_settings.value("lastProject", QString()).toString();
}

void AppSettings::setWindowGeometry(const QByteArray& geometry)
{
    m_settings.setValue("windowGeometry", geometry);
}

QByteArray AppSettings::windowGeometry() const
{
    return m_settings.value("windowGeometry", QByteArray()).toByteArray();
}

void AppSettings::setRecentProjects(const QStringList& paths)
{
    m_settings.setValue("recentProjects", paths);
}

QStringList AppSettings::recentProjects() const
{
    return m_settings.value("recentProjects", QStringList()).toStringList();
}
