#include "tPlayer.h"

#include <qdebug.h>

tPlayer::tPlayer(QString program_, QStringList arguments_, QObject *parent)
    :	QObject(parent), program(program_), arguments(arguments_)
{
	isPause = false;
	this->myProcess = new QProcess(this);
	this->filename.clear();

	// connect up the signals
	connect(this->myProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SIGNAL(finished(int, QProcess::ExitStatus)));
	connect(this->myProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SIGNAL(stateChanged(QProcess::ProcessState)));
	connect(this->myProcess, SIGNAL(readyReadStandardError()), this, SIGNAL(readyReadStandardError()));
	connect(this->myProcess, SIGNAL(readyReadStandardOutput()), this, SIGNAL(readyReadStandardOutput()));
}

tPlayer::~tPlayer() {
	this->stop();
	delete this->myProcess;
}

void tPlayer::play() {
	QStringList arg(this->arguments);
	arg << this->filename;

	/* If not video already play */
	if ((this->myProcess->state() == QProcess::NotRunning) && (isPause == false)) {
		this->myProcess->start(this->program, arg);
		if (!this->myProcess->waitForStarted(3000)) {
				qDebug() << "tPlayer => Error, task not started !";
			}
	} else if (isPause == true) {
		this->myProcess->write("p");
		this->myProcess->closeWriteChannel();
		isPause	= false;
	}
}

void tPlayer::pause() {
	if ((this->myProcess->state() != QProcess::NotRunning) && (isPause == false)) {
		this->myProcess->write("p");
		this->myProcess->closeWriteChannel();
		isPause = true;
	}
}

void tPlayer::stop() {
	if (this->myProcess->state() != QProcess::NotRunning) {
			qDebug() << "tPlayer => Stop";
		this->myProcess->write("q");
		this->myProcess->closeWriteChannel();
		this->myProcess->close();
		isPause	= false;
	}
}

void tPlayer::write(QString data_)
{
	if (this->myProcess->state() != QProcess::NotRunning) {
			this->myProcess->write(data_.toStdString().c_str());
		}
}
