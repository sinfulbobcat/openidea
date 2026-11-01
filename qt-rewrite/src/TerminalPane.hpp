#pragma once

#include <QWidget>

class QPlainTextEdit;

class TerminalPane : public QWidget {
    Q_OBJECT

public:
    explicit TerminalPane(QWidget* parent = nullptr);
    void appendOutput(const QString& text);

private:
    QPlainTextEdit* m_output = nullptr;
};
