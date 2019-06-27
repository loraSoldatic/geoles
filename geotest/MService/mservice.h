#ifndef MSERVICE
#define MSERVICE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <qdebug.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <signal.h>
#include <QCloseEvent>

#include <errno.h>
#include <linux/i2c-dev.h>

#include <QLabel>

#include "mparametr.h"

// для системных вызовов
void spawn(char* program, char** argv_list);
// инициализация CAN
void initSystemCan();
// установка даты времени
void setDataTimeReal(QString strTime);
QTextStream& qStdOut();

//-------------------------------------------
// i2c
#define NAME_BUS_I2C_ADC_0 "/dev/i2c-0"
#define NAME_BUS_I2C_ADC_1 "/dev/i2c-1"
// Чтение байта
int get_i2c_register(int file, unsigned char addr, unsigned char reg, unsigned char *val);
// Запись байта
int set_i2c_register(int file, unsigned char addr, unsigned char reg, unsigned char value);
// чтение статуса ADC7180
int getStatusAdc( quint8 *addr) ;

//-------------------------------------------------------
// Обновление скрипта загрузки
void updateScriptBoot();


#ifdef WDT_YES
static void softWdtReset(void);
int softWdtOn();
int softWdtOff();

#endif

#ifdef TEST_FRAME
class MTestFrame : public QWidget
{
public:
    MTestFrame(QWidget *parent = 0 );
protected:
    void paintEvent(QPaintEvent * event);

};
#endif


class MLabel : public QWidget
{
public:
    explicit MLabel(QWidget *parent = 0) : QWidget(parent){
        colorMain = Qt::lightGray;//Qt::yellow;
        stringMain = "";
    }
    void setCfg(QRect rectM, QColor colorM, QFont fontM, QString strM) {
        setGeometry(QRect(rectM.x(), rectM.y(), rectM.height(), rectM.width()));
        colorMain = colorM;
        fontMain = fontM;
        stringMain = strM;
    }
    void setTextLabel(QString _text)
    {
        stringMain = _text;
        update();
    }
    void setColorText(QColor colorM) {
        colorMain = colorM;
    }

protected:
    QColor colorMain;
    QFont fontMain;
    QString stringMain;
    void paintEvent(QPaintEvent * event) {
        Q_UNUSED(event);
        QPainter paint(this);
        paint.setRenderHint(QPainter::Antialiasing, true); //sglagivanie
        paint.setTransform(QTransform()
               .translate(rect().width() / 2, rect().height() / 2)
               //.rotate(89, Qt::XAxis)
               .rotate(-90, Qt::ZAxis)
               .translate(-rect().height()/2 , -rect().width()/2));

     //  fontMain.setPixelSize(12);
       paint.setFont(fontMain);
       paint.setPen(QPen(colorMain, 1, Qt::SolidLine));
       QRect rect1 = QRect(0, 0, rect().height(), rect().width());

       paint.drawText(rect1, stringMain);
    }
};



#endif // MSERVICE

