#include "TerminalPane.hpp"

#include <QPlainTextEdit>
#include <QVBoxLayout>

TerminalPane::TerminalPane(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_output = new QPlainTextEdit(this);
    m_output->setReadOnly(true);
    m_output->setPlainText("$ openidea\nReady\n");
    layout->addWidget(m_output);
}

void TerminalPane::appendOutput(const QString& text)
{
    if (m_output) {
        m_output->appendPlainText(text);
    }
}
