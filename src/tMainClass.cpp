#include "tMainClass.h"

#include <QFile>
#include <QDateTime>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QThread>

tParametreGlobal  paramGlobal;

tMainClass::tMainClass(char *filename_, QObject *parent)
	: QObject(parent), filename(filename_), isInLive(0), modeAuto(true)
{
	qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Launch Application";

#ifndef QT_DEBUG
	/* Wait DHCP is UP */
	QThread::sleep(10);
#endif

	/* Write Parameter from XML File */
	if (!QString(filename_).isEmpty()) {
			this->writeParamGlobalFromFile(QString(filename_));
		}

	/* Create TcpServer */
	this->myTcpSocket = new tTcpSocket();
	this->myTcpSocket->listenSocket(QHostAddress::Any, 10000);
	connect(this->myTcpSocket, SIGNAL(dataReceived(QByteArray)), this, SLOT(dataReceivedFromTcp(QByteArray)));

	QString program;
	QStringList arguments;

	/*********** CEC HDMI ***********/
	/* Option for tPlayer */
	program = "cec-client";
	arguments.clear();
	arguments << "-o" << "MediaPlayer" << "-d" << "1";

	this->myCEC = new tPlayer(program, arguments, this);
	this->myCEC->play();

	/********** Background **********/
	/* Option for tPlayer */
	program = "pngview";
	arguments.clear();

	this->myBackGround = new tPlayer(program, arguments, this);

	/************ Logo **************/
	/* Option for tPlayer */
	program = "pngview";
	arguments.clear();
	arguments << "-l" << "3" << "-b" << "0x0000";

	this->myLogo = new tPlayer(program, arguments, this);

	/************ Video *************/
	/* Option for tPlayer */
	program = "omxplayer";
	arguments.clear();
	arguments << "-o" << "hdmi" << "-b" << "--no-osd" << "--layer" << "2";

	/* Create Event player */
	this->myEventPlayer = new tPlayer(program, arguments, this);
	connect(this->myEventPlayer, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(playerEvent_finished(int, QProcess::ExitStatus)));
	connect(this->myEventPlayer, SIGNAL(readyReadStandardOutput()), this, SLOT(playerBackground_readStandardOutput()));
	connect(this->myEventPlayer, SIGNAL(readyReadStandardError()), this, SLOT(playerBackground_readStandardError()));
	
	/* Option for tPlayer */
	arguments.clear();
	arguments << "-o" << "hdmi" << "-b" << "--no-osd" << "--layer" << "1" << "--live";

	/* Create Background player */
	this->myBackGroundPlayer = new tPlayer(program, arguments, this);
	connect(this->myBackGroundPlayer, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(playerBackground_finished(int, QProcess::ExitStatus)));
	connect(this->myBackGroundPlayer, SIGNAL(readyReadStandardOutput()), this, SLOT(playerBackground_readStandardOutput()));
	connect(this->myBackGroundPlayer, SIGNAL(readyReadStandardError()), this, SLOT(playerBackground_readStandardError()));

	/************ Picture *************/
	arguments.clear();

	/* Option for tPlayer */
	program = "pngview";
	arguments << "-l" << "2";

	/* Create Event player */
	this->myEventPlayerPicture = new tPlayer(program, arguments, this);
	connect(this->myEventPlayerPicture, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(playerEvent_finished(int, QProcess::ExitStatus)));

	/* Option for tPlayer */
	arguments.clear();
	arguments << "-l" << "1";

	/* Create Background player */
	this->myBackGroundPlayerPicture = new tPlayer(program, arguments, this);
	connect(this->myBackGroundPlayerPicture, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(playerBackground_finished(int, QProcess::ExitStatus)));

	/* Initialize program, Search Corresping Background */
	if (!this->init()) {
			//			quit();
			return;
		}

	/* Create QTimer */
	this->myTimer = new QTimer(this);
	connect(this->myTimer, SIGNAL(timeout()), this, SLOT(timerTick()));
	this->myTimer->start(1000);
}

/****************************************************
***	    	   		~tMainClass							  		    ***
****************************************************/
tMainClass::~tMainClass() {
	qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Quit Application";
	this->myEventPlayer->stop();
	delete this->myEventPlayer;
	this->myBackGroundPlayer->stop();
	delete this->myBackGroundPlayer;
	this->myTimer->stop();
	delete this->myTimer;
}


/****************************************************
***	    	   				quit								  		    ***
****************************************************/
void tMainClass::quit()
{
	this->myEventPlayer->stop();
	this->myEventPlayerPicture->stop();
	this->myBackGroundPlayer->stop();
	this->myBackGroundPlayerPicture->stop();
	this->myTimer->stop();

	// you can do some cleanup here
	// then do emit finished to signal CoreApplication to quit
	emit finished();
}


/****************************************************
***	    	   				init								  		    ***
****************************************************/
int tMainClass::init()
{
	QSettings setting;

	/* Get Current Ip Address */
	QHostAddress addressIp = this->getIpAddress();
	qDebug() << "addressIp: " << addressIp.toString();

	int index=-1, j=0;
	/* Search Corresponding TV from Ip Address */
	for (QList<tTv>::iterator i=paramGlobal.listTv.begin() ; i!=paramGlobal.listTv.end() ; ++i, ++j) {
			QHostAddress addressTv((*i).getUrl());
			qDebug() << "addressTv: " << addressTv.toString();
			if (addressTv == addressIp) {
					index = j;
					break;
				}
		}

	/* Get Current Tv */
	if (index == -1) {
			qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "TV not found";
			return 0;
		}

	/* Play Background */
	QString urlBackground = setting.value("Background").toString();
	urlBackground = tools::mountUrlOnLinux(urlBackground, "", "");
	this->myBackGround->stop();
	if (!urlBackground.isEmpty()) {
			this->myBackGround->setVideo(urlBackground);
			this->myBackGround->play();
		}

	/* Play Logo */
	this->myLogo->stop();
	if (setting.value("Logo/Enable").toBool() == true) {
			QString urlLogo = setting.value("Logo/Url").toString();
			urlLogo = tools::mountUrlOnLinux(urlLogo, "", "");
			this->myLogo->setVideo(urlLogo);
			QStringList arg = this->myLogo->getArguments();
			arg << "-x" << QString("%1").arg(setting.value("Logo/PosX").toInt()) << "-y" << QString("%1").arg(setting.value("Logo/PosY").toInt());
			this->myLogo->setArguments(arg);
			this->myLogo->play();
		}

	this->myCurrentTv = paramGlobal.listTv.at(index);
	qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "tvCurrent: " << this->myCurrentTv.getName();

	return 1;
}


/****************************************************
***	    	   		getIpAddress						  		    ***
****************************************************/
QHostAddress tMainClass::getIpAddress()
{
	QHostAddress address_found;
	foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
			if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
				address_found = address;
		}
	return address_found;
}


/****************************************************
***	    	   dataReceivedFromTcp				  		    ***
****************************************************/
void tMainClass::dataReceivedFromTcp(QByteArray data)
{
	QString firstLine = QString(data).mid(0, QString(data).indexOf("\n"));
	QString cmd = firstLine.mid(0, firstLine.indexOf(":"));
	QString param = firstLine.mid(firstLine.indexOf(":")+2);
	
	qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "TCP => Receive Data: " << data;

	data.remove(0, data.indexOf("\n")+1);

	if (cmd == QString("File")) {
			this->filename = param;
			/** Create and Write File */
			QFile file(param);
			if(!(file.open(QIODevice::WriteOnly)))
				{
					qDebug("File cannot be opened.");
					return;
				}
			file.write(data);
			file.close();
			qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "TCP => Read and write file successfully !";
		}
	else if (cmd == QString("Set")) {
			if (param == QString("Mode")) {
					this->modeAuto = (data == QString("Auto")) ? true : false;
					QByteArray data = "Get: Mode\n";
					data += this->modeAuto ? "Auto\n" : "Manuel\n";
					this->myTcpSocket->writeData(data);
				}
			else if (param == QString("Restart")) {
					/* Stop Old Player */
					this->myEventPlayer->stop();
					this->myEventPlayerPicture->stop();
					this->myBackGroundPlayer->stop();
					this->myBackGroundPlayerPicture->stop();
					/* Write Parameter from XML File */
					this->writeParamGlobalFromFile(this->filename);
					this->init();
				}
			else if (param == QString("Live_Start")) {
					/* Stop Old Player */
					this->myEventPlayer->stop();
					this->myEventPlayerPicture->stop();
					this->myBackGroundPlayer->stop();
					this->myBackGroundPlayerPicture->stop();
					/* Start Live Player */
					this->playLive(QString(data));
				}
			else if (param == QString("Live_Stop")) {
					/* Stop Old Player */
					if (this->isInLive == 1) {
							this->myEventPlayer->stop();
							this->myEventPlayerPicture->stop();
							this->myBackGroundPlayer->stop();
							this->myBackGroundPlayerPicture->stop();
						}
				}
			else if (param == QString("PowerOnHDMI")) {
					/* Envoie la commande CEC d'allumage */
					qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "RUN TV";
					if (this->myCurrentTv.getUseCEC() == true) {
							/* Power ON TV */
							system("echo \"on 0\" | cec-client -s -d 1");
							system(QString(QString("echo \"tx 4F:82:") + QString("%1").arg(this->myCurrentTv.getNumHDMI()) + QString("0:00\" | cec-client -s -d 1")).toStdString().c_str());
						}
					// Power On HDMI
					system("vcgencmd display_power 1");
					this->myCurrentTv.setPowerOn(true);
				}
			else if (param == QString("PowerOffHDMI")) {
					/* Envoie la commande CEC d'extinction */
					qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "STOP TV";
					if (this->myCurrentTv.getUseCEC() == true) {
							/* Power OFF TV */
							system("echo \"standby 0\" | cec-client -s -d 1");
						}
					// Power Off HDMI
					system("vcgencmd display_power 0");
					this->myCurrentTv.setPowerOn(false);
				}
		}
	else if (cmd == QString("Get")) {
			if (param == QString("Mode")) {
					QByteArray data = "Get: Mode\n";
					data += this->modeAuto ? "Auto\n" : "Manuel\n";
					this->myTcpSocket->writeData(data);
				}
			else if (param == QString("Live")) {
					QByteArray data = "Get: Live\n";
					data += this->isInLive ? "1:" : "0:";
					data += this->liveName;
					data += "\n";
					this->myTcpSocket->writeData(data);
				}
		}
}


/****************************************************
***	    	 writeParamGlobalFromFile						    ***
****************************************************/
int tMainClass::writeParamGlobalFromFile(QString filename_)
{
  /** Ouvre le fichier Xml */
  QDomDocument dom("XML");
  QFile xml_doc(filename_);
  if(!xml_doc.open(QIODevice::ReadOnly))
    {
      qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Erreur à l'ouverture du document XML, Impossible d'ouvrir le fichier !";
      return -1;
    }
  if (!dom.setContent(&xml_doc))
    {
      xml_doc.close();
      qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Erreur à l'ouverture du document XML, Le document XML n'a pas pu être attribué à l'objet QDomDocument.";
      return -1;
    }
  xml_doc.close();
  QDomElement dom_element = dom.documentElement();

  /** Efface les anciens parametres */
  paramGlobal.clearAll();

  /** Parse le Xml et rempli les listes */
  paramGlobal.fromXml(dom_element);

  //  qDebug() << paramGlobal.toString();

  qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Read and parse XML File with success !";
  return 0;
}


/****************************************************
***	    	 		playerEvent_finished						    ***
****************************************************/
void tMainClass::playerEvent_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Event Finished: " << exitCode << exitStatus;
	
	/* If its Live, we stop Live when Event is finish */
	if (this->isInLive) {
			this->isInLive = 0;
		}
}


/****************************************************
***	    	 	playerBackground_finished					    ***
****************************************************/
void tMainClass::playerBackground_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Background Finished: " << exitCode << exitStatus;

	/* If its Live, we stop Live when Event is finish */
	if (this->isInLive) {
			this->isInLive = 0;
		}
}

void tMainClass::playerBackground_readStandardOutput()
{
	qDebug() << this->myBackGroundPlayer->readAllStandardOutput();
	qDebug() << this->myEventPlayer->readAllStandardOutput();
}

void tMainClass::playerBackground_readStandardError()
{
	qDebug() << this->myBackGroundPlayer->readAllStandardError();
	qDebug() << this->myEventPlayer->readAllStandardError();
}


/****************************************************
***	            		 timerTick								    ***
****************************************************/
void tMainClass::timerTick()
{

	static tBackground 	background;
	static tEvent				event;

	/* If we are not in mode auto, we stop now */
	if (this->modeAuto == false)
		return;

	/* Check Scheduler */
	checkScheduler(QDateTime::currentDateTime());
	
	if (this->myCurrentTv.getPowerOn() == false)
		return;

	/* If is not in live */
	if (this->isInLive == 0) {
			/* If Background is not running, we search next background */
			if ((this->myBackGroundPlayer->getState() == QProcess::NotRunning) && (this->myBackGroundPlayerPicture->getState() == QProcess::NotRunning)) {
					if (this->searchBackground(&background)) {
							this->playBackground(&background);
						}
				}
			/* If Background is running, we verify it is already the good */
			else if ((this->myBackGroundPlayer->getState() == QProcess::Running) || (this->myBackGroundPlayerPicture->getState() == QProcess::Running)) {
					if (!background.isCorresponding(QDateTime::currentDateTime())) {
							qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Stop Background";
							this->myBackGroundPlayer->stop();
					}
			}
	}

	/* Search if Media must be run */
	if (this->searchEvent(&event)) {
			this->playEvent(&event);
		}

	/* Decrease TimeToClose for timer Event */
	if (this->myEventPlayerPicture->getState() == QProcess::Running) {
			if (timeToClose > 0) {
					timeToClose--;
				}
			else {
					qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Stop Event Picture";
					this->myEventPlayerPicture->stop();
				}
		}
}

/****************************************************
***	            checkScheduler                    ***
****************************************************/
void tMainClass::checkScheduler(QDateTime date_)
{
	/* Cherche le jour courant */
	for (QList<tScheduler>::iterator i=this->myCurrentTv.listScheduler.begin() ; i!=this->myCurrentTv.listScheduler.end() ; ++i) {
			if ((*i).getDate().toString("dddd") == date_.date().toString("dddd")) {
					// Si la TV est eteinte
					if (this->myCurrentTv.getPowerOn() == false) {
							/* Verifie l'heure d'ouverture */
							if (((*i).getOpeningTime() <= date_.time()) && ((*i).getClosingTime() > date_.time())) {
									/* Envoie la commande CEC d'allumage */
									qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "RUN TV";
									if (this->myCurrentTv.getUseCEC() == true) {
											/* Power ON TV */
											this->myCEC->write(QString("on 0\n"));
											this->myCEC->write(QString(QString("tx 4F:82:") + QString("%1").arg(this->myCurrentTv.getNumHDMI()) + QString("0:00\n")));
											qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Send Power On and set HDMI" << this->myCurrentTv.getNumHDMI();
											//system("echo \"on 0\" | cec-client -s -d 1");
											//system(QString(QString("echo \"tx 4F:82:") + QString("%1").arg(this->myCurrentTv.getNumHDMI()) + QString("0:00\" | cec-client -s -d 1")).toStdString().c_str());
										}
									// Power On HDMI
									system("vcgencmd display_power 1");
									this->myCurrentTv.setPowerOn(true);
								}
						}
					// Si la TV est allumer
					else {
							/* Verifie l'heure de fermeture */
							if ((*i).getClosingTime() <= date_.time()) {
									/* Envoie la commande CEC d'extinction */
									qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "STOP TV";
									if (this->myCurrentTv.getUseCEC() == true) {
											/* Power OFF TV */
											this->myCEC->write(QString("standby 0\n"));
											qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Send Power Off";
											//system("echo \"standby 0\" | cec-client -s -d 1");
										}
									// Power Off HDMI
									system("vcgencmd display_power 0");
									this->myCurrentTv.setPowerOn(false);
								}
						}
				}
		}
}

/****************************************************
***	            playBackground                    ***
****************************************************/
void tMainClass::playBackground(tBackground* background_)
{
  int j=0;
  int index=-1;

  /* Search Corresponding Media */
  for (QList<tMedia>::iterator i=paramGlobal.listMedia.begin() ; i!=paramGlobal.listMedia.end() ; ++i, ++j) {
      if ((*i).getName() == background_->getName()) {
          index = j;
          break;
        }
    }

  /* Get Current Media */
  if (index == -1) {
      qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Media not found";
      return ;
    }

  tMedia media = paramGlobal.listMedia.at(index);
  if (media.getType() == QString("Photo")) {
      this->myBackGroundPlayerPicture->stop();
      this->myBackGroundPlayerPicture->setVideo(media.getUrl());
      this->myBackGroundPlayerPicture->play();
      qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Play Background Picture:" << media.getUrl();
    }
  else {
      this->myBackGroundPlayer->stop();
      this->myBackGroundPlayer->setVideo(media.getUrl());
      this->myBackGroundPlayer->play();
      qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Play Background Video:" << media.getUrl();
    }

}


/****************************************************
***	           searchBackground		  						  ***
****************************************************/
int tMainClass::searchBackground(tBackground* background_)
{
	//	qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Search Background";

	int j=0;
	int index=-1;
	
	/* Search BackGround Task */
	for (QList<tBackground>::iterator i=this->myCurrentTv.listBackground.begin() ; i!=this->myCurrentTv.listBackground.end() ; ++i, ++j) {
			if ((*i).isCorresponding(QDateTime::currentDateTime())) {
					index = j;
					break;
				}
		}

	/* Get Current Background */
	if (index == -1) {
			this->myBackGroundPlayer->stop();
			//		qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Background not found";
			return 0;
		}

	*background_ = this->myCurrentTv.listBackground.at(index);

	qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "BackgroundCurrent:" << background_->getName();

	return 1;
}

/****************************************************
***	            		playEvent									    ***
****************************************************/
void tMainClass::playEvent(tEvent* event_)
{
	int j=0;
	int index=-1;
	
	/* Search Corresponding Media */
	for (QList<tMedia>::iterator i=paramGlobal.listMedia.begin() ; i!=paramGlobal.listMedia.end() ; ++i, ++j) {
			if ((*i).getName() == event_->getName()) {
					index = j;
					break;
				}
		}

	/* Get Current Media */
	if (index == -1) {
			qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Media not found";
			return ;
		}

	tMedia media = paramGlobal.listMedia.at(index);

	if (media.getType() == QString("Photo")) {
			/* Start Next Media */
			this->myEventPlayerPicture->stop();
			this->myEventPlayerPicture->setVideo(media.getUrl());
			this->myEventPlayerPicture->play();
			this->timeToClose = QTime(0, 0, 0).secsTo(media.getTime());
			qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Play Event Picture:" << media.getUrl() << ", during (s) :" << this->timeToClose;
		}
	else {
			/* Start Next Media */
			this->myEventPlayer->stop();
			this->myEventPlayer->setVideo(media.getUrl());
			this->myEventPlayer->play();
			qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Play Event Video:" << media.getUrl();
		}
}


/****************************************************
***	             searchEvent			  			  		  ***
****************************************************/
int tMainClass::searchEvent(tEvent* event_)
{
	int j=0;
	int index=-1;
	
	/* Search next Event */
	for (QList<tEvent>::iterator i=this->myCurrentTv.listEvent.begin() ; i!=this->myCurrentTv.listEvent.end() ; ++i, ++j) {
			if ((*i).isCorresponding(QDateTime::currentDateTime())) {
					index = j;
					break;
				}
		}

	/* Get Current Event */
	if (index == -1) {
			return 0;
		}

	/* If its the same Event */
	if (*event_ == this->myCurrentTv.listEvent.at(index)) {
			return 0;
		}

	*event_ = this->myCurrentTv.listEvent.at(index);

	qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "New Event: " << event_->getName();
	
	return 1;
}


/****************************************************
***	            		playLive									    ***
****************************************************/
void tMainClass::playLive(QString name_)
{

	int j=0;
	int index=-1;
	
	/* Search Corresponding Media */
	for (QList<tMedia>::iterator i=paramGlobal.listMedia.begin() ; i!=paramGlobal.listMedia.end() ; ++i, ++j) {
			if ((*i).getName() == name_) {
					index = j;
					break;
				}
		}

	/* Get Current Media */
	if (index == -1) {
			qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Media not found";
			return ;
		}

	tMedia media = paramGlobal.listMedia.at(index);

	if (media.getType() == QString("Photo")) {
			this->myBackGroundPlayerPicture->stop();
			this->myBackGroundPlayerPicture->setVideo(media.getUrl());
			this->myBackGroundPlayerPicture->play();
			qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Play Live Picture:" << media.getUrl();
		}
	else {
			this->myBackGroundPlayer->stop();
			this->myBackGroundPlayer->setVideo(media.getUrl());
			this->myBackGroundPlayer->play();
			qDebug()	<< QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Play Live Video:" << media.getUrl();
		}

	this->isInLive = 1;
	this->liveName = name_;
}
