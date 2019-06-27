#include "mservice.h"
#include <QProcess>

//-------------------------------------------------------
#ifdef CAN_INIT
// если инициализация CAN в программе (сейчас в загрузочном скрипте)
void spawn(char* program, char** argv_list)
{
    pid_t child_pid;
    child_pid = fork();
    if (child_pid != 0) {
        //parent proc : wait end shild
//        while (wait(&status) != child_pid) ;
    }
    else {
        //shild proc
        execv(program, argv_list);
    }

}

void initSystemCan()
{
    char *argv[] = {
           "/sbin/ip", "link", "set", "can0", "down", NULL,
            "/sbin/ip", "link", "set", "can0", "type", "can", "bitrate", "250000", "triple-sampling", "on", NULL,
            "/sbin/ip", "link", "set", "can0", "up", NULL,
           "/sbin/ip", "link", "set", "can1", "down", NULL,
            "/sbin/ip", "link", "set", "can1", "type", "can", "bitrate", "250000", "triple-sampling", "on", NULL,
            "/sbin/ip", "link", "set", "can1", "up", NULL,
            NULL
        };
   int i = 0;
     do {

         spawn(argv[i], &argv[i]);
         sleep(1);//2000);

         while (argv[i]) i++;
         i++;
     } while (argv[i]);

}

#endif

//для Debug
QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}


//-----------------------------------
// Установка времени и даты
void setDataTimeReal(QString strTime)
{
    QString str1 = QString("date -s \"");
    str1 += strTime;
    str1 += "\"";
    QProcess * proc = new QProcess();
    if (proc) {
        proc->start(str1);
        if (proc->waitForStarted()) {
            while (proc->waitForFinished());
            proc->close();
            proc->start("hwclock -w");
            if (proc->waitForStarted()) {
                while (proc->waitForFinished());
                proc->close();
            }
         }
        delete proc;
    }
}

//-------------------------------------------------------
// Обновление скрипта загрузки
void updateScriptBoot()
{
    QFile fileBoot(FILE_SCRIPT_BOOT);
    if (fileBoot.exists())
    {
        if (fileBoot.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&fileBoot);
            QString strScr = in.readAll();
            if (!strScr.isEmpty()) {
                qDebug() << "S99greyupdReboot.sh read yes";

                QFile fileScr("/etc/init.d/S99greyupdFlash.sh");
                if (!fileScr.exists()) {
                    fileScr.setFileName("/etc/init.d/S99greyupd.sh");
                    if (!fileScr.exists()){
                        fileScr.setFileName("/etc/init.d/S99greyupd_upd.sh");
                        if (!fileScr.exists()){
                            fileScr.setFileName("/etc/init.d/S99greyupdRestart.sh");
                        }
                    }
                }
                if (fileScr.exists()) {
                    //fileScr.remove();
                    if (fileScr.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream out(&fileScr);
                        out <<  strScr;
                        out.flush();
                        fileScr.close();
                        fileBoot.close();
                        qDebug() << "S99greyupdRestart.sh write yes";
                        fileBoot.remove();

                    }
                }
            }
        }
     }
}
//------------------------------------------------------

#ifdef WDT_YES
// состояние WDT: testCan=3 -> canThread0 = 2 -> canThread1 = 1 -> algoritmThread = 0

//-------------------------------------------------------
//---  WDT

int fdWDT = -1;

/*
 * This function simply sends an IOCTL to the driver, which in turn ticks
 * the PC Watchdog card to reset its internal timer so it doesn't trigger
 * a computer reset.
 */
static void softWdtReset(void)
{
    int dummy;
    if (fdWDT>=0){
        ioctl(fdWDT, WDIOC_KEEPALIVE, &dummy);
        qDebug() << "WDT reset!";
    }
}

int softWdtOn()
{
    int flags;
    fdWDT = open("/dev/watchdog1", O_WRONLY);
    if (fdWDT == -1) {
        qDebug() << "WDT not open!";
        return -1;
    }
    // включение WDT
    int timeout = WDT_TIMEOUT;
    ioctl(fdWDT, WDIOC_SETTIMEOUT, &timeout);

   flags = WDIOS_ENABLECARD;
   ioctl(fdWDT, WDIOC_SETOPTIONS, &flags);
   qDebug() << "WDT enable!";
   return 0;
}

int softWdtOff()
{
    int flags;
    if (fdWDT >= 0) {
        // выключение WDT
        flags = WDIOS_DISABLECARD;
        ioctl(fdWDT, WDIOC_SETOPTIONS, &flags);
        qDebug() << "WDT disable!";
        close(fdWDT);
        fdWDT = -1;
        return 0;
    }
    return -1;
}

#endif


#ifdef TEST_FRAME
MTestFrame::MTestFrame(QWidget *parent ) : QWidget(parent) {
        QPalette Pal(palette());
        setGeometry(0,0,800,480);
        Pal.setColor(QPalette::Background, QColor(0, 0, 0, 80) );
        setAutoFillBackground(true);
        setPalette(Pal);
    }
void MTestFrame::paintEvent(QPaintEvent * event)
{
        Q_UNUSED(event);
        QPainter paint(this);
     QRect rect1 = QRect(0, 0, rect().width(), rect().height());
    QColor colorMain = QColor(255, 255, 255, 120);
   paint.setPen(QPen(colorMain, 2, Qt::SolidLine));
   paint.setBrush(Qt::NoBrush);
   paint.drawRect(rect1);
    paint.drawLine(QPoint(0, rect().height()/2), QPoint(rect().width(), rect().height()/2));
    paint.drawLine(QPoint(0, rect().height()/4), QPoint(rect().width(), rect().height()/4));
    paint.drawLine(QPoint(0, rect().height()/4*3), QPoint(rect().width(), rect().height()/4*3));
    paint.drawLine(QPoint(rect().width()/2,0), QPoint(rect().width()/2, rect().height()));

    }
#endif

//--------------------------------------------------
//  i2c
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



//-------------------------------------
// Чтение байта
int get_i2c_register(int file,               // файл i2c в /dev
                            unsigned char addr,     // адрес устройства на шине
                            unsigned char reg,      // адрес регистра
                            unsigned char *val)     // куда читать
{
    unsigned char inbuf, outbuf;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    /* Чтобы прочитать регистр, мы сначала делаем «фиктивную запись», записывая
      * 0 байтов в регистр, который мы хотим прочитать. Это похоже на
      * пакет в set_i2c_register, за исключением 1 байта, а не 2.  */
    outbuf = reg;
    messages[0].addr  = addr;
    messages[0].flags = 0;
    messages[0].len   = sizeof(outbuf);
    messages[0].buf   = &outbuf;

    // Данные будут возвращены в этой структуре
    messages[1].addr  = addr;
    messages[1].flags = I2C_M_RD/* | I2C_M_NOSTART*/;
    messages[1].len   = sizeof(inbuf);
    messages[1].buf   = &inbuf;

    /* Отправьте запрос в ядро и верните результат */
    packets.msgs      = messages;
    packets.nmsgs     = 2;
    if(ioctl(file, I2C_RDWR, &packets) < 0) {
        perror("Unable to send data");
        return 1;
    }
    *val = inbuf;

    return 0;
}

//----------------------------------------------------
// Запись байта
int set_i2c_register(int file,           // файл i2c в /dev
                            unsigned char addr, // адрес уст-ва на шине
                            unsigned char reg,  // регистр
                            unsigned char value) // что писать
{

    unsigned char outbuf[2];
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

    messages[0].addr  = addr;
    messages[0].flags = 0;
    messages[0].len   = sizeof(outbuf);
    messages[0].buf   = outbuf;

    // Первый байт : в какой регистр пишем
    outbuf[0] = reg;
    // следующие: что писать
    outbuf[1] = value;

   // передать сообщения через ядро
    packets.msgs  = messages;
    packets.nmsgs = 1;
    if(ioctl(file, I2C_RDWR, &packets) < 0) {
        qDebug() << "Unable to send data";
        return 1;
    }

    return 0;
}


