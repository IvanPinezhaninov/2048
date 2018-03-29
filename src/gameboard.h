#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QObject>

#include <memory>

QT_BEGIN_NAMESPACE
class QQuickItem;
QT_END_NAMESPACE

class Cell;
class GameboardPrivate;

class Gameboard final : public QObject
{
    Q_OBJECT
public:
    explicit Gameboard(QQuickItem *gameboardQuickItem, QObject *parent = nullptr);
    ~Gameboard();

    QQuickItem *tilesParent() const;

    int rows() const;
    int columns() const;

    QMap<int, std::shared_ptr<Cell>> cells() const;

public slots:
    void setRows(int rows);
    void setColumns(int columns);

private:
    Q_DISABLE_COPY(Gameboard)

    const std::unique_ptr<GameboardPrivate> d;
};

#endif // GAMEBOARD_H
