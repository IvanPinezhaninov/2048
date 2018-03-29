#include "cell.h"

#include "tile.h"

#include <QQuickItem>


Cell::Cell(QQuickItem *cellQuickItem, QObject *parent) :
    QObject(parent),
    m_cellQuickItem(cellQuickItem)
{
    Q_ASSERT(m_cellQuickItem != nullptr);

    connect(m_cellQuickItem, &QQuickItem::xChanged, this, &Cell::onXChanged);
    connect(m_cellQuickItem, &QQuickItem::yChanged, this, &Cell::onYChanged);
    connect(m_cellQuickItem, &QQuickItem::widthChanged, this, &Cell::onWidthChanged);
    connect(m_cellQuickItem, &QQuickItem::heightChanged, this, &Cell::onHeightChanged);
}


Cell::~Cell()
{
}


std::shared_ptr<Tile> Cell::tile() const
{
    return m_tile.lock();
}


void Cell::setTile(const std::shared_ptr<Tile> &tile)
{
    Q_ASSERT(m_cellQuickItem != nullptr);

    m_tile = tile;

    if (auto tile = m_tile.lock()) {
        tile->setGeometry(m_cellQuickItem->x(), m_cellQuickItem->y(),
                          m_cellQuickItem->width(), m_cellQuickItem->height());
    }
}


void Cell::onXChanged()
{
    Q_ASSERT(m_cellQuickItem);

    if (auto tile = m_tile.lock()) {
        tile->setX(m_cellQuickItem->x());
    }
}


void Cell::onYChanged()
{
    Q_ASSERT(m_cellQuickItem);

    if (auto tile = m_tile.lock()) {
        tile->setY(m_cellQuickItem->y());
    }
}


void Cell::onWidthChanged()
{
    Q_ASSERT(m_cellQuickItem);

    if (auto tile = m_tile.lock()) {
        tile->setWidth(m_cellQuickItem->width());
    }
}


void Cell::onHeightChanged()
{
    Q_ASSERT(m_cellQuickItem);

    if (auto tile = m_tile.lock()) {
        tile->setHeight(m_cellQuickItem->height());
    }
}
