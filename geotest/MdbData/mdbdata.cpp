#include <QtCore/qmath.h>
#include <limits.h>
#include <QMutex>
#include <qmath.h>
#include <QFile>

#include "mdbdata.h"
//#include "mparametr.h"
//#include "mserial.h"

// дескриптор пакета данных
struct TDescrDataPackBld{
    // тип пакета
    quint8 typePack;
    // маска для фиксирования приема
    quint8 maskState;
    // смещение данных в общем пакете сырых данных
    int ofsData;
    // размер сырых данных
    int sizeData;
    // функция преобразования
//    MConvFunc funcConv;
};

const TDescrDataPackBld TBL_DESCR_PACK_BLD[QUANT_TYPE_PACK_BLD] = {
//системные данные
    {TYPE_DATA_RAW_SYS, MASK_DATA_RAW_SYS, OFS_DATA_SYS, sizeof(TStateSysRaw)},
//данные GPS
    {TYPE_DATA_RAW_GPS, MASK_DATA_RAW_GPS, OFS_DATA_GPS, sizeof(TGpsRaw)},
// данные компаса
    {TYPE_DATA_RAW_CLI, MASK_DATA_RAW_CLI, OFS_DATA_CLI, sizeof(TCliRaw)},
// данные лазера (дальномер)
    {TYPE_DATA_RAW_LASER, MASK_DATA_RAW_LASER, OFS_DATA_LASER, sizeof(TLaserRaw)},
// данные акселерометра
    {TYPE_DATA_RAW_ACCEL, MASK_DATA_RAW_ACCEL, OFS_DATA_ACCEL, sizeof(TAccelRaw)},
// данные инклинометра
    {TYPE_DATA_RAW_INCL, MASK_DATA_RAW_INCL, OFS_DATA_INCL, sizeof(TInclRaw)},
// нажата кнопка (500мкс<time<2сек)
    {TYPE_DATA_EV_BUT, MASK_DATA_EV_BUT, OFS_DATA_BUT, 1}
};



ManDataMdb::ManDataMdb()// : QObject()
{
    stateNewPackData = 0; //нет новых данныхformat
    dataBldRaw.fill(0, SIZE_DATA_BLD_RAW);
    //--------------------------------------------------------

}


quint8 ManDataMdb::handBldPack(QByteArray _pack)
{
    QMutexLocker lock(&mutexData);
    stateNewPackData = 0;
    for(int i=0; i<_pack.length();) {
        quint8 typeData = _pack.at(i);
        for (int j=0; j<QUANT_TYPE_PACK_BLD; j++)
        {

            if (typeData == TBL_DESCR_PACK_BLD[j].typePack) {
                int size =  TBL_DESCR_PACK_BLD[j].sizeData;
                if ((size +i+1) > _pack.length()) size = _pack.length()-i-1;
                QByteArray bufTemp = QByteArray(_pack);
                bufTemp.remove(0, i+1);
                bufTemp.truncate(size);
                dataBldRaw.replace(TBL_DESCR_PACK_BLD[j].ofsData, size, bufTemp);
                TStateSysRaw tempDataSys;
                switch (typeData){
                case TYPE_DATA_RAW_SYS://   0x01    //системные данные
                    tempDataSys = *((TStateSysRaw*) &dataBldRaw.data()[OFS_DATA_SYS]);

//                    dataSys.state = tempDataSys.state;
 //                   dataSys.state = dataBldRaw.at(i+1);
                    dataSys.vBat = tempDataSys.vBatHi*256 + tempDataSys.vBatLo;
                    dataSys.display = tempDataSys.display;
                    dataSys.button = tempDataSys.button;
                    dataSys.packTtl = QDateTime::currentMSecsSinceEpoch();
                    dataSys.packTtl = QDateTime::currentMSecsSinceEpoch();
                    break;
                case TYPE_DATA_RAW_GPS: //   0x02    //данные GPS
                    // время
                    dataGps.time = QTime(_pack.at(i+1), _pack.at(i+2), _pack.at(i+3));
                    dataGps.status = _pack.at(i+4);
                    // широта
                    dataGps.latD = _pack.at(i+5);
                    dataGps.latM = _pack.at(i+6)+(float)_pack.at(i+7)*0.1 +(float)_pack.at(i+8)*0.01;
                    dataGps.latSign = _pack.at(i+9);
                    dataGps.latAngle = dataGps.latD + dataGps.latM/60;
                    if (dataGps.latSign == 'S') dataGps.latAngle *= -1;
                    // долгота
                    dataGps.longD = _pack.at(i+10);
                    dataGps.longM = _pack.at(i+11)+(float)_pack.at(i+12)*0.1 +(float)_pack.at(i+13)*0.01;
                    dataGps.longSign = _pack.at(i+14);
                    dataGps.longAngle = dataGps.longD + dataGps.longM/60;
                    if (dataGps.longSign == 'W') dataGps.longAngle *= -1;
                    //дата
                    dataGps.date = QDate(_pack.at(i+17), _pack.at(i+16), _pack.at(i+15));
                    dataGps.packTtl = QDateTime::currentMSecsSinceEpoch();
                    break;
                case TYPE_DATA_RAW_CLI: //     0x03    // данные компаса
                        // координата X (Hi LO)
                        dataCli.coordX = (quint16)_pack.at(i+1)*256 + _pack.at(i+2);
                        // координата Y (Hi Lo)
                        dataCli.coordY = (quint16)_pack.at(i+3)*256 + _pack.at(i+4);
                        // координата Z (Hi Lo)
                        dataCli.coordX = (quint16)_pack.at(i+5)*256 + _pack.at(i+6);
                        dataCli.packTtl = QDateTime::currentMSecsSinceEpoch();
                    break;
                case TYPE_DATA_RAW_LASER: // 0x04    // данные лазера (дальномер)
                    dataLaser.err = _pack.at(i+2);
                    dataLaser.dist = (quint16)_pack.at(i+3)*100 + _pack.at(i+4);
                    dataLaser.packTtl = QDateTime::currentMSecsSinceEpoch();
                    break;
                case TYPE_DATA_RAW_ACCEL:   // 0x05    // данные акселерометра
                    dataAccel.HX = _pack.at(i+1)*256 + _pack.at(i+2);
                    dataAccel.HY = _pack.at(i+3)*256 + _pack.at(i+4);
                    dataAccel.HZ = _pack.at(i+5)*256 + _pack.at(i+6);
                    dataAccel.packTtl = QDateTime::currentMSecsSinceEpoch();
                    break;
                case TYPE_DATA_RAW_INCL:    //  0x06    // данные инклинометра
                    dataIncl.acclX = _pack.at(i+1)*256 + _pack.at(i+2);
                    dataIncl.acclY = _pack.at(i+3)*256 + _pack.at(i+4);
                    dataIncl.temperature = _pack.at(i+5)*256 + _pack.at(i+6);
                    dataIncl.inclX = _pack.at(i+7)*256 + _pack.at(i+8);
                    dataIncl.inclY = _pack.at(i+9)*256 + _pack.at(i+10);
                    dataIncl.rot = _pack.at(i+11)*256 + _pack.at(i+12);
                    dataIncl.packTtl = QDateTime::currentMSecsSinceEpoch();
                    break;
                case TYPE_DATA_EV_BUT:  //    0x10    // нажата кнопка (500мкс<time<2сек)
                case TYPE_DATA_EV_BUT_L1:// 0x11    // нажата кнопка долго (3сек<time)
                    evButton = _pack.at(i+1);
                    break;
                }

                i+=size;
                stateNewPackData |= TBL_DESCR_PACK_BLD[j].maskState;
                break;
            }
        }
        i++;
    }
    return stateNewPackData;
}


QString ManDataMdb::getStrGpsStatus()
{
    QString str1 = "-";
    if (dataGps.packTtl) {
        str1 = dataGps.status;
    }
    return str1;
}

QString ManDataMdb::getStrGpsLat()
{
    QString str1 = "-";
    if (dataGps.packTtl) {
        str1 = QString("%1°").arg(dataGps.latAngle, 0, 'f', 4);
    }
    return str1;
}

QString ManDataMdb::getStrGpsLong()
{
    QString str1 = "-";
    if (dataGps.packTtl) {
        str1 = QString("%1°").arg(dataGps.longAngle, 0, 'f', 4);
    }
    return str1;
}

QString ManDataMdb::getStrGpsDate()
{
    QString str1 = "-";
    if (dataGps.packTtl) {
  //      QList<QLocale> locales {QLocale(QLocale::Russian)};
        QString format = "dd.MM.yy";
        str1 = dataGps.date.toString(format);
    }
    return str1;
}

QString ManDataMdb::getStrGpsTime()
{
    QString str1 = "-";
    if (dataGps.packTtl) {
        str1 = dataGps.time.toString("hh:mm:ss");
    }
    return str1;
}


QString ManDataMdb::getStrCliX()
{
    QString str1 = "-";
    if (dataCli.packTtl) {
        str1 = QString("%1").arg(dataCli.coordX);
    }
    return str1;

}

QString ManDataMdb::getStrCliY()
{
    QString str1 = "-";
    if (dataCli.packTtl) {
        str1 = QString("%1").arg(dataCli.coordY);
    }
    return str1;

}

QString ManDataMdb::getStrCliZ()
{
    QString str1 = "-";
    if (dataCli.packTtl) {
        str1 = QString("%1").arg(dataCli.coordZ);
    }
    return str1;

}

QString ManDataMdb::getStrLaserDist()
{
    QString str1 = "-";
    if (dataLaser.packTtl) {
        str1 = QString("%1").arg(dataLaser.dist);
    }
    return str1;

}

QString ManDataMdb::getStrLaserErr()
{
    QString str1 = "-";
    if (dataLaser.packTtl) {
        str1 = QString("%1").arg(dataLaser.err, 2, 16, QLatin1Char('0'));
    }
    return str1;
}

QString ManDataMdb::getStrAccelX()
{
    QString str1 = "-";
    if (dataAccel.packTtl) {
        str1 = QString("%1").arg(dataAccel.HX);
    }
    return str1;

}

QString ManDataMdb::getStrAccelY()
{
    QString str1 = "-";
    if (dataAccel.packTtl) {
        str1 = QString("%1").arg(dataAccel.HY);
    }
    return str1;
}

QString ManDataMdb::getStrAccelZ()
{
    QString str1 = "-";
    if (dataAccel.packTtl) {
        str1 = QString("%1").arg(dataAccel.HX);
    }
    return str1;
}


QString ManDataMdb::getStrInclAccelX()
{
    QString str1 = "-";
    if (dataIncl.packTtl) {
        str1 = QString("%1").arg(dataIncl.acclX);
    }
    return str1;
}

QString ManDataMdb::getStrInclAccelY()
{
    QString str1 = "-";
    if (dataIncl.packTtl) {
        str1 = QString("%1").arg(dataIncl.acclY);
    }
    return str1;
}


QString ManDataMdb::getStrInclX()
{
    QString str1 = "-";
    if (dataIncl.packTtl) {
        str1 = QString("%1").arg(dataIncl.inclX);
    }
    return str1;
}



QString ManDataMdb::getStrInclY()
{
    QString str1 = "-";
    if (dataIncl.packTtl) {
        str1 = QString("%1").arg(dataIncl.inclY);
    }
    return str1;
}

QString ManDataMdb::getStrInclRot()
{
        QString str1 = "-";
        if (dataIncl.packTtl) {
            str1 = QString("%1").arg(dataIncl.rot);
        }
        return str1;
}

QString ManDataMdb::getStrInlTemp()
{
    QString str1 = "-";
    if (dataIncl.packTtl) {
        str1 = QString("%1°C").arg(dataIncl.temperature);
    }
    return str1;

}

QString ManDataMdb::getStrSysBatary()
{

    QString str1 = "-";
    if (dataSys.packTtl) {
        str1 = QString("%1B").arg(dataSys.vBat);
    }
    return str1;
}

QString ManDataMdb::getStrSysDisplay()
{
    QString str1 = "-";
    if (dataSys.packTtl) {

        str1 = (dataSys.display) ? "On" : "Off";
    }
    return str1;
}

QString ManDataMdb::getStrSysButton()
{
    QString str1 = "-";
    if (dataSys.packTtl) {

        str1 = QString("%1").arg(dataSys.button, 2, 16, QChar('0'));
    }
    return str1;
}


//---------------------------------------------
void ManDataMdb::getDataStr(QStringList *listData, int _dev)
{
    listData->clear();
    QString str1;
    switch (_dev) {
        case TYPE_DATA_RAW_SYS:
        //системные данные
//            listData listData->append( getStrSysBatary() listData->append( getStrSysButton() listData->append( getStrSysDisplay();
            listData->append(getStrSysBatary());
            listData->append(getStrSysButton());
            listData->append(getStrSysDisplay());
        break;
        case TYPE_DATA_RAW_GPS:
        //данные GPS
            listData->append(getStrGpsStatus());
            listData->append(getStrGpsLat());
            listData->append(getStrGpsLong());
            listData->append(getStrGpsDate());
            listData->append( getStrGpsTime());
        break;
        case TYPE_DATA_RAW_CLI:
        // данные компаса
            listData->append(getStrCliX());
            listData->append( getStrCliY());
            listData->append( getStrCliZ());
        break;
        case TYPE_DATA_RAW_LASER:
        // данные лазера (дальномер)
            listData->append( getStrLaserDist());
            listData->append( getStrLaserErr());
        break;
        case TYPE_DATA_RAW_ACCEL:
        // данные акселерометра
            listData->append(getStrAccelX());
            listData->append(getStrAccelY());
            listData->append(getStrAccelZ());
        break;
        case TYPE_DATA_RAW_INCL:
        // данные инклинометра
            listData->append(getStrInclX());
            listData->append(getStrInclY());
            listData->append( getStrInclRot());
        break;
    }
}


void ManDataMdb::getBldData(void *ptrData, int _dev, int _field)
{


}
