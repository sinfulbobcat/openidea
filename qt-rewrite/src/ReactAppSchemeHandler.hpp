#pragma once

#include <QWebEngineUrlSchemeHandler>

class ReactAppSchemeHandler final : public QWebEngineUrlSchemeHandler {
    Q_OBJECT

public:
    explicit ReactAppSchemeHandler(QString root, QObject* parent = nullptr);

    void requestStarted(QWebEngineUrlRequestJob* job) override;

private:
    QString m_root;
};