#include "game.h"

#include <QGuiApplication>

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    app.setOrganizationName(QLatin1Literal(ORGANIZATION_NAME));
    app.setApplicationName(QLatin1Literal(APPLICATION_NAME));

    Game game;
    if (!game.launch()) {
        return EXIT_FAILURE;
    }

    return app.exec();
}
