#ifndef TOOLS
#define TOOLS

#include <QString>
#include <QDir>
#include <QDebug>

class tools {

public:

	// Mount device in current directory
	static QString mountUrlOnLinux(QString url_, QString login_, QString password_) {
		if (url_.startsWith("//")) {
				QString mountSrc = url_.mid(0, url_.indexOf("/", url_.indexOf("/", 2)+1));
				QString mountDst = mountSrc;
				mountDst.remove(0,2);
				if (!QDir(mountDst.mid(0, mountDst.lastIndexOf("/"))).exists()) {
					QDir().mkdir(mountDst.mid(0, mountDst.lastIndexOf("/")));
				}
				if (!QDir(mountDst).exists()) {
					QDir().mkdir(mountDst);
				}
				/* Mount device only if not mount */
				if (!QDir(url_.mid(2, url_.lastIndexOf("/")-2)).exists()) {
					QString mountSrcIp = mountSrc.mid(2, mountSrc.lastIndexOf("/")-2);
					QString mountSrcDir = mountSrc.mid(mountSrc.lastIndexOf("/")+1);
					QString  mountCmd = QString("sudo mount -t cifs -o username=%1,password=%2,vers=1.0,ro \"//$(nmblookup %3 | grep -v \"querying\" | grep -v \"Unknown\" | grep -v \"Ignoring\" | awk '{print $1}')/%4\" \"%5\"").arg(login_).arg(password_).arg(mountSrcIp).arg(mountSrcDir).arg(mountDst);
//					qDebug() << "mount device with command: " << mountCmd;
					system(QString("%1").arg(mountCmd).toStdString().c_str());
				}
				/* Change the Url */
				return url_.replace(mountSrc, mountDst);
			}
		return url_;
	}
};

#endif // TOOLS

