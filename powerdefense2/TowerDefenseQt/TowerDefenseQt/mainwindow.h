#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "towerposition.h"
#include "tower.h"
#include "tower1.h"
#include "tower2.h"


namespace Ui {
class MainWindow;
}

class WayPoint;
class Enemy;
class Bullet;
class AudioPlayer;

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void getHpDamage(int damage = 1);
	void removedEnemy(Enemy *enemy);
	void removedBullet(Bullet *bullet);
	void addBullet(Bullet *bullet);
	void awardGold(int gold);
    void settowertype(int type);

	AudioPlayer* audioPlayer() const;
	QList<Enemy *> enemyList() const;

protected:
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *);

private slots:
    void updateMap();
    void gameStart();

private:
    void loadTowerPositions();
    void addWayPoints();
    bool loadWave();
    bool canBuyTower1() const;//
    bool canBuyTower2() const;//
    bool canUpgradeTower1() const;//
    bool canUpgradeTower2() const;//
	void drawWave(QPainter *painter);
	void drawHP(QPainter *painter);
	void drawPlayerGold(QPainter *painter);
	void doGameOver();
	void preLoadWavesInfo();


private:
	Ui::MainWindow *		ui;
	int						m_waves;
	int						m_playerHp;
	int						m_playrGold;
    int                     m_towertype;
	bool					m_gameEnded;
	bool					m_gameWin;
	AudioPlayer *			m_audioPlayer;
	QList<QVariant>			m_wavesInfo;
	QList<TowerPosition>	m_towerPositionsList;
    QList<Tower1 *>			m_tower1List;
    QList<Tower2 *>         m_tower2List;
	QList<WayPoint *>		m_wayPointsList;
	QList<Enemy *>			m_enemyList;
	QList<Bullet *>			m_bulletList;
};

#endif // MAINWINDOW_H
