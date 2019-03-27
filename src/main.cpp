#include "tMainClass.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>

#include <QtCore>
#include <QtDebug>

#define LOCK_FILE		"/var/lock/Gestion_Tv_Fitness.lock"
#define LOG_FILE		"/var/log/Gestion_Tv_Fitness.log"

const QString logFilePath = "debug.log";
#ifdef QT_DEBUG
  bool logToFile = false;
#else
  bool logToFile = false;
#endif

QCoreApplication *app;
tMainClass *mainClass;

void customMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  Q_UNUSED(context)

  QHash<QtMsgType, QString> msgLevelHash({{QtDebugMsg, "Debug"}, {QtInfoMsg, "Info"}, {QtWarningMsg, "Warning"}, {QtCriticalMsg, "Critical"}, {QtFatalMsg, "Fatal"}});
  QByteArray localMsg = msg.toLocal8Bit();
  QString formattedTime = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
  QString logLevelName = msgLevelHash[type];

  if (logToFile) {
    QString txt = QString("%1 >> %2: %3").arg(formattedTime, logLevelName, msg);
    QFile outFile(logFilePath);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
    outFile.close();
  } else {
    fprintf(stderr, "%s\n", localMsg.constData());
    fflush(stderr);
  }

  if (type == QtFatalMsg)
    abort();
}

/** Handle caught signals **/
void signal_handler(int signum)
{
  syslog(LOG_NOTICE, "Signal %d caught. Exiting", signum);
	printf("Signal receive. Exiting...\n");
  /* Exit */
	mainClass->quit();
	delete mainClass;
	app->quit();
	delete app;
  exit(0);
}

/** Daemonify the process
  */
void daemonify()
{    
	qDebug() << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << " => " << "Daemonify";
	
  pid_t pid, sid; 
	char str[10];
	
	if(getppid()==1)
		return; /* already a daemon */

  /* Fork off the parent process */       
  pid = fork();
  if(pid < 0){
    syslog(LOG_ERR, "Unable to fork daemon process. Exiting...");
    exit(EXIT_FAILURE);
  }
  /* If we got a good PID, then we can exit the parent process. */
  if(pid > 0) exit(EXIT_SUCCESS);
  /* Change the file mode mask */
  umask(0);  
  /* Create a new SID for the child process */
  sid = setsid();
  if(sid < 0){
    syslog(LOG_ERR, "Unable to set SID. Exiting...");
    exit(EXIT_FAILURE);
  }
  /* Change the current working directory */
  if((chdir("/home/pi/")) < 0){
    syslog(LOG_ERR, "Unable to change working directory to /. Exiting...");
    exit(EXIT_FAILURE);
  }
  /* Create lock file */
  int lfp = open(LOCK_FILE, O_RDWR|O_CREAT, 0640);
  if(lfp < 0){
    syslog(LOG_ERR, "Unable to create lock file. Exiting...");
    exit(EXIT_FAILURE);
  }
  if(lockf(lfp, F_TLOCK, 0) < 0){
    syslog(LOG_ERR, "An instance is already running. Exiting...");
    exit(EXIT_FAILURE);
  }
	/* first instance continues */
	sprintf(str,"%d\n",getpid());
	write(lfp,str,strlen(str)); /* record pid to lockfile */

  /* Close out the standard file descriptors */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
	//freopen(LOG_FILE, "w", stdout);	// redirect printf to LOG_FILE
  //close(STDERR_FILENO);
	freopen(LOG_FILE, "w", stderr); // redirect qDebug() to LOG_FILE
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Error, must take 1 or more arguments\n");
				printf("Ex: %s media.xml\n", argv[0]);
		return 0;
	}

  /* Connect signal handler with signals */
  signal(SIGINT, signal_handler);
  signal(SIGHUP, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGKILL, signal_handler);
	
  /* Daemonify */
  if (QString(argv[3]) == "--daemon")
		daemonify();

	app = new QCoreApplication(argc, argv);

	qInstallMessageHandler(customMessageOutput); // custom message handler for debugging
	qInfo() << "Lancement du programme";

	QCoreApplication::setOrganizationName("TVController");
	QCoreApplication::setApplicationName("TV Controleur");

	mainClass = new tMainClass(argv[1], app);

	// connect up the signals
  QObject::connect(mainClass, SIGNAL(finished()), app, SLOT(quit()));

	return app->exec();
}

