#ifndef COMMON_NAME_H
#define COMMON_NAME_H

#define __DEBUG

#pragma pack(push,1)

// общее состояние
struct TStateSysRaw {
//    unsigned char state;
    unsigned char vBatHi;
    unsigned char vBatLo;
    unsigned char button;
    unsigned char display;
};


// состояние GPS по UART (18 байт)
struct TGpsRaw {
// время
    unsigned char timeH;
    unsigned char timeM;
    unsigned char timeS;
// статус
    unsigned char status; // 'A'-активный | 'V' - игнорировать
// широта
    unsigned char latD; // градусы
    unsigned char latM; // минуты
    unsigned char latM_1; // минуты * 0.1
    unsigned char latM_01; //минуты * 0.01
    unsigned char latType; // тип широты: 'N'(+) - северная, 'S'(-) - южная
// долгота
    unsigned char longD; // градусы
    unsigned char longM; // минуты
    unsigned char londM_1; // минуты * 0.1
    unsigned char longM_01; //минуты * 0.01
    unsigned char longType; // тип долготы: 'E'(+) - восточная, 'W'(-) - западная
//дата
    unsigned char dateD; // день
    unsigned char dateM; //месяц
    unsigned char dateY; // год
};

// состояние магнитометра (компаса)
struct TCliRaw {
    // координата X (Hi LO)
    unsigned char coordXHi;
    unsigned char coordXLo;
    // координата Y (Hi Lo)
    unsigned char coordYHi;
    unsigned char coordYLo;
    // координата Z (Hi Lo)
    unsigned char coordZHi;
    unsigned char coordZLo;
};

// состояние дальномера
struct TLaserRaw {
 //   unsigned char byteNone;
    unsigned char err;
    unsigned char distM; // расстояние в м
    unsigned char distSm; // расстояние см
};

// состояние акселерометр (гироскоп) MPU-9255
struct TAccelRaw {
    unsigned char hXHi;
    unsigned char hXLo;
    unsigned char hYHi;
    unsigned char hYLo;
    unsigned char hZHi;
    unsigned char hZLo;
};

// состояние инклинометра ADIS16209
struct TInclRaw {
    unsigned char acclXHi;
    unsigned char acclXLo;
    unsigned char acclYHi;
    unsigned char acclYLo;
    unsigned char tempHi;
    unsigned char tempLo;
    unsigned char inclXHi;
    unsigned char inclXlo;
    unsigned char inclYHi;
    unsigned char inclYlo;
    unsigned char rotHi;
    unsigned char rotLo;
};
struct TButEvent {
    unsigned char evBut;
};

#pragma pack(pop)

//------------------------------
// типы данных в пакете
#define TYPE_DATA_RAW_SYS   0x01    //системные данные
#define TYPE_DATA_RAW_GPS   0x02    //данные GPS
#define TYPE_DATA_RAW_CLI   0x03    // данные компаса
#define TYPE_DATA_RAW_LASER 0x04    // данные лазера (дальномер)
#define TYPE_DATA_RAW_ACCEL 0x05    // данные акселерометра
#define TYPE_DATA_RAW_INCL  0x06    // данные инклинометра

#define TYPE_DATA_EV_BUT    0x10    // нажата кнопка (500мкс<time<2сек)
#define TYPE_DATA_EV_BUT_L1 0x11    // нажата кнопка долго (3сек<time)

#define TYPE_DATA_RPI_GET   0x81   // запрос данных от RPI
#define TYPE_DATA_RPI_OFF   0x82   // запрос RPI "выключить питание"

#define QUANT_TYPE_PACK_BLD 7

//---------------------------------
// маски состояния пакетов  и запросов данных от Rpi
#define MASK_DATA_RAW_SYS   0x01    //системные данные
#define MASK_DATA_RAW_GPS   0x02    //данные GPS
#define MASK_DATA_RAW_CLI   0x04    // данные компаса
#define MASK_DATA_RAW_LASER 0x08    // данные лазера (дальномер)
#define MASK_DATA_RAW_ACCEL 0x10    // данные акселерометра
#define MASK_DATA_RAW_INCL  0x20    // данные инклинометра

#define MASK_DATA_EV_BUT    0x40    // нажата кнопка (500мкс<time<2сек)

//-----------------------
// вычисление контрольной суммы
/*unsigned char getCrcEasy(QByteArray &buf)
{
    unsigned char temp = 0;
    for (int i=0; i< buf.length(); i++)
        temp += (unsigned char)buf[i];
    return(0-temp);
}
*/



#endif // COMMON_NAME_H

