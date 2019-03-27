#ifndef TPLAYER_H
#define TPLAYER_H

#include <QProcess>

class tPlayer : public QObject
{
	Q_OBJECT

	public:
		tPlayer(QString program_, QStringList arguments_, QObject *parent=0);
		virtual ~tPlayer();

		/* Set Video */
		void setVideo(QString filename_) { this->filename = filename_; }
		
		/* Get Video */
		QString getVideo() { return this->filename; }

		/* Set Arguments */
		void setArguments(QStringList arguments_) { this->arguments = arguments_; }

		/* Get Arguments */
		QStringList getArguments() { return this->arguments; }

		/* Play Video */
		void play();

		/* Pause Video */
		void pause();
		
		/* Stop Video */
		void stop();

		/* Write data */
		void write(QString data_);

		/* Get State of Process */
		QProcess::ProcessState getState() { return this->myProcess->state(); }

		/* Get Output and Error */
		QByteArray readAllStandardError() { return this->myProcess->readAllStandardError(); }
		QByteArray readAllStandardOutput() { return this->myProcess->readAllStandardOutput(); }

	protected:

	public slots:
		void slot_finished(int exitCode, QProcess::ExitStatus exitStatus) { emit finished(exitCode, exitStatus); }

	signals:
		void finished(int exitCode, QProcess::ExitStatus exitStatus);
		void stateChanged(QProcess::ProcessState newState);
		void readyReadStandardError();
		void readyReadStandardOutput();

	private:
		bool					isPause;
		QProcess 			*myProcess;
		QString 			program;
		QStringList 	arguments;
		QString				filename;
};

#endif // TPLAYER
