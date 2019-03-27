#ifndef TEVENT_H
#define TEVENT_H

#include <QDate>
#include <QString>
#include <QLocale>
#include <QDomDocument>
#include <QDomElement>

class tEvent
{
  public:
    tEvent()
			: isModify(false) {}
    tEvent(QString _name)
      : name(_name), isModify(false) {}
    tEvent(QString _name, QDate _date, QString _hour)
      : name(_name), date(_date), hour(_hour), isModify(false) {}
    tEvent(QDomElement &node)
    {
      this->fromXml(node);
			this->isModify = false;
    }

    /** Set Attribut */
    void setName(QString _name)     { this->name      = _name;    this->isModify = true;  }
    void setDate(QDate _date)     	{ this->date      = _date;    this->isModify = true;  }
    void setDateIndex(int _index)   { this->date      =  QDate::fromString(QDate::longDayName(_index), "dddd"); }//(_index == 0 ? "Monday" : (_index == 1 ? "Tuesday" : (_index == 2 ? "Wednesday" : (_index == 3 ? "Thursday" : (_index == 4 ? "Friday" : (_index == 5 ? "Saturday" : "Unknown"))))));   this->isModify = true;   }
    void setHour(QString _hour)     { this->hour      = _hour;    this->isModify = true;  }

    /** Get Attribut */
    QString getName()       const { return this->name;    }
    QDate 	getDate()       const { return this->date;    }
		QString getDateName()  	const { return QDate::longDayName(this->date.dayOfWeek());    		}
    int     getDateIndex()  const { return this->date.dayOfWeek(); }//(date == "Monday" ? 0 : (date == "Tuesday" ? 1 : (date == "Wednesday" ? 2 : (date == "Thursday" ? 3 : (date == "Friday" ? 4 : (date == "Saturday" ? 5 : -1))))));    }
    QString getHour()       const { return this->hour;    }

    /** Convert Data to Xml */
    void toXml(QDomDocument &parent, QDomElement &child) {
      QDomElement newChild = parent.createElement("Event");
      newChild.setAttribute("name", this->name);
      QLocale curLocale(QLocale("en_EN"));
      QLocale::setDefault(curLocale);
      newChild.setAttribute("date", QLocale().toString(this->date, "dddd"));
      newChild.setAttribute("hour", this->hour);
      child.appendChild(newChild);
    }

    /** Convert Xml to Data */
    void fromXml(QDomElement &node) {
      this->name = node.attribute("name");
      QLocale curLocale(QLocale("en_EN"));
      QLocale::setDefault(curLocale);
      this->date = QLocale().toDate(node.attribute("date"), "dddd");
      this->hour = node.attribute("hour");
    }

		bool maybeSave() {
			return isModify;
		}

		void saveDone() {
      isModify = false;
		}

    bool operator==(const tEvent &event) {
      return ((this->name == event.getName()) &&
              (this->date == event.getDate()) &&
              (this->hour == event.getHour()));
    }

		bool isCorresponding(QDateTime date_) {
			QTime timeStart = QTime::fromString(this->hour, "hh:mm");
			if (this->date.dayOfWeek() == date_.date().dayOfWeek()) {
				if ((timeStart.hour() == date_.time().hour()) && (timeStart.minute() == date_.time().minute())) {
					return true;
				}
			}
			return false;
		}
		
    /** Debug */
    QString toString() {
      QString res;
      res = "tEvent => \t name: " + this->name + ",\n";
      res += "\t\t\t date: " + this->date.toString("dddd") + ",\n";
      res += "\t\t\t hour: " + this->hour + "\n";
      return res;
    }

    virtual ~tEvent() {}

  protected:

  private:
    QString   name;
    QDate   	date;
    QString   hour;
		bool			isModify;
};

#endif // TEVENT_H
