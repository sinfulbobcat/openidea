#include "AppTheme.hpp"

#include <QApplication>
#include <QPalette>
#include <QStyleHints>
#include <QStyleFactory>
#include <QString>
#include <QVariant>

namespace {

bool isDarkThemeHintValue(const QString& value)
{
    const QString normalized = value.toLower();
    return normalized.contains("dark") || normalized.contains("night") || normalized.contains("black") || normalized.contains("adwaita-dark");
}

bool envHasDarkHint(const char* key)
{
    const QString value = qEnvironmentVariable(key);
    return !value.isEmpty() && isDarkThemeHintValue(value);
}

} // namespace

bool AppTheme::isDarkThemeHint(const QString& value)
{
    return isDarkThemeHintValue(value);
}

AppTheme::Mode AppTheme::detect()
{
    if (envHasDarkHint("GTK_THEME") || envHasDarkHint("GDK_THEME") || envHasDarkHint("COLOR_SCHEME") ||
        envHasDarkHint("XDG_CURRENT_DESKTOP") || envHasDarkHint("DESKTOP_SESSION")) {
        return Mode::Dark;
    }

    return Mode::Light;
}

void AppTheme::apply(QApplication& app)
{
    app.setStyle(QStyleFactory::create("Fusion"));

    const auto mode = detect();
    QPalette palette;

    if (mode == Mode::Dark) {
        palette.setColor(QPalette::Window, QColor(0x1d, 0x1d, 0x1f));
        palette.setColor(QPalette::WindowText, QColor(0xf5, 0xf5, 0xf5));
        palette.setColor(QPalette::Base, QColor(0x28, 0x28, 0x2c));
        palette.setColor(QPalette::AlternateBase, QColor(0x3a, 0x3a, 0x3d));
        palette.setColor(QPalette::ToolTipBase, QColor(0x1f, 0x1f, 0x1f));
        palette.setColor(QPalette::ToolTipText, QColor(0xff, 0xff, 0xff));
        palette.setColor(QPalette::Text, QColor(0xf1, 0xf1, 0xf1));
        palette.setColor(QPalette::Button, QColor(0x35, 0x35, 0x39));
        palette.setColor(QPalette::ButtonText, QColor(0xf5, 0xf5, 0xf5));
        palette.setColor(QPalette::BrightText, QColor(0xff, 0xff, 0xff));
        palette.setColor(QPalette::Highlight, QColor(0x5b, 0x9d, 0xff));
        palette.setColor(QPalette::HighlightedText, QColor(0x00, 0x00, 0x00));
    } else {
        palette.setColor(QPalette::Window, QColor(0xf3, 0xf3, 0xf3));
        palette.setColor(QPalette::WindowText, QColor(0x11, 0x11, 0x11));
        palette.setColor(QPalette::Base, QColor(0xff, 0xff, 0xff));
        palette.setColor(QPalette::AlternateBase, QColor(0xee, 0xee, 0xee));
        palette.setColor(QPalette::ToolTipBase, QColor(0xff, 0xff, 0xff));
        palette.setColor(QPalette::ToolTipText, QColor(0x11, 0x11, 0x11));
        palette.setColor(QPalette::Text, QColor(0x11, 0x11, 0x11));
        palette.setColor(QPalette::Button, QColor(0xe5, 0xe5, 0xe5));
        palette.setColor(QPalette::ButtonText, QColor(0x11, 0x11, 0x11));
        palette.setColor(QPalette::BrightText, QColor(0xff, 0xff, 0xff));
        palette.setColor(QPalette::Highlight, QColor(0x4a, 0x90, 0xff));
        palette.setColor(QPalette::HighlightedText, QColor(0xff, 0xff, 0xff));
    }

    palette.setColor(QPalette::Link, QColor(0x66, 0x99, 0xff));
    palette.setColor(QPalette::LinkVisited, QColor(0x99, 0x88, 0xff));
    app.setPalette(palette);

    app.setStyleSheet(
        "QMainWindow, QDockWidget, QWidget { border: 0; }"
        "QTreeWidget, QTextEdit, QListWidget { background: palette(base); color: palette(text); }"
        "QStatusBar { background: palette(window); color: palette(text); }"
    );
}
