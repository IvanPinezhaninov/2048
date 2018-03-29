#ifndef CELL_H
#define CELL_H

#include <QObject>

#include <memory>

QT_BEGIN_NAMESPACE
class QQuickItem;
QT_END_NAMESPACE

class Tile;


class Cell final : public QObject
{
    Q_OBJECT
public:
    explicit Cell(QQuickItem *cellQuickItem, QObject *parent = nullptr);
    ~Cell();

    std::shared_ptr<Tile> tile() const;

public slots:
    void setTile(const std::shared_ptr<Tile> &tile);

private slots:
    void onXChanged();
    void onYChanged();
    void onWidthChanged();
    void onHeightChanged();

private:
    Q_DISABLE_COPY(Cell)

    QQuickItem *const m_cellQuickItem;
    std::weak_ptr<Tile> m_tile;
};

#endif // CELL_H
