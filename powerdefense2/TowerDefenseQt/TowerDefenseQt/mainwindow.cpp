#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "waypoint.h"
#include "enemy.h"
#include "bullet.h"
#include "audioplayer.h"
#include "plistreader.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtGlobal>
#include <QMessageBox>
#include <QTimer>
#include <QXmlStreamReader>
#include <QtDebug>

static const int Tower1Cost = 300;
static const int Tower2Cost = 500;
static const int Tower1Upgrade = 200;
static const int Tower2Upgrade = 300;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m_waves(0)
    , m_playerHp(100)
	, m_playrGold(1000)
	, m_gameEnded(false)
	, m_gameWin(false)
    , m_towertype(-1)
{
	ui->setupUi(this);

	preLoadWavesInfo();
	loadTowerPositions();
	addWayPoints();

	m_audioPlayer = new AudioPlayer(this);
	m_audioPlayer->startBGM();

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateMap()));
	timer->start(30);

	// 设置300ms后游戏启动
	QTimer::singleShot(300, this, SLOT(gameStart()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::loadTowerPositions()
{
	QFile file(":/config/TowersPosition.plist");
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, "TowerDefense", "Cannot Open TowersPosition.plist");
		return;
	}

	PListReader reader;
	reader.read(&file);

	QList<QVariant> array = reader.data();
	foreach (QVariant dict, array)
	{
		QMap<QString, QVariant> point = dict.toMap();
		int x = point.value("x").toInt();
		int y = point.value("y").toInt();
		m_towerPositionsList.push_back(QPoint(x, y));
	}

	file.close();
}

void MainWindow::paintEvent(QPaintEvent *)
{
	if (m_gameEnded || m_gameWin)
	{
		QString text = m_gameEnded ? "YOU LOST!!!" : "YOU WIN!!!";
		QPainter painter(this);
		painter.setPen(QPen(Qt::red));
		painter.drawText(rect(), Qt::AlignCenter, text);
		return;
	}

	QPixmap cachePix(":/image/Bg.png");
	QPainter cachePainter(&cachePix);

	foreach (const TowerPosition &towerPos, m_towerPositionsList)
		towerPos.draw(&cachePainter);

    foreach (const Tower1 *tower1, m_tower1List)//
        tower1->draw(&cachePainter);
    foreach (const Tower2 *tower2, m_tower2List)//
        tower2->draw(&cachePainter);

	foreach (const WayPoint *wayPoint, m_wayPointsList)
		wayPoint->draw(&cachePainter);

	foreach (const Enemy *enemy, m_enemyList)
		enemy->draw(&cachePainter);

	foreach (const Bullet *bullet, m_bulletList)
		bullet->draw(&cachePainter);

	drawWave(&cachePainter);
	drawHP(&cachePainter);
	drawPlayerGold(&cachePainter);

	QPainter painter(this);
	painter.drawPixmap(0, 0, cachePix);

    QPixmap towermenu(":/image/towermenu.png.jpg");//
    painter.drawPixmap(0,127,towermenu);//
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	QPoint pressPos = event->pos();
//    QPoint buyPos(0,0);
//    QPoint setPos(0,0);
    if(pressPos.x()<88 && pressPos.y()<171 &&pressPos.y()>127){//
//        buyPos = pressPos;
        if(pressPos.x()<44 && canBuyTower1()){//
            settowertype(1);
        }
        else if(pressPos.x()>=44 && canBuyTower2()){//
            settowertype(2);
        }
    }

	auto it = m_towerPositionsList.begin();
	while (it != m_towerPositionsList.end())
	{
        if ((m_towertype!=-1) && it->containPoint(pressPos) && !it->hasTower()){

            m_audioPlayer->playSound(TowerPlaceSound);
            it->setHasTower();
            if(m_towertype==1){//

                m_playrGold -= Tower1Cost;//
                Tower1 *tower1 = new Tower1(it->centerPos(), this);//
                m_tower1List.push_back(tower1);//
                update();
                m_towertype = -1;//
                pressPos.setX(0);//
                pressPos.setY(0);//
                break;
            }
            if(m_towertype==2){//
                m_playrGold -= Tower2Cost;//
                Tower2 *tower2 = new Tower2(it->centerPos(), this);//
                m_tower2List.push_back(tower2);//
                update();
                m_towertype = -1;//
                pressPos.setX(0);//
                pressPos.setY(0);//
                break;
            }
        }
        ++it;
        if(it->containPoint(pressPos) && it->hasTower()){//
            foreach(TowerPosition towerposition, m_towerPositionsList){
                if (pressPos.x()<towerposition.centerPos().x()+towerposition.ms_fixedSize.width()/2
                 && pressPos.x()>towerposition.centerPos().x()-towerposition.ms_fixedSize.width()/2
                 && pressPos.y()<towerposition.centerPos().y()+towerposition.ms_fixedSize.height()/2
                 && pressPos.y()>towerposition.centerPos().y()-towerposition.ms_fixedSize.height()/2){
                     pressPos.setX(towerposition.centerPos().x());
                     pressPos.setY(towerposition.centerPos().y());
                     break;
                }

            }
            foreach(Tower1 *tower1, m_tower1List){//
                if(tower1->getpos()==pressPos && canUpgradeTower1()){
                    tower1->upgrade();
                    break;
                }
            }
            foreach(Tower2 *tower2, m_tower2List){//
                if(tower2->getpos()==pressPos && canUpgradeTower2()){
                    tower2->upgrade();
                    break;
                }
            }
        }
    }


}

bool MainWindow::canBuyTower1() const
{
    if (m_playrGold >= Tower1Cost)
		return true;
	return false;
}
bool MainWindow::canBuyTower2() const
{
    if (m_playrGold >= Tower2Cost)
        return true;
    return false;
}
bool MainWindow::canUpgradeTower1() const
{
    if (m_playrGold >= Tower1Upgrade)
        return true;
    return false;
}
bool MainWindow::canUpgradeTower2() const
{
    if (m_playrGold >= Tower2Upgrade)
        return true;
    return false;
}

void MainWindow::drawWave(QPainter *painter)
{
	painter->setPen(QPen(Qt::red));
	painter->drawText(QRect(400, 5, 100, 25), QString("WAVE : %1").arg(m_waves + 1));
}

void MainWindow::drawHP(QPainter *painter)
{
	painter->setPen(QPen(Qt::red));
	painter->drawText(QRect(30, 5, 100, 25), QString("HP : %1").arg(m_playerHp));
}

void MainWindow::drawPlayerGold(QPainter *painter)
{
	painter->setPen(QPen(Qt::red));
	painter->drawText(QRect(200, 5, 200, 25), QString("GOLD : %1").arg(m_playrGold));
}

void MainWindow::doGameOver()
{
	if (!m_gameEnded)
	{
		m_gameEnded = true;
		// 此处应该切换场景到结束场景
		// 暂时以打印替代,见paintEvent处理
	}
}

void MainWindow::awardGold(int gold)
{
	m_playrGold += gold;
	update();
}

AudioPlayer *MainWindow::audioPlayer() const
{
	return m_audioPlayer;
}

void MainWindow::addWayPoints()
{
	WayPoint *wayPoint1 = new WayPoint(QPoint(420, 285));
	m_wayPointsList.push_back(wayPoint1);

	WayPoint *wayPoint2 = new WayPoint(QPoint(35, 285));
	m_wayPointsList.push_back(wayPoint2);
	wayPoint2->setNextWayPoint(wayPoint1);

	WayPoint *wayPoint3 = new WayPoint(QPoint(35, 195));
	m_wayPointsList.push_back(wayPoint3);
	wayPoint3->setNextWayPoint(wayPoint2);

	WayPoint *wayPoint4 = new WayPoint(QPoint(445, 195));
	m_wayPointsList.push_back(wayPoint4);
	wayPoint4->setNextWayPoint(wayPoint3);

	WayPoint *wayPoint5 = new WayPoint(QPoint(445, 100));
	m_wayPointsList.push_back(wayPoint5);
	wayPoint5->setNextWayPoint(wayPoint4);

	WayPoint *wayPoint6 = new WayPoint(QPoint(35, 100));
	m_wayPointsList.push_back(wayPoint6);
	wayPoint6->setNextWayPoint(wayPoint5);
}

void MainWindow::getHpDamage(int damage/* = 1*/)
{
	m_audioPlayer->playSound(LifeLoseSound);
	m_playerHp -= damage;
	if (m_playerHp <= 0)
		doGameOver();
}

void MainWindow::removedEnemy(Enemy *enemy)
{
	Q_ASSERT(enemy);

	m_enemyList.removeOne(enemy);
	delete enemy;

	if (m_enemyList.empty())
	{
		++m_waves;
		if (!loadWave())
		{
			m_gameWin = true;
			// 游戏胜利转到游戏胜利场景
			// 这里暂时以打印处理
		}
	}
}

void MainWindow::removedBullet(Bullet *bullet)
{
	Q_ASSERT(bullet);

	m_bulletList.removeOne(bullet);
	delete bullet;
}

void MainWindow::addBullet(Bullet *bullet)
{
	Q_ASSERT(bullet);

	m_bulletList.push_back(bullet);
}

void MainWindow::updateMap()
{
	foreach (Enemy *enemy, m_enemyList)
		enemy->move();
	foreach (Tower *tower, m_towersList)
		tower->checkEnemyInRange();
	update();
}

void MainWindow::preLoadWavesInfo()
{
	QFile file(":/config/Waves.plist");
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, "TowerDefense", "Cannot Open TowersPosition.plist");
		return;
	}

	PListReader reader;
	reader.read(&file);

	// 获取波数信息
	m_wavesInfo = reader.data();

	file.close();
}

bool MainWindow::loadWave()
{
	if (m_waves >= m_wavesInfo.size())
		return false;

	WayPoint *startWayPoint = m_wayPointsList.back();
	QList<QVariant> curWavesInfo = m_wavesInfo[m_waves].toList();

    for (int i = 0; i < curWavesInfo.size(); ++i)
	{
		QMap<QString, QVariant> dict = curWavesInfo[i].toMap();
		int spawnTime = dict.value("spawnTime").toInt();

		Enemy *enemy = new Enemy(startWayPoint, this);
		m_enemyList.push_back(enemy);
		QTimer::singleShot(spawnTime, enemy, SLOT(doActivate()));
	}

	return true;
}

QList<Enemy *> MainWindow::enemyList() const
{
	return m_enemyList;
}

void MainWindow::gameStart()
{
	loadWave();
}

void MainWindow::settowertype(int type){
    m_towertype = type;
}
