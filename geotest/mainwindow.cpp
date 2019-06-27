#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;



int getNumbObjFromString(QString str)
{
    int res = -1;
    QRegExp rx("_([0-9]+)$");
    if (rx.indexIn(str)>=0)
    {
        res = rx.cap(1).toInt();
    }
    return res;
}


void DrawButOnOff(QPushButton *but, bool state)
{
    if (but) {
        if (!state)
            but->setStyleSheet(("background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #4ce494,stop: 1 #128448); \
                                    border-style: outset; \
                                    border-width: 2px; \
                                    border-radius: 8px; \
                                    border-color: beige; \
                                    padding: 4px; \
                                    " ));

            else
                //but->setText("ON");
                but->setStyleSheet(("background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #e6e9ed,stop: 1 #84878a); \
                                                    border-style: outset; \
                                                    border-width: 2px; \
                                                    border-radius: 8px; \
                                                    border-color: beige; \
                                                    padding: 4px; \
                                                    " ));

//               but->update();
    }

}


struct descrParPack {
    quint8 dev;
    quint8 mask;
    int interval;
    int lineDraw;
};

const descrParPack TBL_DESCR_PACK[QUANT_TYPE_PACK_BLD-1] =
{
    {TYPE_DATA_RAW_SYS, MASK_DATA_RAW_SYS, 1000, 5},  //системные данные
    {TYPE_DATA_RAW_GPS, MASK_DATA_RAW_GPS, 1000, 0},//данные GPS
    {TYPE_DATA_RAW_CLI, MASK_DATA_RAW_CLI, 500, 1},// данные компаса
    {TYPE_DATA_RAW_LASER, MASK_DATA_RAW_LASER, 500, 2},   // данные лазера (дальномер)
    {TYPE_DATA_RAW_ACCEL, MASK_DATA_RAW_ACCEL, 500, 4}, // данные акселерометра
    {TYPE_DATA_RAW_INCL, MASK_DATA_RAW_INCL, 500, 3}    // данные инклинометра

};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
/*    bcm2835_init();
    bcm2835_gpio_fsel(RPI_GPIO_P1_11, BCM2835_GPI_bufReqO_FSEL_OUTP);
*/

#define COL_DEV_W 120
#define ROW_DEV_H 60


    ui->tblwTitle->setStyleSheet("padding-left: 2px;" );

    ui->tblwTitle->horizontalHeader()->resizeSection(0, 20);
    ui->tblwTitle->horizontalHeader()->resizeSection(1, 40);
    ui->tblwTitle->horizontalHeader()->resizeSection(2, 82);

    QStringList listHeadTbTest;
    listHeadTbTest << "GPS" << "Компас" << "Дальномер" << "Инклинометр" << "Акселерометр" << "Состояние";

    ui->tblwTitle->verticalHeader()->setFixedHeight(ROW_DEV_H*2);

    QCheckBox *ch1;
    MObjGraphFl *led1;
    QLabel *lb1;
    for (int i=0; i<6; i++) {
        ch1 = new QCheckBox(ui->tblwTitle);
        ch1->setObjectName(QString("ch_%1").arg(i));
        if (i==5) ch1->setChecked(true);
        ui->tblwTitle->setCellWidget(i, 0, ch1);
        vecCh.push_back(ch1);
        connect(ch1, SIGNAL(clicked()), this, SLOT(slotCheckedPack()));

        led1 = new MObjGraphFl(ui->tblwTitle, "", 20, 20);
        led1->setObjectName(QString("led_%1").arg(i));
        ui->tblwTitle->setCellWidget(i, 1, led1);
        vecLed.push_back(led1);

        lb1 = new QLabel(listHeadTbTest.at(i));
        ui->tblwTitle->setCellWidget(i, 2, lb1);
    }


//--------------------------
    ui->tblwState->setStyleSheet("padding-left: 2px;" );

    ui->tblwTitle->horizontalHeader()->setFixedHeight(120);
    ui->tblwTitle->verticalHeader()->setFixedHeight(ROW_DEV_H);
//--------------
// GPS
    lb1 = new QLabel("Статус");
    ui->tblwState->setCellWidget(0, 0, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(1, 0, lb1);

    lb1 = new QLabel("Широта");
    ui->tblwState->setCellWidget(0, 1, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(1, 1, lb1);

    lb1 = new QLabel("Долгота");
    ui->tblwState->setCellWidget(0, 2, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(1, 2, lb1);

    lb1 = new QLabel("Дата");
    ui->tblwState->setCellWidget(0, 3, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(1, 3, lb1);

    lb1 = new QLabel("Время");
    ui->tblwState->setCellWidget(0, 4, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(1, 4, lb1);

//---------------_bufReq
// Компас
    lb1 = new QLabel("X");
    ui->tblwState->setCellWidget(2, 0, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(3, 0, lb1);

    lb1 = new QLabel("Y");
    ui->tblwState->setCellWidget(2, 1, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(3, 1, lb1);

    lb1 = new QLabel("Z");
    ui->tblwState->setCellWidget(2, 2, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(3, 2, lb1);

//--------------------
// Дальномер
    lb1 = new QLabel("Расстояние, м");
    ui->tblwState->setCellWidget(4, 0, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(5, 0, lb1);

/*    lb1 = new QLabel("Расстояние, см");
    ui->tblwState->setCellWidget(4, 1, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(5, 1, lb1);
*/
    lb1 = new QLabel("Ошибка");
    ui->tblwState->setCellWidget(4, 1, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(5, 1, lb1);

//--------------------
// Инклинометр
    lb1 = new QLabel("X");
    ui->tblwState->setCellWidget(6, 0, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(7, 0, lb1);

    lb1 = new QLabel("Y");
    ui->tblwState->setCellWidget(6, 1, lb1);
    lb1 = new QLabel("-");

    ui->tblwState->setCellWidget(7, 1, lb1);

    lb1 = new QLabel("a°");
    ui->tblwState->setCellWidget(6, 2, lb1);
    lb1 = new QLabel("-");
    ui->tblwState->setCellWidget(7, 2, lb1);

//--------------------_bufReq
// Акселерометр
     lb1 = new QLabel("X");
     ui->tblwState->setCellWidget(8, 0, lb1);
     lb1 = new QLabel("-");
     ui->tblwState->setCellWidget(9, 0, lb1);

     lb1 = new QLabel("Y");
     ui->tblwState->setCellWidget(8, 1, lb1);
     lb1 = new QLabel("-");
     ui->tblwState->setCellWidget(9, 1, lb1);

     lb1 = new QLabel("Z");
     ui->tblwState->setCellWidget(8, 2, lb1);
     lb1 = new QLabel("-");
     ui->tblwState->setCellWidget(9, 2, lb1);
//------------------------
// Состояние
     lb1 = new QLabel("Батарея");
     ui->tblwState->setCellWidget(10, 0, lb1);
     lb1 = new QLabel("-");
     ui->tblwState->setCellWidget(11, 0, lb1);
     lb1 = new QLabel("Кнопка");
     ui->tblwState->setCellWidget(10, 1, lb1);
     lb1 = new QLabel("-");
     ui->tblwState->setCellWidget(11, 1, lb1);
     lb1 = new QLabel("Дисплей");
     ui->tblwState->setCellWidget(10, 2, lb1);
     lb1 = new QLabel("-");
     ui->tblwState->setCellWidget(11, 2, lb1);


//-------------------


     manDataMdb = new ManDataMdb();
     interfaceBld = new MainInterface();
     connect(interfaceBld, SIGNAL(signalPackResiev(QByteArray)), this, SLOT(slotHandRxBld(QByteArray)));
     connect(this, SIGNAL(signalSendBld(QByteArray)), interfaceBld, SLOT(slotSendReq(QByteArray)));
     connect(interfaceBld, SIGNAL(signalStarted()), this, SLOT(slotStartInterface()));

     serialThread = new QThread;
     interfaceBld->moveToThread(serialThread);
     connect(serialThread, SIGNAL(started()), interfaceBld, SLOT(startInterface()));
     serialThread->start();

     connect(ui->pbPack, SIGNAL(clicked()), this, SLOT(slotSendReq()));
     connect(ui->pbOff, SIGNAL(clicked()), this, SLOT(slotSendOff()));
     connect(ui->pbClose, SIGNAL(clicked()), this, SLOT(slotClose()));
     connect(ui->pbClose, SIGNAL(clicked()), interfaceBld, SLOT(closePort()));

     connect(ui->pbCamera,SIGNAL(clicked()), this, SLOT(slotCamera()));

     stateBld = MASK_DATA_RAW_SYS;
     cmBld = MASK_DATA_RAW_SYS;
     intervalPack = INTERVAL_REQ_SYS/100;

}


MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::slotStartInterface()
{
    QByteArray bufReq;
    bufReq.append(TYPE_DATA_RPI_GET);
    bufReq.append(cmBld);
    bufReq.append(intervalPack);
    emit signalSendBld(bufReq);
}


void MainWindow::slotHandRxBld(QByteArray _bufReq)
{
    stateBld = manDataMdb->handBldPack(_bufReq);
    QLabel *lb1;
    for (int i=0; i< QUANT_TYPE_PACK_BLD-1; i++) {
        int linedraw = TBL_DESCR_PACK[i].lineDraw;
        bool statePack = stateBld & TBL_DESCR_PACK[i].mask;

        MObjGraphFl *led1 = dynamic_cast<MObjGraphFl*>(ui->tblwTitle->cellWidget(linedraw, 1));
        if (led1) led1->setState(statePack);
// активные данные
        QStringList listData;
        if (statePack) manDataMdb->getDataStr(&listData, TBL_DESCR_PACK[i].dev);
//        for (int j=0; j<listData.size(); j++)
        for (int j=0; j<ui->tblwState->colorCount(); j++ )
        {
            lb1 = qobject_cast<QLabel*>(ui->tblwState->cellWidget(linedraw*2+1,j));
            if (lb1) {
                if (statePack) {
                    lb1->setStyleSheet("QLabel { color: green }");
                    lb1->setText(listData.at(j));
                }
                else {
                    lb1->setStyleSheet("QLabel { color: black }");
                }
            }
            else break;
        }

    }
}


void MainWindow::slotCheckedPack()
{
    QCheckBox *ch = qobject_cast<QCheckBox*>(sender());
    QString str1 = ch->objectName();
    // определяю номер
    int i = getNumbObjFromString(str1);
    if ((i>=0) && (i<(QUANT_TYPE_PACK_BLD-1))) {
        for (int j=0; j<QUANT_TYPE_PACK_BLD-1; j++ ) {
            if (TBL_DESCR_PACK[j].lineDraw == i) {
                i = j;
                break;
            }
        }
        quint8 mask = TBL_DESCR_PACK[i].mask;
        if (ch->checkState()) cmBld |= mask;
        else cmBld &= ~mask;
    }
 }

// запрос формирования пакета
void MainWindow::slotSendReq()
{
    int temp = ui->spbInterval->value();
    temp /= 100;
    intervalPack = temp;
    slotStartInterface();
}

void MainWindow::slotSendOff()
{
    QByteArray bufReq;
    bufReq.append(TYPE_DATA_RPI_OFF);
    emit signalSendBld(bufReq);

}

void MainWindow::slotCamera()
{
 //   VideoCapture cap(0);
 /*   raspicam::RaspiCam Camera; //Cmaera object
     //Open camera
     cout<<"Opening Camera..."<<endl;
     if ( !Camera.open()) {cerr<<"Error opening camera"<<endl;return;}
     //wait a while until camera stabilizes
     cout<<"Sleeping for 3 secs"<<endl;
     QThread::sleep(3);
     //capture
     Camera.grab();
     //allocate memory
     unsigned char *data=new unsigned char[  Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_RGB )];
     //extract the image in rgb format
     Camera.retrieve ( data,raspicam::RASPICAM_FORMAT_RGB );//get camera image
     //save
     std::ofstream outFile ( "raspicam_image.ppm",std::ios::binary );
     outFile<<"P6\n"<<Camera.getWidth() <<" "<<Camera.getHeight() <<" 255\n";
     outFile.write ( ( char* ) data, Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_RGB ) );
     cout<<"Image saved at raspicam_image.ppm"<<endl;
     //free resrources
     delete data;
*/}

void MainWindow::slotClose()
{
    serialThread->exit();
    close();

}
