/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MASTERTHREAD_H
#define MASTERTHREAD_H

#include <QMutex>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <qdebug.h>
#include <QQueue>
#include "mdbdata.h"


#define ST_CON_NO              0x00
#define ST_CON_OK              0x01 // транзакция успешеа
#define ST_CON_NO_WRITE        -1
#define ST_CON_ERR_NO_ANSW     -2 // нет ответа
#define ST_CON_ERR_CRC         -3  // ошибка CRC
#define ST_CON_ERR_PORT        -4

#define ST_PORT_ERR            -1
#define ST_PORT_NO             0
#define ST_PORT_OPEN           1

#define SER_REPEAT_SEND        3 // кол-во повторений в ожидании ответа


//--------------------------------
// адреса абонентов
#define ADDR_UART_RPI           1    // адрес RPI на интерфейсе UART1
#define ADDR_UART_BLD           2    // адрес платы device
//-----------------------

#define INTERVAL_WAIT_RX        3000   // 3 сек

#define UART_ERR_TX     0x01U
#define UART_ERR_RX     0x02U

struct tCfgSerial {
    QString name;
    quint32 baud;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
};


#define UART_PH_NONE 0  // нет данных для передачи
#define UART_PH_SEND 1  // данные переданы
#define UART_PH_ANSW 2  // получен ответ

class MainInterface : public QObject
{
    Q_OBJECT
public:
    MainInterface();
    ~MainInterface();
private:
    QSerialPort *serial;
    QTimer *timerWdtConnect; // таймер определения обрыва связи
    QTimer *timerWdtRead;
protected:
    tCfgSerial cfg;
    int statePort;      // состояние порта
    int codeErr;        // код ошибки
    int countRx;      // кол-во полученных пакетов
    int phasaTx;     // фаза передачи передачи

    QMutex mutexUart;
    QByteArray bufReq;  // запрос от приложения для передачи
    QByteArray buffTx;  // буфер передачи
    QByteArray buffRx;   // буфер приема
private slots:
    void slotTimeOutRx();   // слот обработки тайаута приема (нет ничего, передаю запрос)
    void slotReadData();    // слот приема данных
    void slotTimeoutRead();

public slots:
    void startInterface();
    void closePort();
    void slotSendReq(QByteArray _req);//передать команду
signals:
    void signalStarted();
    void signalPackResiev(QByteArray);

};



#endif // MASTERTHREAD_H
