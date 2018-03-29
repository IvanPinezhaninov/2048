#ifndef GAME_H
#define GAME_H

#include <QObject>

#include <memory>

class GamePrivate;


class Game final : public QObject
{
    Q_OBJECT
public:
    explicit Game(QObject *parent = nullptr);
    ~Game();

    bool launch();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void onRootObjectCreated(QObject *object, const QUrl &url);
    void onContinueGameRequested();
    void onRestartGameRequested();
    void onTileMoveAnimationFinished();

private:
    Q_DISABLE_COPY(Game)

    const std::unique_ptr<GamePrivate> d;

    friend class GamePrivate;
};

#endif // GAME_H
