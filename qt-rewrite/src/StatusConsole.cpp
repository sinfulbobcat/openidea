#include "StatusConsole.hpp"

StatusConsole::StatusConsole(QObject* parent)
    : QObject(parent)
{
}

void StatusConsole::log(const QString& message)
{
    m_log += message;
    emit logUpdated(m_log);
}

QString StatusConsole::currentLog() const
{
    return m_log;
}
