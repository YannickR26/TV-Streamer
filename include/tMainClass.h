#ifndef TMAINCLASS_H
#define TMAINCLASS_H

#include "tPlayer.h"
#include "tParametreGlobal.h"
#include "tTcpSocket.h"

#include <QTimer>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QHostAddress>

class tMainClass : public QObject
{
	Q_OBJECT

  public:
		tMainClass(char *filename_, QObject *parent=0);
		virtual ~tMainClass();
		void 					quit();

	protected:
		int 					init();
		QHostAddress 	getIpAddress();
		int 					writeParamGlobalFromFile(QString filename_);
		void					checkScheduler(QDateTime date_);
		void 					playBackground(tBackground* background_);
		int						searchBackground(tBackground* background_);
		void 					playEvent(tEvent* event_);
		int			 			searchEvent(tEvent* event_);
		void					playLive(QString name_);

	public slots:
		void 					playerEvent_finished(int exitCode, QProcess::ExitStatus exitStatus);
		void 					playerBackground_finished(int exitCode, QProcess::ExitStatus exitStatus);
		void					playerBackground_readStandardOutput();
		void					playerBackground_readStandardError();
		void 					dataReceivedFromTcp(QByteArray data);
		void 					timerTick();

	signals:
		void 					finished();

	private:
		tTcpSocket		*myTcpSocket;
		tPlayer				*myCEC;
		tPlayer				*myBackGround, *myLogo;
		tPlayer 			*myBackGroundPlayer, *myEventPlayer;
		tPlayer 			*myBackGroundPlayerPicture, *myEventPlayerPicture;
		QTimer 				*myTimer;
		qint64				timeToClose;
		tTv 					myCurrentTv;
		tMedia 				myCurrentMediaEvent, myCurrentMediaBackground;
		QString				filename;
		int						isInLive;
		QString				liveName;
		bool					modeAuto;
};

#endif // TMAINCLASS_H
