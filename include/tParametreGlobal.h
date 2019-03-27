#ifndef TPARAMETREGLOBAL_H
#define TPARAMETREGLOBAL_H

#include "tMedia.h"
#include "tTv.h"

#include <QList>
#include <QString>
#include <QSettings>
#include <QDomDocument>
#include <QDomElement>

class tParametreGlobal
{
public:
    tParametreGlobal() {}
    tParametreGlobal(QDomElement &node)
    {
        this->fromXml(node);
    }

    /** Convert Data to Xml */
    QDomDocument toXml(QDomDocument &parent, QDomElement &child) {
        QSettings setting;
        // Logo
        QDomElement logo = parent.createElement("Logo");
        logo.setAttribute("enable", setting.value("Logo/Enable").toBool());
        logo.setAttribute("url", setting.value("Logo/Url").toString());
        logo.setAttribute("posX", setting.value("Logo/PosX").toInt());
        logo.setAttribute("posY", setting.value("Logo/PosY").toInt());
        child.appendChild(logo);
        // Background
        QDomElement background = parent.createElement("Background");
        background.setAttribute("url", setting.value("Background").toString());
        child.appendChild(background);
        QDomElement media_list = parent.createElement("Media_List");
        child.appendChild(media_list);
        for (QList<tMedia>::iterator i=this->listMedia.begin() ; i!=this->listMedia.end() ; ++i) {
            (*i).toXml(parent, media_list);
        }
        QDomElement tv_list = parent.createElement("TV_List");
        child.appendChild(tv_list);
        for (QList<tTv>::iterator i=this->listTv.begin() ; i!=this->listTv.end() ; ++i) {
            (*i).toXml(parent, tv_list);
        }
        return parent;
    }

    /** Convert Xml to Data */
    void fromXml(QDomElement &node) {
        QSettings setting;

        /** On verifie que le fichier soit bien celui que nous avons creer */
        if (node.tagName() == "TV_Controller")
        {
            QDomElement child = node.firstChildElement();
            while(!child.isNull())
            {
                // Logo
                if (child.tagName() == "Logo")
                {
                    setting.setValue("Logo/Url", child.attribute("url"));
                    setting.setValue("Logo/Enable", child.attribute("enable").toInt() > 0);
                    setting.setValue("Logo/PosX", child.attribute("posX").toInt());
                    setting.setValue("Logo/PosY", child.attribute("posY").toInt());
                }

                // Background
                if (child.tagName() == "Background")
                {
                    setting.setValue("Background", child.attribute("url"));
                }

                /** On récupere la liste des Media */
                if (child.tagName() == "Media_List")
                {
                    QDomElement smallChild = child.firstChildElement();
                    while(!smallChild.isNull()) {
                        /** On ajoute le Media dans la liste des Media */
                        this->listMedia.append(tMedia(smallChild));

                        smallChild = smallChild.nextSiblingElement();
                    }
                }

                /** On récupere la liste des TV */
                if (child.tagName() == "TV_List")
                {
                    QDomElement smallChild = child.firstChildElement();
                    while(!smallChild.isNull()) {
                        /** On ajoute la TV dans la liste des TV */
                        this->listTv.append(tTv(smallChild));

                        smallChild = smallChild.nextSiblingElement();
                    }
                }

                child = child.nextSiblingElement();
            }
        }
    }

    void clearAll() {
        listMedia.clear();
        for (QList<tTv>::iterator i=this->listTv.begin() ; i!=this->listTv.end() ; ++i) {
            (*i).clearAll();
        }
        listTv.clear();
    }

    bool maybeSave() {
        bool res = false;
        for (QList<tMedia>::iterator i=this->listMedia.begin() ; i!=this->listMedia.end() ; ++i) {
            res |= (*i).maybeSave();
        }
        for (QList<tTv>::iterator i=this->listTv.begin() ; i!=this->listTv.end() ; ++i) {
            res |= (*i).maybeSave();
        }
        return res;
    }

    void saveDone() {
        for (QList<tMedia>::iterator i=this->listMedia.begin() ; i!=this->listMedia.end() ; ++i) {
            (*i).saveDone();
        }
        for (QList<tTv>::iterator i=this->listTv.begin() ; i!=this->listTv.end() ; ++i) {
            (*i).saveDone();
        }
    }

    /** Debug */
    QString toString() {
        QString res;
        res = "tParametreGlobal => \n ";
        for (QList<tMedia>::iterator i=this->listMedia.begin() ; i!=this->listMedia.end() ; ++i) {
            res += "   " + (*i).toString();
        }
        for (QList<tTv>::iterator i=this->listTv.begin() ; i!=this->listTv.end() ; ++i) {
            res += "   " + (*i).toString();
        }
        return res;
    }

    virtual ~tParametreGlobal() {}

    //  protected:
    //
    //  private:
    QList<tMedia>   listMedia;
    QList<tTv>      listTv;

};

#endif // TPARAMETREGLOBAL_H
