/****************************************************************************

****************************************************************************/
#include "mserial.h"
//#include "mparametr.h"

#include <qdebug.h>




quint8 getCrcEasy(QByteArray &buf)
{
    quint8 temp = 0;
    for (int i=0; i< buf.length(); i++)
        temp += (quint8)buf[i];
    return(0-temp);
}

const tCfgSerial CFG_UART_BLD = {
    "ttyS0",
    QSerialPort::Baud115200,
    QSerialPort::Data8,
    QSerialPort::NoParity,
    QSerialPort::OneStop
};


MainInterface::MainInterface()
{
    statePort = ST_PORT_NO;
    countRx = 0;
    codeErr = 0;
    phasaTx = UART_PH_NONE;
    bufReq.clear();
    buffRx.clear();
    buffTx.clear();
}

void MainInterface::startInterface()
{
    statePort = ST_PORT_NO;
    serial = new QSerialPort();
    timerWdtConnect = new QTimer();
    timerWdtRead = new QTimer();
    connect(timerWdtRead, SIGNAL(timeout()), this, SLOT(slotTimeoutRead()));
    serial->setPortName(CFG_UART_BLD.name);
    if (serial->open(QIODevice::ReadWrite)) {
           if (serial->setBaudRate(CFG_UART_BLD.baud)
                    && serial->setDataBits(CFG_UART_BLD.dataBits)
                    && serial->setParity(CFG_UART_BLD.parity)
                    && serial->setStopBits(CFG_UART_BLD.stopBits)
                    && serial->setFlowControl(QSerialPort::NoFlowControl))
            {
                if (serial->isOpen()){
                    statePort = ST_PORT_OPEN;
                    qDebug() << "--Port" << cfg.name << " open ---";
                    connect(serial, SIGNAL(readyRead()), this, SLOT(slotReadData()));
                }
            } else {
  //              error_(serial.errorString().toLocal8Bit());
               statePort = ST_PORT_ERR;
               serial->close();
              }
        } else {
            statePort = ST_PORT_ERR;
            serial->close();
    }
    connect(timerWdtConnect, SIGNAL(timeout()), this, SLOT(slotTimeOutRx()));
    emit signalStarted();
//    timerWdtConnect->start(INTERVAL_WAIT_RX);ttyS0
}

MainInterface::~MainInterface()
{
}



void MainInterface::closePort()
{
   QMutexLocker lock(&mutexUart);
   serial->close();
}


// слот обработки тайаута приема (нет ничего, передаю запрос)
void MainInterface::slotTimeOutRx()
{
    timerWdtConnect->stop();
    if (statePort == ST_PORT_OPEN) {


//      serial.clear();
      // нет приема или есть новые данные для передачи
        if (countRx==0 || phasaTx==UART_PH_SEND) {
            if (buffTx.length()){
                serial->write(buffTx);
     // ожидаю завершения записи
                qDebug() << buffTx.length() << ":" << buffTx.toHex();
                if (serial->waitForBytesWritten(2)) {
                    phasaTx = UART_PH_NONE;
                    codeErr &= ~UART_ERR_TX;
                }
            }
        }
    }
    countRx = 0;
    timerWdtConnect->start(INTERVAL_WAIT_RX);
}


//----------------------------------slotTimeOutRx
// отправка запроса
void MainInterface::slotSendReq(QByteArray _req)
{
    mutexUart.lock();
    buffTx.clear();
    buffTx[0] = ADDR_UART_BLD;
    buffTx[1] = _req.length()+3;
    buffTx.append(_req);
    buffTx.append(getCrcEasy(buffTx));
    phasaTx = UART_PH_SEND;
    mutexUart.unlock();
    slotTimeOutRx();
}


//-------------------------------------------
//прием в ответ на сигнал readyRead
void MainInterface::slotReadData()
{
    QMutexLocker lock(&mutexUart);
    buffRx += serial->readAll();
    if (buffRx.length() > 1000) buffRx.clear();
    timerWdtRead->stop();
    timerWdtRead->start(10);
}

void MainInterface::slotTimeoutRead()
{
    timerWdtRead->stop();
    QMutexLocker lock(&mutexUart);
    if (buffRx.length() >= 4)
    {
#ifdef __DEBUG
   qDebug() << buffRx.length() << ":" << buffRx.toHex();
#endif
        if ((buffRx.at(0) == ADDR_UART_RPI) && (buffRx.at(1) == buffRx.length()) && (getCrcEasy(buffRx)==0) ) {
            countRx++;
            buffRx.remove(0,2);
            buffRx.remove(buffRx.length()-1,1);
            emit signalPackResiev(buffRx);
#ifdef __DEBUG
    qDebug() << "Rx ok!";
#endif
        }
        else {
#ifdef __DEBUG
    qDebug() << "Rx Err!";
#endif

        }
     }
    buffRx.clear();

}
