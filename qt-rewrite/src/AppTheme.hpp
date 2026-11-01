#pragma once

#include <QString>

class QApplication;

class AppTheme {
public:
    enum class Mode {
        Light,
        Dark,
    };

    static void apply(QApplication& app);
    static Mode detect();

private:
    static bool isDarkThemeHint(const QString& value);
};
