#include "tTcpSocket.h"
#include <QDataStream>
#include <QDebug>

static inline qint32 ArrayToInt(QByteArray source);
static inline QByteArray IntToArray(qint32 source);

tTcpSocket::tTcpSocket(QObject *parent) :
  QObject(parent)
{
  this->socket = NULL;
  this->server = NULL;
}

bool tTcpSocket::connectToHost(const QHostAddress &addr, quint16 port)
{
	this->socket = new QTcpSocket(this);

	connect(this->socket, SIGNAL(connected()), this, SLOT(connected()));
	connect(this->socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

	QByteArray *buffer = new QByteArray();
	qint32 *s = new qint32(0);
	buffers.insert(socket, buffer);
	sizes.insert(socket, s);

//  qDebug() << "Connecting...";

  // this is not blocking call
	this->socket->connectToHost(addr, port);

	return this->socket->waitForConnected();
}

void tTcpSocket::listenSocket(const QHostAddress &addr, quint16 port)
{
  this->server = new QTcpServer(this);

	connect(this->server, SIGNAL(newConnection()), this, SLOT(newConnection()));

	this->server->listen(addr, port);

//	qDebug() << "Listening...";
}

void tTcpSocket::newConnection()
{
	while (this->server->hasPendingConnections())
	{
		this->socket = server->nextPendingConnection();
		connect(this->socket, SIGNAL(readyRead()), SLOT(readyRead()));
		connect(this->socket, SIGNAL(disconnected()), SLOT(disconnected()));
		QByteArray *buffer = new QByteArray();
		qint32 *s = new qint32(0);
		buffers.insert(this->socket, buffer);
		sizes.insert(this->socket, s);

//    qDebug() << "New Connection...";
	}
}

void tTcpSocket::writeData(QByteArray data)
{
	if ((this->socket != NULL) && (this->socket->state() == QAbstractSocket::ConnectedState))
	{
		this->socket->write(IntToArray(data.size())); //write size of data
		this->socket->write(data); //write the data itself
		this->socket->flush();
		qDebug() << "TCP <= " << data;
	}
}

void tTcpSocket::disconnect()
{
  if (this->socket != NULL)
    this->socket->close();

  if (this->server != NULL)
    this->server->close();
}

void tTcpSocket::connected()
{
  qDebug() << "connected...";
}

void tTcpSocket::disconnected()
{
	QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
	QByteArray *buffer = buffers.value(socket);
	qint32 *s = sizes.value(socket);
	socket->deleteLater();
	delete buffer;
	delete s;

//	qDebug() << "disconnected...";
}

void tTcpSocket::readyRead()
{
	QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
	QByteArray *buffer = buffers.value(socket);
	qint32 *s = sizes.value(socket);
	qint32 size = *s;
//  qDebug() << socket->bytesAvailable() << "Bytes Available";
	while (socket->bytesAvailable() > 0)
	{
		buffer->append(socket->readAll());
		while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size)) //While can process data, process it
		{
			if (size == 0 && buffer->size() >= 4) //if size of data has received completely, then store it on our global variable
			{
                size = ArrayToInt(buffer->mid(0, 4));
				*s = size;
				buffer->remove(0, 4);
			}
			if (size > 0 && buffer->size() >= size) // If data has received completely, then emit our SIGNAL with the data
			{
				QByteArray data = buffer->mid(0, size);
				buffer->remove(0, size);
				size = 0;
				*s = size;
				emit dataReceived(data);
			}
		}
	}
}

qint32 ArrayToInt(QByteArray source)
{
    qint32 temp;;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

QByteArray IntToArray(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}
