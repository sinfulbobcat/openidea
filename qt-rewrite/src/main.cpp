#include "AppTheme.hpp"
#include "MainWindow.hpp"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setOrganizationName("OpenIDEA");
    app.setApplicationName("OpenIDEA Qt Rewrite");

    AppTheme::apply(app);

    MainWindow window;
    window.resize(1400, 900);
    window.show();

    return app.exec();
}
