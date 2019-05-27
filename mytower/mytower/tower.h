#ifndef TOWER_H
#define TOWER_H
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QPointF>
#include <QObject>
#include "enemy.h"

class Tower:public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Tower(QGraphicsItem * parent=0, double attack_radius);
    bool Judge_fire();//是否攻击
    double DistanceToEnemy(QGraphicsItem* item);
    QPointF Enemy_posToAttacked_pt(Enemy* target);
    void RotateTower(QPointF dest);
public slots:
    void fire();
private:
    QPointF cen_pos;//塔中心坐标
    double attack_radius;
//    double width,length;//塔图片
    QGraphicsEllipseItem * attack_area;//攻击范围
    Enemy * target_enemy;
    QPointF target_position;
};

#endif // TOWER_H
