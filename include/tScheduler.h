#ifndef TSCHEDULER_H
#define TSCHEDULER_H

#include <stdio.h>

#include <QTime>
#include <QDate>
#include <QString>
#include <QLocale>
#include <QDomElement>
#include <QDomDocument>

class tScheduler
{
public:
  tScheduler(QDomElement &node)
  {
    this->fromXml(node);
  }

  /** Set Attribut */
  void setOpeningTime(QTime _time)    { this->openingTime   = _time; }
  void setClosingTime(QTime _time)    { this->closingTime   = _time; }
  void setDate(QDate _date)           { this->date          = _date; }


  /** Get Attribut */
  QTime getOpeningTime()      { return this->openingTime; }
  QTime getClosingTime()      { return this->closingTime; }
  QDate getDate()             { return this->date; }

  /** Convert Data to Xml */
  void toXml(QDomDocument &parent, QDomElement &child) {
    QDomElement newChild = parent.createElement("Scheduler");
    newChild.setAttribute("openingTime", this->openingTime.toString("hh:mm"));
    newChild.setAttribute("closingTime", this->closingTime.toString("hh:mm"));
    QLocale curLocale(QLocale("en_EN"));
    QLocale::setDefault(curLocale);
    newChild.setAttribute("date", QLocale().toString(this->date, "dddd"));
    child.appendChild(newChild);
  }

  /** Convert Xml to Data */
  void fromXml(QDomElement &node) {
    if (node.tagName() == "Scheduler") {
        this->openingTime = QTime().fromString(node.attribute("openingTime"), "hh:mm");
        this->closingTime = QTime().fromString(node.attribute("closingTime"), "hh:mm");
        QLocale curLocale(QLocale("en_EN"));
        QLocale::setDefault(curLocale);
        this->date = QLocale().toDate(node.attribute("date"), "dddd");
      }
  }

  /** Debug */
  QString toString() {
    QString res;
    res = "tScheduler => \t date: " + date.toString("dddd") + ", openingTime: " + openingTime.toString("hh:mm") + ", closingTime: " + closingTime.toString("hh:mm") + "\n";
    return res;
  }

  virtual ~tScheduler() {}

protected:

private:
  QTime     openingTime;
  QTime     closingTime;
  QDate     date;
};

#endif // TSCHEDULER_H
