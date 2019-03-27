#ifndef TBACKGROUND_H
#define TBACKGROUND_H

#include <QDebug>
#include <QDate>
#include <QString>
#include <QDateTime>
#include <QDomDocument>
#include <QDomElement>

class tBackground
{
  public:
    tBackground()
			: isModify(false) {}
    tBackground(QString _name)
      : name(_name), isModify(false) {}
    tBackground(QString _name, QDate _date, QString _hourStart, QString _hourEnd)
      : name(_name), date(_date), hourStart(_hourStart), hourEnd(_hourEnd), isModify(false) {}
    tBackground(QDomElement &node)
    {
      this->fromXml(node);
			this->isModify = false;
    }

    /** Set Attribut */
    void setName(QString _name)     		{ this->name      = _name;    this->isModify = true;  }
    void setDate(QDate _date)     			{ this->date      = _date;    this->isModify = true;  }
    void setDateIndex(int _index)   		{ this->date      = QDate::fromString(QDate::longDayName(_index), "dddd"); }//(_index == 0 ? "Lundi" : (_index == 1 ? "Mardi" : (_index == 2 ? "Mercredi" : (_index == 3 ? "Jeudi" : (_index == 4 ? "Vendredi" : (_index == 5 ? "Samedi" : "Unknown"))))));   this->isModify = true;   }
    void setHourStart(QString _hour)    { this->hourStart = _hour;    this->isModify = true;  }
    void setHourEnd(QString _hour)    	{ this->hourEnd 	= _hour;    this->isModify = true;  }

    /** Get Attribut */
    QString getName() const { return this->name;    		}
    QDate 	getDate() const { return this->date;    		}
		QString getDateName() const { return QDate::longDayName(this->date.dayOfWeek());    		}
    int 		getDateIndex() const  { return  this->date.dayOfWeek(); }//(date == "Lundi" ? 0 : (date == "Mardi" ? 1 : (date == "Mercredi" ? 2 : (date == "Jeudi" ? 3 : (date == "Vendredi" ? 4 : (date == "Samedi" ? 5 : -1))))));    }
    QString getHourStart() const { return this->hourStart;   }
    QString getHourEnd() const { return this->hourEnd;    	}

    /** Convert Data to Xml */
    void toXml(QDomDocument &parent, QDomElement &child) {
      QDomElement newChild = parent.createElement("Background");
      newChild.setAttribute("name", this->name);
      QLocale curLocale(QLocale("en_EN"));
      QLocale::setDefault(curLocale);
      newChild.setAttribute("date", QLocale().toString(this->date, "dddd"));
      newChild.setAttribute("hourStart", this->hourStart);
      newChild.setAttribute("hourEnd", this->hourEnd);
      child.appendChild(newChild);
    }

    /** Convert Xml to Data */
    void fromXml(QDomElement &node) {
      this->name = node.attribute("name");
      QLocale curLocale(QLocale("en_EN"));
      QLocale::setDefault(curLocale);
      this->date = QLocale().toDate(node.attribute("date"), "dddd");
      this->hourStart = node.attribute("hourStart");
      this->hourEnd = node.attribute("hourEnd");
    }

		bool maybeSave() {
			return this->isModify;
		}

		void saveDone() {
      this->isModify = false;
		}

    bool operator==(const tBackground &event) {
      return ((this->name == event.getName()) &&
              (this->date == event.getDate()) &&
              (this->hourStart >= event.getHourStart()) &&
              (this->hourStart <= event.getHourEnd()));
    }

		bool isCorresponding(QDateTime date_) {
			QTime timeStart = QTime::fromString(this->hourStart, "hh:mm");
			QTime timeEnd 	= QTime::fromString(this->hourEnd, "hh:mm");
			if (this->date.dayOfWeek() == date_.date().dayOfWeek()) {
				if ((timeStart <= date_.time()) && (date_.time() < timeEnd)) {
					return true;
				}
			}
			return false;
		}

    /** Debug */
    QString toString() {
      QString res;
      res = "tBackground => \t name: " + this->name + ",\n";
      res += "\t\t\t date: " + this->date.toString("dddd") + ",\n";
      res += "\t\t\t hourStart: " + this->hourStart + "\n";
      res += "\t\t\t hourEnd: " + this->hourEnd + "\n";
      return res;
    }

    virtual ~tBackground() {}

  protected:

  private:
    QString   name;
    QDate   	date;
    QString   hourStart;
    QString   hourEnd;
		bool			isModify;
};

#endif // TBACKGROUND_H
