#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>

class NativeBridge final : public QObject {
    Q_OBJECT

public:
    explicit NativeBridge(QObject* parent = nullptr);

    Q_INVOKABLE void invoke(const QString& requestId, const QString& command, const QVariantMap& args);

signals:
    void response(const QString& requestId, const QVariantMap& result);
};