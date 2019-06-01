#include "audioplayer.h"
#include <QDir>//该类提供对目录结构及其内容的访问
#include <QMediaPlayer>//该类可以播放影音
#include <QMediaPlaylist>//该类提供要播放的媒体内容列表

// 为了解决mac下声音无法输出,总之发现使用绝对路径可以完成目标,蛋疼,因此以此种方式暂时处理
static const QString s_curDir = QDir::currentPath() + "/";//返回当前目录绝对路径
                                                          //Returns the absolute path of the application's current directory.

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)//继承 基类1名（初始化参数表），基类2名（初始化参数表）
    , m_backgroundMusic(NULL)//类的组合AudioPlayer的私有成员为QMediaPlayer类的一个指针m_backgroundMusic
{
	// 创建一直播放的背景音乐
    //Url:Uniform Resoure Locator统一资源定位符,通俗点就是网址
	QUrl backgroundMusicUrl = QUrl::fromLocalFile(s_curDir + "music/8bitDungeonLevel.mp3");
    //fromLocalFile是QUrl类的函数Return a QUrl representation of localFile,interpreted as a local file.
    //返回网址并转成本地文件，可接受/分隔符，可指示远程文件
    if (QFile::exists(backgroundMusicUrl.toLocalFile()))//exists判断文件是否存在,toLocalFile返回本地文件格式的路径
	{
        m_backgroundMusic = new QMediaPlayer(this);//创建一个播放器
        QMediaPlaylist *backgroundMusicList = new QMediaPlaylist();//创建一个播放列表

        QMediaContent media(backgroundMusicUrl);//Constructs a media content with url providing a reference to the content.
        backgroundMusicList->addMedia(media);//Append the media content to the playlist.bool型，添加成功返回1，不过这里没用返回值
        backgroundMusicList->setCurrentIndex(0);//在播放列表中激活指定媒体内容
		// 设置背景音乐循环播放
        backgroundMusicList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);//setPlaybackMode设置播放模式
                                                                                //CurrentItemInLoop为QMediaPlaylist类的一个枚举类型，值为1
        m_backgroundMusic->setPlaylist(backgroundMusicList);//setPlaylist is a public slot???应该是把创建的播放列表设置为要播放的
	}
}

void AudioPlayer::startBGM()
{
    if (m_backgroundMusic)// QMediaPlayer类的对象有值？？？
		m_backgroundMusic->play();
}

void AudioPlayer::playSound(SoundType soundType)
{
    static const QUrl mediasUrls[] =                              //创建保存各种声音网址的数组
	{
        QUrl::fromLocalFile(s_curDir + "music/tower_place.wav"),  //wav是无损音质的格式，MP3为了缩小空间会破坏音质
		QUrl::fromLocalFile(s_curDir + "music/life_lose.wav"),
		QUrl::fromLocalFile(s_curDir + "music/laser_shoot.wav"),
		QUrl::fromLocalFile(s_curDir + "music/enemy_destroy.wav")
	};
	static QMediaPlayer player;

    if (QFile::exists(mediasUrls[soundType].toLocalFile()))//是不是将在线的网址转化成本地文件网址？转化之后判断是否存在文件
	{
		player.setMedia(mediasUrls[soundType]);
		player.play();
	}
}
//怎么实现事件与音乐绑定的？怎么判断什么时候播放什么声音？
