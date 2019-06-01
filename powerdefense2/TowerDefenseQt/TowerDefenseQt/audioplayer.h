#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>//信号和槽的类

class QMediaPlayer;//QT写好的类，可以播放影音

enum SoundType              //枚举
{
	TowerPlaceSound,		// 放塔时的声音0
	LifeLoseSound,			// 基地费血时的声音1
	LaserShootSound,		// 打中敌人时的生意2
	EnemyDestorySound		// 敌人升天时的声音3
};

class AudioPlayer : public QObject
{
public:
	explicit AudioPlayer(QObject *parent = 0);//explicit防止构造函数隐式转换

	void startBGM();
	void playSound(SoundType soundType);

private:
	QMediaPlayer *m_backgroundMusic; // 只用来播放背景音乐
};


#endif // AUDIOPLAYER_H
