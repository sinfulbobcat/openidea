#pragma once

#include <QWidget>

class QPlainTextEdit;
class QTabWidget;

class EditorPanel : public QWidget {
    Q_OBJECT

public:
    explicit EditorPanel(QWidget* parent = nullptr);
    void openFile(const QString& path);

private:
    QTabWidget* m_tabs = nullptr;
};
