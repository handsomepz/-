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

static const int TowerCost = 300;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m_waves(0)
	, m_playerHp(5)
	, m_playrGold(1000)
	, m_gameEnded(false)
	, m_gameWin(false)
    , whether(0,0)
    , startposition(0,0)
    , lastposition(0,0)
    , position(0,320)
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
    QTimer::singleShot(300, this, SLOT(start()));

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

    if(page==0)
    {
        QPixmap startpicture(":/image/startpicture.png");
        QPainter painter(this);
        painter.drawPixmap(0,0,startpicture);
    }
    else if(page==1)
    {
        QPixmap cachePix(":/image/map1.png");
        QPainter cachePainter(&cachePix);

        if(startposition.x()>=0&&startposition.x()<=50&&startposition.y()>=320&&startposition.y()<=370)
        {
            QPixmap tt(":/image/tower.png");
            cachePainter.drawPixmap(position,tt);
        }

        foreach (const TowerPosition &towerPos, m_towerPositionsList)
            towerPos.draw(&cachePainter);

        foreach (const Tower *tower, m_towersList)
            tower->draw(&cachePainter);

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
        QPixmap tt(":/image/tower.png");
        painter.drawPixmap(0,320,tt);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    position=event->pos();
    this->update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint pressPos = event->pos();
    if(startposition.x()>=0&&startposition.x()<=50&&startposition.y()>=320&&startposition.y()<=370)
    {
        auto it = m_towerPositionsList.begin();
        while (it != m_towerPositionsList.end())
        {
            if (canBuyTower() && it->containPoint(pressPos) && !it->hasTower())
            {
                m_audioPlayer->playSound(TowerPlaceSound);
                m_playrGold -= TowerCost;
                it->setHasTower();

                Tower *tower = new Tower(it->centerPos(), this);
                m_towersList.push_back(tower);
                update();
                break;
            }

            ++it;
        }
    }

    QPoint reset(0,0),reset1(0,320);
    whether=reset;
    position=reset1;
    startposition=reset;
    this->update();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(page==0)
    {
        QPoint pressPos = event->pos();
        if(pressPos.x()>=220&&pressPos.x()<=660&&pressPos.y()>=110&&pressPos.y()<=280)
        {
            page++;
            this->gameStart();
            this->update();
        }
    }
    else if(page==1)
    {
        QPoint pressPos = event->pos();
        startposition=pressPos;
    }
}

bool MainWindow::canBuyTower() const
{
	if (m_playrGold >= TowerCost)
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

void MainWindow::start()
{

}
