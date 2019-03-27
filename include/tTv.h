#ifndef TTV_H
#define TTV_H

#include "tEvent.h"
#include "tBackground.h"
#include "tScheduler.h"

#include <QList>
#include <QString>
#include <QDomDocument>
#include <QDomElement>

class tTv
{
public:
  tTv()
    : isModify(false) {}
  tTv(QString _name)
    : name(_name), isModify(false) {
    numHDMI=0; useCEC=false;
  }
  tTv(QString _name, QString _url)
    : name(_name), url(_url), isModify(false) {}
  tTv(QDomElement &node) {
    this->fromXml(node);
    this->isModify = false;
    this->powerOn = false;
  }

  /** Set Attribut */
  void setName(QString _name)     { name      = _name;    this->isModify = true;  }
  void setUrl(QString _url)     	{ url       = _url;     this->isModify = true;  }
  void setNumHDMI(int _num)       { numHDMI   = _num;     this->isModify = true;  }
  void setUseCEC(bool _enable)    { useCEC    = _enable;  this->isModify = true;  }
  void setPowerOn(bool _enable)   { powerOn   = _enable; }

  /** Get Attribut */
  QString getName()       { return name;      }
  QString getUrl()       	{ return url;       }
  int     getNumHDMI()    { return numHDMI;   }
  bool    getUseCEC()     { return useCEC;    }
  bool    getPowerOn()    { return powerOn;   }

  /** Convert Data to Xml */
  QDomElement toXml(QDomDocument &parent, QDomElement &child) {
    QDomElement tv = parent.createElement("TV");
    tv.setAttribute("name", name);
    tv.setAttribute("url", url);
    tv.setAttribute("numHDMI", numHDMI);
    tv.setAttribute("useCEC", useCEC);
    child.appendChild(tv);
    for (QList<tScheduler>::iterator i=this->listScheduler.begin() ; i!=this->listScheduler.end() ; ++i) {
        (*i).toXml(parent, tv);
      }
    for (QList<tEvent>::iterator i=this->listEvent.begin() ; i!=this->listEvent.end() ; ++i) {
        (*i).toXml(parent, tv);
      }
    for (QList<tBackground>::iterator i=this->listBackground.begin() ; i!=this->listBackground.end() ; ++i) {
        (*i).toXml(parent, tv);
      }
    return tv;
  }

  /** Convert Xml to Data */
  void fromXml(QDomElement &node) {
    if (node.tagName() == "TV") {
        name = node.attribute("name");
        url = node.attribute("url");
        // Delete all 0 after .
        while (url.indexOf(".0") >= 0) {
            url = url.replace(".0", ".");
          }
        numHDMI = node.attribute("numHDMI", "0").toInt();
        useCEC = node.attribute("useCEC", "0").toInt() > 0;

        QDomElement child = node.firstChildElement();
        while(!child.isNull())
          {
            /** On récupere les planning */
            if (child.tagName() == "Scheduler")
              {
                /** On ajoute le Media dans la liste des Media */
                this->listScheduler.append(tScheduler(child));
              }

            /** On récupere les événements */
            if (child.tagName() == "Event")
              {
                /** On ajoute le Media dans la liste des Media */
                this->listEvent.append(tEvent(child));
              }

            /** On récupere les arriéres plan */
            if (child.tagName() == "Background")
              {
                /** On ajoute l'arriére plan dans la liste des arriére plan */
                this->listBackground.append(tBackground(child));
              }
            child = child.nextSiblingElement();
          }
      }
  }

  void clearAll() {
    listEvent.clear();
    listBackground.clear();
    listScheduler.clear();
  }

  bool maybeSave() {
    bool res = false;
    for (QList<tEvent>::iterator i=this->listEvent.begin() ; i!=this->listEvent.end() ; ++i) {
        res |= (*i).maybeSave();
      }
    for (QList<tBackground>::iterator i=this->listBackground.begin() ; i!=this->listBackground.end() ; ++i) {
        res |= (*i).maybeSave();
      }
    return (res | isModify);
  }

  void saveDone() {
    for (QList<tEvent>::iterator i=this->listEvent.begin() ; i!=this->listEvent.end() ; ++i) {
        (*i).saveDone();
      }
    for (QList<tBackground>::iterator i=this->listBackground.begin() ; i!=this->listBackground.end() ; ++i) {
        (*i).saveDone();
      }
    isModify = false;
  }

  /** Debug */
  QString toString() {
    QString res;
    res = "tTv => \t name: " + name + ",\n";
    res += "\t\t url: " + url + "\n";
    res += "\t\t numHDMI: " + QString("%1").arg(numHDMI) + "\n";
    res += "\t\t useCEC: " + QString("%1").arg(useCEC) + "\n";
    for (QList<tScheduler>::iterator i=this->listScheduler.begin() ; i!=this->listScheduler.end() ; ++i) {
        res += "   " + (*i).toString();
      }
    for (QList<tEvent>::iterator i=this->listEvent.begin() ; i!=this->listEvent.end() ; ++i) {
        res += "      " + (*i).toString();
      }
    for (QList<tBackground>::iterator i=this->listBackground.begin() ; i!=this->listBackground.end() ; ++i) {
        res += "      " + (*i).toString();
      }
    return res;
  }

  virtual ~tTv() {}

  QList<tEvent>       listEvent;
  QList<tBackground>  listBackground;
  QList<tScheduler>   listScheduler;

protected:

private:
  QString         name;
  QString         url;
  int             numHDMI;
  bool            useCEC;
  bool            powerOn;
  bool            isModify;
};

#endif // TTV_H
