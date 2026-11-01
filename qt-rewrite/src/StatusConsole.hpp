#pragma once

#include <QObject>
#include <QString>

class StatusConsole : public QObject {
    Q_OBJECT

public:
    explicit StatusConsole(QObject* parent = nullptr);

    void log(const QString& message);
    QString currentLog() const;

signals:
    void logUpdated(const QString& text);

private:
    QString m_log;
};
