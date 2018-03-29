#ifndef TILE_H
#define TILE_H

#include <QObject>

#include <memory>

QT_BEGIN_NAMESPACE
class QQmlComponent;
class QQuickItem;
QT_END_NAMESPACE


class Tile final : public QObject
{
    Q_OBJECT
public:
    explicit Tile(QQmlComponent *tileQmlComponent, QQuickItem *parent);
    ~Tile();

    qreal x() const;
    qreal y() const;
    qreal z() const;

    qreal width() const;
    qreal height() const;
    QRectF geometry() const;

    int value() const;
    int displayValue() const;
    bool isMoveAnimationEnabled() const;

  signals:
    void moveAnimationFinished();

public slots:
    void setX(qreal x);
    void setY(qreal y);
    void setZ(qreal z);

    void setWidth(qreal width);
    void setHeight(qreal height);
    void setGeometry(const QRectF &rect);
    void setGeometry(qreal x, qreal y, qreal width, qreal height);

    void setValue(int value);
    void setDisplayValue(int value);

    void setMoveAnimationEnabled(bool enabled);

private:
    Q_DISABLE_COPY(Tile)

    const std::unique_ptr<QQuickItem> m_tileQuickItem;
    int m_value;
};

#endif // TILE_H
