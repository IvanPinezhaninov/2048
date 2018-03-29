#include "game.h"

#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setOrganizationName(QLatin1Literal(ORGANIZATION_NAME));
    app.setApplicationName(QLatin1Literal(APPLICATION_NAME));

    Game game;
    if (!game.launch()) {
        return EXIT_FAILURE;
    }

    return app.exec();
}
