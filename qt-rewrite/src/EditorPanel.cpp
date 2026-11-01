#include "EditorPanel.hpp"

#include <QFile>
#include <QFileInfo>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QVBoxLayout>

EditorPanel::EditorPanel(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tabs = new QTabWidget(this);
    m_tabs->setTabsClosable(false);
    layout->addWidget(m_tabs);

    auto* editor = new QPlainTextEdit(this);
    editor->setPlaceholderText("Open a file to begin editing.");
    editor->setPlainText("// OpenIDEA Qt rewrite\nint main() {\n    return 0;\n}\n");
    m_tabs->addTab(editor, "main.cpp");
}

void EditorPanel::openFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    const QByteArray contents = file.readAll();
    auto* editor = new QPlainTextEdit(this);
    editor->setPlainText(QString::fromUtf8(contents));
    m_tabs->addTab(editor, QFileInfo(path).fileName());
    m_tabs->setCurrentWidget(editor);
}
