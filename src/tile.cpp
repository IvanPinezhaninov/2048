#include "tile.h"

#include <QQmlComponent>
#include <QQuickItem>

static const char *const X_PROPERTY_NAME = "x";
static const char *const Y_PROPERTY_NAME = "y";
static const char *const Z_PROPERTY_NAME = "z";
static const char *const WIDTH_PROPERTY_NAME = "width";
static const char *const HEIGHT_PROPERTY_NAME = "height";
static const char *const VALUE_PROPERTY_NAME = "value";
static const char *const MOVE_ANIMATION_ENABLED_PROPERTY_NAME = "moveAnimationEnabled";


Tile::Tile(QQmlComponent *tileQmlComponent, QQuickItem *parent) :
    QObject(parent),
    m_tileQuickItem(qobject_cast<QQuickItem*>(tileQmlComponent->create())),
    m_value(0)
{
    m_tileQuickItem->setParentItem(parent);
    connect(m_tileQuickItem.get(), SIGNAL(moveAnimationFinished()), this, SIGNAL(moveAnimationFinished()));
}


Tile::~Tile()
{
}


qreal Tile::x() const
{
    return m_tileQuickItem->x();
}


qreal Tile::y() const
{
    return m_tileQuickItem->y();
}


qreal Tile::z() const
{
    return m_tileQuickItem->z();
}


qreal Tile::width() const
{
    return m_tileQuickItem->width();
}


qreal Tile::height() const
{
    return m_tileQuickItem->height();
}


QRectF Tile::geometry() const
{
    return QRectF(x(), y(), width(), height());
}


int Tile::value() const
{
    return m_value;
}


int Tile::displayValue() const
{
    return m_tileQuickItem->property(VALUE_PROPERTY_NAME).toInt();
}


bool Tile::isMoveAnimationEnabled() const
{
    return m_tileQuickItem->property(MOVE_ANIMATION_ENABLED_PROPERTY_NAME).toBool();
}


void Tile::setX(qreal x)
{
    m_tileQuickItem->setProperty(X_PROPERTY_NAME, x);
}


void Tile::setY(qreal y)
{
    m_tileQuickItem->setProperty(Y_PROPERTY_NAME, y);
}


void Tile::setZ(qreal z)
{
    m_tileQuickItem->setProperty(Z_PROPERTY_NAME, z);
}


void Tile::setWidth(qreal width)
{
    m_tileQuickItem->setProperty(WIDTH_PROPERTY_NAME, width);
}


void Tile::setHeight(qreal height)
{
    m_tileQuickItem->setProperty(HEIGHT_PROPERTY_NAME, height);
}


void Tile::setGeometry(const QRectF &rect)
{
    setGeometry(rect.x(), rect.y(), rect.width(), rect.height());
}


void Tile::setGeometry(qreal x, qreal y, qreal width, qreal height)
{
    setX(x);
    setY(y);
    setWidth(width);
    setHeight(height);
}


void Tile::setValue(int value)
{
    m_value = value;
}


void Tile::setDisplayValue(int value)
{
    m_tileQuickItem->setProperty(VALUE_PROPERTY_NAME, value);
}


void Tile::setMoveAnimationEnabled(bool enabled)
{
    m_tileQuickItem->setProperty(MOVE_ANIMATION_ENABLED_PROPERTY_NAME, enabled);
}
