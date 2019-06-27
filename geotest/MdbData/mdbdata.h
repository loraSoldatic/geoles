#ifndef MDBDATA_H
#define MDBDATA_H

#include <QtGlobal>
#include <QTimer>
#include <QObject>
#include <QBitArray>
#include <QMutex>
#include <QDateTime>

#include "common_name.h"
#include <QStringList>

//-------------------------------
// распределение данных в общем пакете
#define OFS_DATA_SYS 0
#define OFS_DATA_GPS sizeof(TStateSysRaw)
#define OFS_DATA_CLI (OFS_DATA_GPS+sizeof(TGpsRaw))
#define OFS_DATA_LASER (OFS_DATA_CLI+sizeof(TCliRaw))
#define OFS_DATA_ACCEL (OFS_DATA_LASER+sizeof(TLaserRaw))
#define OFS_DATA_INCL (OFS_DATA_ACCEL+sizeof(TAccelRaw))
#define OFS_DATA_BUT  (OFS_DATA_INCL + sizeof(TInclRaw))
#define SIZE_DATA_BLD_RAW (sizeof(TStateSysRaw) + sizeof(TGpsRaw) + sizeof(TCliRaw) + sizeof(TLaserRaw)+sizeof(TAccelRaw)+sizeof(TInclRaw)+sizeof(TButEvent))


struct TDataConvSys {
    // общее состояние
        quint32 packTtl; // время обновления (=0 - нет данных)
        unsigned char state;
        unsigned char display;
        unsigned char button;
        float vBat;

};
#define INTERVAL_REQ_SYS    1000

struct TdataConvGps{
    // GPS
        quint64 packTtl; // время обновления (=0 - нет данных)
    // время
        QTime time;
    //дата
        QDate date;
    // статус
        unsigned char status; // 'A'-активный | 'V' - игнорировать
    // широта
        unsigned char latD; // градусы
        float latM;       // минуты
        unsigned char latSign; // знак широты: 'N'(+) - северная, 'S'(-) - южная
        float latAngle;     // широта
    // долгота
        unsigned char longD; // градусы
        float longM; // минуты
        unsigned char longSign; // знак долготы: 'E'(+) - восточная, 'W'(-) - западная
        float longAngle;     // долгота

};
#define INTERVAL_REQ_GPS    1000

struct TDataConvCli{
    // компас
        quint64 packTtl; // время обновления (=0 - нет данных)
        unsigned short coordX; // координата X
        unsigned short coordY; // координата Y
        unsigned short coordZ; // координата Z

};
#define INTERVAL_REQ_CLI    500

struct TDataConvLaser {
    //дальномер
        quint64 packTtl; // время обновления (=0 - нет данных)
        unsigned char err;
        unsigned short dist; // расстояние в м

};

struct TDataConvAccel {
    //акселерометр
        quint64 packTtl; // время обновления (=0 - нет данных)
        unsigned short HX;
        unsigned short HY;
        unsigned short HZ;

};
#define INTERVAL_REQ_ACCEL    500

struct TDataConvIncl {
    // инклинометр
        quint64 packTtl; // время обновления (=0 - нет данных)
        unsigned short acclX;
        unsigned short acclY;

        unsigned short temperature;
        unsigned short inclX;
        unsigned short inclY;
        unsigned short rot;

};
#define INTERVAL_REQ_INCL    500

//---------------------------------
typedef void (*MConvFunc) (quint8*);

//----------------------------------------------
// Дискретные параметры
#define QUANT_DIN           32  // кол-во дискретных
//----------------------------------------------
#define DIN_ST_CONNECT      0   // состояние связи: = 1 - связь установлена
#define DIN_REQ_SYS         1   // запрос системных данных
#define DIN_REQ_GPS         2   // запрос GPS активен
#define DIN_REQ_CLI         3   // запрос компаса активен
#define DIN_REQ_LASER       4   // запрос дальномера активен
#define DIN_REQ_ACCEL       5   // запрос акселерометра активен
#define DIN_REQ_INCL        6   // запрос инклинометра активен


//---------------------------------------------------------
// Аналоговые выходы
#define QUANT_AIN               32  // кол-во аналоговых входов

//------------------------------------
class ManDataMdb //: public QObject
{
//    Q_OBJECT
private:
    QMutex mutexData;
    // сырые данные
    quint8 stateNewPackData; //наличие данных пакетов
    QByteArray dataBldRaw;  // данные
    //--------------------------------------------------------
    // обраьотанные данные
    TDataConvSys dataSys;
    TdataConvGps dataGps;
    TDataConvCli dataCli;
    TDataConvLaser dataLaser;
    TDataConvAccel dataAccel;
    TDataConvIncl dataIncl;
    quint8 evButton;

public:
    ManDataMdb();
    QString getStrGpsStatus();
    QString getStrGpsLat();
    QString getStrGpsLong();
    QString getStrGpsDate();
    QString getStrGpsTime();

    QString getStrCliX();
    QString getStrCliY();
    QString getStrCliZ();

    QString getStrLaserDist();
    QString getStrLaserErr();

    QString getStrAccelX();
    QString getStrAccelY();
    QString getStrAccelZ();

    QString getStrInclAccelX();
    QString getStrInclAccelY();

    QString getStrInclX();
    QString getStrInclY();
    QString getStrInclRot();
    QString getStrInlTemp();

    QString getStrSysBatary();
    QString getStrSysDisplay();
    QString getStrSysButton();

    quint8 handBldPack(QByteArray _pack);
    void getBldData(void *ptrData, int _dev, int _field);
    void getDataStr(QStringList *listData, int _dev);

};





#endif // MDBDATA_H
