#include "Tower.h"
#include <QPixmap>
#include <QVector>
#include <QPointF>
#include <QGraphicsEllipseItem>
#include "Bullet.h"
#include <QPointF>
#include <QLineF>
#include "game.h"
#include <QTimer>
#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include "Enemy.h"
#include <QDebug>
#include <QPen>
extern game * game;

Tower::Tower(QGraphicsItem *parent, double attack_radius)
    :QObject(), QGraphicsPixmapItem(parent),cen_pos(50,50){
    setPixmap(QPixmap(":/mushroom.jpg"));
    attack_area = new QGraphicsEllipseItem(cen_pos.x(),cen_pos.y(),300,300,this);
//    attack_area = new QGraphicsEllipseItem()


    QPen pen = QPen(Qt::DotLine);
    pen.setWidth(5);
    attack_area->setPen(pen);
    attack_area->setPos(cen_pos.x()+0.5*this->pixmap().width(),cen_pos.y()+0.5*this->pixmap().height());

    QTimer * timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(fire()));
    timer->start(100);

    target_position = QPointF(0,0);
}

bool Tower::Judge_fire(){
    // get a list of all enemies that collide with attack_area, find the closest one
    // and set it's position as the attack_dest

    // get a list of all enemies within attack_area
    QList<QGraphicsItem *> inrange_items = attack_area->collidingItems();

    //find the closest enemy
    double closest_dist = 300;
//    QPointF target_position(0,0);
    for (size_t i = 0, n = inrange_items.size(); i < n; ++i){

        // make sure it is an enemy
        Enemy * enemy = dynamic_cast<Enemy *>(inrange_items[i]);

        // see if distance is closer
        if (enemy){
            double this_dist = DistanceToEnemy(inrange_items[i]);
            if (this_dist < closest_dist){
                closest_dist = this_dist;
                target_enemy = dynamic_cast<Enemy *>(inrange_items[i]);
                return true;
            }
        }
    }
    return false;
}
double Tower::DistanceToEnemy(QGraphicsItem *item){
    QLineF distline(item->pos(),cen_pos);
    return distline.length();
}
QPointF Tower::Enemy_posToAttacked_pt(Enemy *target){

}
void Tower::RotateTower(QPointF dest){

}
void Tower::fire(){

}
