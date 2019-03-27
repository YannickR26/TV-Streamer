#ifndef TMEDIA_H
#define TMEDIA_H

#include <stdio.h>

#include "tools.h"

#include <QTime>
#include <qcolor.h>
#include <QString>
#include <QDomElement>
#include <QDomDocument>

class tMedia
{
  public:
    tMedia()
      : fadeIn(0), fadeOut(0), repeat(0), login(""), password(""), time(), isModify(false) {}
    tMedia(QString _name)
      : name(_name), fadeIn(0), fadeOut(0), repeat(0), login(""), password(""), time(), isModify(false) {}
    tMedia(QString _name, QColor _color)
      : name(_name), fadeIn(0), fadeOut(0), repeat(0), login(""), password(""), color(_color), time(0,0,0,0), isModify(false) {}
    tMedia(QString _name, QString _url, QString _type, bool _fadeIn, bool _fadeOut, bool _repeat, QColor _color, QTime _time)
      : name(_name), url(_url), type(_type), fadeIn(_fadeIn), fadeOut(_fadeOut), repeat(_repeat), login(""), password(""), color(_color), time(_time), isModify(false) {}
    tMedia(QDomElement &node)
    {
      this->fromXml(node);
			this->isModify = false;
    }

    /** Set Attribut */
    void setName(QString _name)         { this->name      = _name;      this->isModify = true;	}
    void setUrl(QString _url)           { this->url       = _url;       this->isModify = true;	}
    void setType(QString _type)         { this->type      = _type;      this->isModify = true;	}
    void setFadeIn(bool _fadeIn)        { this->fadeIn    = _fadeIn;    this->isModify = true;	}
    void setFadeOut(bool _fadeOut)      { this->fadeOut   = _fadeOut;   this->isModify = true;	}
    void setRepeat(bool _repeat)        { this->repeat    = _repeat;    this->isModify = true;	}
    void setLogin(QString _login)       { this->login     = _login;     this->isModify = true;	}
    void setPassword(QString _password) { this->password  = _password;  this->isModify = true;	}
    void setColor(QColor _color)        { this->color     = _color;     this->isModify = true;	}
    void setTime(QTime _time)           { this->time      = _time;      this->isModify = true;	}

    /** Get Attribut */
    QString getName()       { return this->name;    }
    QString getUrl()        { return this->url;     }
    QString getType()       { return this->type;    }
    bool    getFadeIn()     { return this->fadeIn;  }
    bool    getFadeOut()    { return this->fadeOut; }
    bool    getRepeat()     { return this->repeat;  }
    QString getLogin()      { return this->login;   }
    QString getPassword()   { return this->password;}
    QColor  getColor()      { return this->color;   }
    QTime   getTime()       { return this->time;    }

    /** Convert Data to Xml */
    void toXml(QDomDocument &parent, QDomElement &child) {
      QDomElement newChild = parent.createElement("Media");
      newChild.setAttribute("name", this->name);
      newChild.setAttribute("url", this->url);
      newChild.setAttribute("type", this->type);
      newChild.setAttribute("fadeIn", this->fadeIn);
      newChild.setAttribute("fadeOut", this->fadeOut);
      newChild.setAttribute("repeat", this->repeat);
      newChild.setAttribute("login", this->login);
      newChild.setAttribute("password", this->password);
      newChild.setAttribute("color", this->color.name());
      newChild.setAttribute("time", this->time.toString());
      child.appendChild(newChild);
    }

    /** Convert Xml to Data */
    void fromXml(QDomElement &node) {
      if (node.tagName() == "Media") {
        this->name = node.attribute("name");
        this->url = node.attribute("url");
        this->type = node.attribute("type");
        this->fadeIn = (node.attribute("fadeIn") == "0" ? false : true);
        this->fadeOut = (node.attribute("fadeOut") == "0" ? false : true);
        this->repeat = (node.attribute("repeat") == "0" ? false : true);
        this->login = node.attribute("login");
        this->password = node.attribute("password");
        this->color = QColor(node.attribute("color"));
        this->time = QTime::fromString(node.attribute("time"));
				
				/* check the url */
				this->url = tools::mountUrlOnLinux(this->url, this->login, this->password);
      }
    }

		bool maybeSave() {
			return isModify;
		}

		void saveDone() {
      isModify = false;
		}

    /** Debug */
    QString toString() {
      QString res;
      res = "tMedia => \t name: " + name + ",\n";
      res += "\t\t url: " + url + ",\n";
      res += "\t\t type: " + type + ",\n";
      res += QString("\t\t fadeIn: %1,\n").arg(fadeIn);
      res += QString("\t\t fadeOut: %1,\n").arg(fadeOut);
      res += QString("\t\t repeat: %1,\n").arg(repeat);
      res += "\t\t login: " + login + ",\n";
      res += "\t\t password: " + password + "\n";
      return res;
    }

    virtual ~tMedia() {}

  protected:

  private:
    QString   name;
    QString   url;
    QString   type;
    bool      fadeIn, fadeOut, repeat;
    QString   login, password;
    QColor    color;
    QTime     time;
		bool			isModify;
};

#endif // TMEDIA_H
