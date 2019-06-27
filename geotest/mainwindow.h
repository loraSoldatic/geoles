#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtSerialPort/QSerialPort>
#include <QThread>

#include "mdbdata.h"
#include "mserial.h"
/*
#include <RaspiCommonSettings.h>
#include "RaspiCamControl.h"
#include "RaspiPreview.h"
#include "RaspiCLI.h"
#include "RaspiHelpers.h"
#include "RaspiGPS.h"
*/
#include <semaphore.h>

#include <stdbool.h>


namespace Ui {
class MainWindow;
}



#define SIZE_FL_ELIPSE 24
#define SP_FL_ELIPSE 2
#define COLOR_ACTIVE_GREEN 0
#define COLOR_ACTIVE_RED 1

#define MOBJ_GRAPH_FONT "DejaVuSans"
#define MOBJ_GRAPH_FONT_SIZE 12

typedef struct {
    int numbR;
    QWidget *wid;
}sDescrRStateVdev;


class MObjGraphFl : public QWidget
{
public:
     MObjGraphFl (QWidget *parent, QString name, int x, int y, int w, int h, int _col, bool _over) : QWidget(parent)
    {
         m_state = false;
        setGeometry(x, y, w, h);
        setStyleSheet( "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffffff, stop: 1 #bcbcbc);"
                       "border-style: solid;"
                       "border-radius: 5;"
                       "border-width: 2;"
                       "font: 11px;"
        );
        m_name = name;
        colorActive = _col;
        textOver = _over;

    }

     MObjGraphFl (QWidget *parent, QString name, int w, int h, int _col, bool _over) : QWidget(parent)
    {
         m_state = false;
        setGeometry(0, 0, w, h);
        setStyleSheet( "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffffff, stop: 1 #bcbcbc);"
                       "border-style: solid;"
                       "border-radius: 5;"
                       "border-width: 2;"
                       "font: 11px;"
        );
        m_name = name;
        colorActive = _col;
        textOver = _over;

    }

     MObjGraphFl (QWidget *parent, QString name, int w, int h, bool _over) : QWidget(parent)
    {
         m_state = false;
        setGeometry(0, 0, w, h);
        setStyleSheet( "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffffff, stop: 1 #bcbcbc);"
                       "border-style: solid;"
                       "border-radius: 5;"
                       "border-width: 2;"
                       "font: 11px;"
        );
        m_name = name;
        colorActive = COLOR_ACTIVE_GREEN;
        textOver = _over;

    }



     MObjGraphFl (QWidget *parent, QString name, int w, int h) : QWidget(parent)
    {
        m_state = false;
        setGeometry(0, 0, w, h);
        setStyleSheet( "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffffff, stop: 1 #bcbcbc);"
                       "border-style: solid;"
                       "border-radius: 5;"
                       "border-width: 2;"
                       "font: 11px;"
        );
        m_name = name;
        colorActive = COLOR_ACTIVE_GREEN;
        textOver = false;

    }
    ~MObjGraphFl(){}
    void setState(bool state)  {
        m_state = state;
        update();
    }
    void setName(QString name) {
        m_name = name;
        update();
    }
    void setColorActive(int _color)
    {
        colorActive = _color;
    }
    void setState_name(bool _state, QString _name)
    {
        m_state = _state;
        m_name = _name;
        update();
    }

protected:
    QString m_name;
    bool m_state;
    int colorActive;
    bool textOver;
protected:
    void paintEvent(QPaintEvent * event)
    {
        Q_UNUSED(event);
        QPainter paint(this);

        paint.setRenderHint(QPainter::Antialiasing, true); //sglagivanie
        QColor colorS, colorT;
         QRadialGradient rg(QPoint((height()-2)/2, (height()-2)/2), SIZE_FL_ELIPSE/2);
         if (colorActive == COLOR_ACTIVE_GREEN) {
             if (m_state) {
                 colorS = QColor(0x1a, 0xe9, 0x64);
                 colorT = QColor(0x56, 0x86, 0x67);
                rg.setColorAt(0, colorS);
                rg.setColorAt(1, colorT);
             }
             else {
                 colorS =QColor(0x8e, 0x94, 0x8f) ;
                 colorT = QColor(0xd9, 0xd9, 0xd9);
                 rg.setColorAt(0, colorS);
                 rg.setColorAt(1, colorT);
             }

         }
         else {
            if (m_state)
            {
                colorS = QColor(0x21, 0xae, 0x53);
                colorT = QColor(0x22, 0x5c, 0x36);
                rg.setColorAt(0, colorS);
                rg.setColorAt(1, colorT);
             }
             else {
                colorS = QColor(0xf4, 0x34, 0x1b);
                colorT = QColor(0xb7, 0x80, 0x77);
                rg.setColorAt(0, colorS);
                rg.setColorAt(1, colorT);
             }

         }
          paint.setBrush(QBrush(rg));
         paint.setPen(QPen(colorS));
         paint.drawEllipse(3, (height() - SIZE_FL_ELIPSE)/2, SIZE_FL_ELIPSE, SIZE_FL_ELIPSE);

         QFont font("Verdana", 12, QFont::Light);
         paint.setFont(font);
         paint.setPen(QPen(QColor(0,0,0)));
         paint.setBrush(QBrush(Qt::NoBrush));

         QRect rectT;
         if (!textOver)
            rectT  = QRect(SIZE_FL_ELIPSE+5, 1, width()-(SIZE_FL_ELIPSE+3), height()-2);
         else rectT  = QRect(1, 1, width()-2, height()-2);
         paint.drawText(rectT,Qt::AlignLeft | Qt::AlignVCenter, m_name);


    }
};



//-------------------------------------------



void DrawButOnOff(QPushButton *but, bool state);


int getNumbObjFromString(QString str);


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MainInterface *interfaceBld;
    ManDataMdb *manDataMdb;
    QThread *serialThread;
    quint8 cmBld;
    quint8 stateBld;
    quint8 intervalPack;

    QVector <QCheckBox*> vecCh;
    QVector <MObjGraphFl*> vecLed;


private slots:
     // слот обработки получения пакета
    void slotHandRxBld(QByteArray);
    void slotStartInterface();

    void slotCheckedPack();
    void slotSendReq();
    void slotSendOff();
    void slotCamera();

    void slotClose();


signals:
    void signalSendBld(QByteArray);
};

#endif // MAINWINDOW_H
