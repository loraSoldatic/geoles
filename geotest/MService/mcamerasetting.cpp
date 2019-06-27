#include "mcamerasetting.h"
#include <linux/i2c.h>
#include "mservice.h"

MCamera::MCamera(QWidget *parent, QColor colorBack)
{
    const TcfgCamera CFG_CAMERA_DEF = {
        CAMERA_EI_KBD + CAMERA_EI_REVERSE, // разрешение по заднему ходу + кнопка
        100,  // смещение слева
        0, // смещение сверху
        360, // ширина
        480, // высота
        CAM_ROT_90_FLIP_HV, // поворот на 90 + отразить по H и V
        //-----------------------
        // Widget для настройки
        590, // смещение слева
        270, // смещение сверху
        210, //
        210
    };



    cfgCamera = CFG_CAMERA_DEF;

/*       QFile file(FILE_CFG_CAMERA);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        QString strCfgPanel = in.readAll();
        int tempI;

        //--------------------------------------------------
        QString str1;
        // разрешение включения камеры по заднему ходу
        int i  = getValueIntFromStr(&strCfgPanel, "<Enable Reverse>", &tempI);
        if (i>=0) {
            if (tempI > 0) cfgCamera.ei |= CAMERA_EI_REVERSE;
            else cfgCamera.ei &= ~CAMERA_EI_REVERSE;
        }
        // разрешение включения камеры по кнопке
         i  = getValueIntFromStr(&strCfgPanel, "<Enable Button>", &tempI);
        if (i>=0) {
            if (tempI > 0) cfgCamera.ei |= CAMERA_EI_KBD;
            else cfgCamera.ei &= ~CAMERA_EI_KBD;
         }
        //смещение слева
        i  = getValueIntFromStr(&strCfgPanel, "<Left>", &tempI);
        if (i>=0) cfgCamera.left = tempI;
        // смещение сверху
        i  = getValueIntFromStr(&strCfgPanel, "<Top>", &tempI);
        if (i>=0) cfgCamera.top = tempI;
        // ширина
        i  = getValueIntFromStr(&strCfgPanel, "<Width>", &tempI);
        if (i>=0) cfgCamera.widht = tempI;
        // высота
        i  = getValueIntFromStr(&strCfgPanel, "<Height>", &tempI);
        if (i>=0) cfgCamera.height = tempI;
        // поворот на 90
        i  = getValueIntFromStr(&strCfgPanel, "<Rotate 90>", &tempI);
        if (i>=0) cfgCamera.rotate = (tempI>0) ? CAM_ROT_90_FLIP_NO : CAM_ROT_NO_FLIP_NO;
        // отразить по H
        i  = getValueIntFromStr(&strCfgPanel, "<Flip horizontal>", &tempI);
        if (i>=0) cfgCamera.rotate += CAM_ROT_NO_FLIP_H;
        // отразить по V
        i  = getValueIntFromStr(&strCfgPanel, "<Flip vertical>", &tempI);
        if (i>=0) cfgCamera.rotate += CAM_ROT_NO_FLIP_V;
        //------------------------------------
        // виджет настроек
        i  = getValueIntFromStr(&strCfgPanel, "<SettingLeft>", &tempI);
        if (i>=0) cfgCamera.setLeft = tempI;
        // смещение сверху
        i  = getValueIntFromStr(&strCfgPanel, "<SettingTop>", &tempI);
        if (i>=0) cfgCamera.setTop = tempI;
        // ширина
        i  = getValueIntFromStr(&strCfgPanel, "<SettingWidth>", &tempI);
        if (i>=0) cfgCamera.setWidht = tempI;
        // высота
        i  = getValueIntFromStr(&strCfgPanel, "<SettingHeight>", &tempI);
        if (i>=0) cfgCamera.setHeight = tempI;

        file.close();
    }
*/    pipeline = QString(CAMERA_PIPELINE);/*
                            .arg(cfgCamera.left)
                            .arg(cfgCamera.top)
                            .arg(cfgCamera.widht)
                            .arg(cfgCamera.height)
                            .arg(cfgCamera.rotate);*/
#ifdef _QDEBUG
    qDebug() << pipeline;
    qDebug() << "Camera EI = " << cfgCamera.ei;
    qDebug() << "Camera Left = " << cfgCamera.left;
    qDebug() << "Camera Top = " << cfgCamera.top;
    qDebug() << "Camera W = " << cfgCamera.widht;
    qDebug() << "Camera H" << cfgCamera.height;
    qDebug() << "Camera Rotate = " << QString::number(cfgCamera.rotate, 16);
#endif
//---------------------
    lbCamera = new QLabel(parent);
    lbCamera->setGeometry(cfgCamera.left, cfgCamera.top, cfgCamera.widht, cfgCamera.height);
    pxCamera = new QPixmap(cfgCamera.widht, cfgCamera.height);
    pxCamera->fill(colorBack.rgb());
    lbCamera->setPixmap(*pxCamera);//QPixmap::fromImage(mGreet));
    lbCamera->hide();


//-------------------------------------------------------
        //настройки ADV7180
    // чтение файла ручных установок
 /*   file.setFileName(FILE_CFG_CAMERA_SAVE_SETING);//"cameraSetMan.cfg");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> cfgAdc.contrast >> cfgAdc.brightness >> cfgAdc.saturationB >> cfgAdc.saturationR >> cfgAdc.hue >> cfgAdc.eiCameraManual;
        file.close();
    }
    if ((cfgAdc.eiCameraManual & 0xffff0000) == CAMERA_EI_ATTR) cfgCamera.ei = cfgAdc.eiCameraManual & 0xff;
*/
/*    QRect rect = QRect(cfgCamera.setLeft, cfgCamera.setTop, cfgCamera.setWidht, cfgCamera.setHeight);
    grSetting = new QGroupBox(parent);
    grSetting->setGeometry(rect);

    lbSetting = new QLabel(grSetting);
    lbSetting->setGeometry(1, 1, rect.width()-2, rect.height()-2);
    lbSetting->setStyleSheet("border-color: white;"
                                "border-style: solid;"
                                "border-width: 1px;"
                                "border-radius: 1px;"
                                );

    pxSetting = new QPixmap(rect.width()-4, rect.height()-4);
    pxSetting->fill(QColor(0x80, 0x80, 0x80, 210));//Qt::darkGray);//colorBack.rgb());
    lbSetting->setPixmap(*pxSetting);

//    laySetCamera = new QStackedLayout(grSetting);//this);//grSetCamera);

    QColor colorLabel = QColor(Qt::white);//lightGray);//QColor(Qt::yellow);
    QFont font("DejaVu Sans", 11, QFont::Light);
#define SL_LEFT     20
#define SL_TOP      5
#define SL_WIDTH    20
#define SL_HEIGHT   (cfgCamera.setHeight-10)

#define MT_LEFT     5
#define MT_TOP      5
#define MT_WIDTH    (cfgCamera.setHeight-10)
#define MT_HEIGHT   (cfgCamera.setHeight-10)
#define DELTA_L     40

    //    mtContrast = new QLabel(grSetting);

    //    mtContrast->setGeometry();
    mtContrast = new MLabel(grSetting);
    mtContrast->setCfg(QRect(MT_LEFT, MT_TOP, MT_WIDTH, MT_HEIGHT), colorLabel, font, "Контрастность");
    slContrast = new QSlider(grSetting); // настройка контрастности
    slContrast->setOrientation(Qt::Vertical);//Qt::Horizontal);
    slContrast->setGeometry(SL_LEFT, SL_TOP, SL_WIDTH, SL_HEIGHT);
    slContrast->setMinimum(0);    cfgAdc.eiCameraManual = 0;

    slContrast->setMaximum(MAX_CONTRAST);
    slContrast->setValue(cfgAdc.contrast);
    slContrast->setTracking(true);

    //    slContrast->setPageStep(1);
//    laySetCamera->addWidget(slContrast);    cfgAdc.eiCameraManual = 0;

    TBL_REG_SET_COLOR[0].slider = slContrast;
    TBL_REG_SET_COLOR[0].addrCfg = &cfgAdc.contrast;
    TBL_REG_SET_COLOR[0].labSl = mtContrast;

    mtBright = new MLabel(grSetting);
    mtBright->setCfg(QRect(MT_LEFT+DELTA_L, MT_TOP, MT_WIDTH, MT_HEIGHT), colorLabel, font, "Яркость");
    slBright = new QSlider(grSetting); // настройка яркости
    slBright->setOrientation(Qt::Vertical);//(Qt::Horizontal);
    slBright->setGeometry(SL_LEFT+DELTA_L, SL_TOP, SL_WIDTH, SL_HEIGHT);
    slBright->setMaximum(MAX_Brightness);
    slBright->setMinimum(0);
    slBright->setValue(cfgAdc.brightness);
//    laySetCamera->addWidget(slBright);
    TBL_REG_SET_COLOR[1].slider = slBright;
    TBL_REG_SET_COLOR[1].addrCfg = &cfgAdc.brightness;    cfgAdc.eiCameraManual = 0;

    TBL_REG_SET_COLOR[1].labSl = mtBright;

    mtSaturB = new MLabel(grSetting);
//    mtSaturB->setCfg(QRect(110, 5, 180, 180), colorLabel, font, "Насыщенность синего");
    mtSaturB->setCfg(QRect(MT_LEFT+DELTA_L*2, MT_TOP, MT_WIDTH, MT_HEIGHT), colorLabel, font, "Насыщенность синего");

    slSaturB = new QSlider(grSetting); // насыщенность
    slSaturB->setOrientation(Qt::Vertical);//(Qt::Horizontal);
//    slSaturB->setGeometry(120, 5, 20, 180);
    slSaturB->setGeometry(SL_LEFT+DELTA_L*2, SL_TOP, SL_WIDTH, SL_HEIGHT);
    slSaturB->setMaximum(MAX_SATURATION);
    slSaturB->setMinimum(0);
    slSaturB->setValue(cfgAdc.saturationB);
//    laySetCamera->addWidget(slSaturB);
    TBL_REG_SET_COLOR[2].slider = slSaturB;
    TBL_REG_SET_COLOR[2].addrCfg = &cfgAdc.saturationB;
    TBL_REG_SET_COLOR[2].labSl = mtSaturB;

    mtSaturR = new MLabel(grSetting);
//    mtSaturR->setCfg(QRect(160, 5, 180, 180), colorLabel, font, "Насыщенность красного");
    mtSaturR->setCfg(QRect(MT_LEFT+DELTA_L*3, MT_TOP, MT_WIDTH, MT_HEIGHT), colorLabel, font, "Насыщенность красного");

    slSaturR = new QSlider(grSetting); // насыщенность
    slSaturR->setOrientation(Qt::Vertical);//(Qt::Horizontal);
//    slSaturR->setGeometry(170, 5, 20, 180);
    slSaturR->setGeometry(SL_LEFT+DELTA_L*3, SL_TOP, SL_WIDTH, SL_HEIGHT);
    slSaturR->setMaximum(MAX_SATURATION);
    slSaturR->setMinimum(0);
    slSaturR->setValue(cfgAdc.saturationR);
//    laySetCamera->addWidget(slSaturR);
    TBL_REG_SET_COLOR[3].slider = slSaturR;
    TBL_REG_SET_COLOR[3].addrCfg = &cfgAdc.saturationR;
    TBL_REG_SET_COLOR[3].labSl = mtSaturR;

    mtHue = new MLabel(grSetting);
//    mtHue->setCfg(QRect(210, 5, 180, 180), colorLabel, font, "Оттенок");
    mtHue->setCfg(QRect(MT_LEFT+DELTA_L*4, MT_TOP, MT_WIDTH, MT_HEIGHT), colorLabel, font, "Оттенок");

    slHue = new QSlider(grSetting); // оттенок
    slHue->setOrientation(Qt::Vertical);//(Qt::Horizontal);
//    slHue->setGeometry(220, 5, 20, 180);    cfgAdc.eiCameraManual = 0;

    slHue->setGeometry(SL_LEFT+DELTA_L*4, SL_TOP, SL_WIDTH, SL_HEIGHT);
    slHue->setMaximum(MAX_HUE);
    slHue->setMinimum(0);
    slHue->setValue(cfgAdc.hue);
//    laySetCamera->addWidget(slHue);
    TBL_REG_SET_COLOR[4].slider = slHue;
    TBL_REG_SET_COLOR[4].addrCfg = &cfgAdc.hue;
    TBL_REG_SET_COLOR[4].labSl = mtHue;

    grSetting->setVisible(false);
 //   grSetting->setVisible(true);
    manDataMdb->setStateDin(Dalg_SetCam_Visible, ST_DIN_OFF);
    stateCamera = false;
    eventState = CAMERA_EI_NO;// свободна
    activeItem = -1;
    timerDetect = new QTimer(this);
    connect(timerDetect, SIGNAL(timeout()), this, SLOT(slotEndDetect()));

    slotSetVideoIn(cfgAdc.stdVideoIn);
    slotCameraDetect(10000);
#ifdef _QDEBUG
    qDebug() << "Video in = " << QString::number(cfgAdc.stdVideoIn, 16);
    qDebug() << "ADV7180 Contrast = " << QString::number(cfgAdc.contrast, 16);
    qDebug() << "ADV7180 Brightness = " << QString::number(cfgAdc.brightness, 16);
    qDebug() << "ADV7180 Saturation B = " << QString::number(cfgAdc.saturationB, 16);
    qDebug() << "ADV7180 Saturatuin R" << QString::number(cfgAdc.saturationR, 16);
    qDebug() << "ADV7180 Hue" << QString::number(cfgAdc.hue, 16);
#endif
    setParamAdv7180(0, cfgAdc.contrast);
    setParamAdv7180(1, cfgAdc.brightness);
    setParamAdv7180(2, cfgAdc.saturationB);
    setParamAdv7180(3, cfgAdc.saturationR);
    setParamAdv7180(4, cfgAdc.hue);
    binCamera = NULL;
}


void MCamera::slotEndDetect()
{
    manDataMdb->setStateDin(Dalg_CamGray, ST_DIN_OFF);
    manDataMdb->setStateDin(DalgCamYelloy, ST_DIN_OFF);
    manDataMdb->setStateDin(DalgCamGreen, ST_DIN_OFF);
    manDataMdb->setStateDin(DalgCamRed, ST_DIN_OFF);    cfgAdc.eiCameraManual = 0;

    timerDetect->stop();
 //   delete timerDetect;

}

/*MCamera::~MCamera()
{
    slotCameraStop();
}

*/


//---------------------------------------------
//установка языковых параметров
void MCamera::setDescrMap()
{
    QString str1;
    bool ok = getStrFromStr(&langParametr, "<MCamera>",  "</MCamera>", &str1);
    if (ok) {

        QRegExp regPar("<mtContrast>([^\r\n]*)");
        if (regPar.indexIn(str1) >= 0)
            mtContrast->setTextLabel(regPar.cap(1));
        regPar = QRegExp("<mtBright>([^\r\n]*)");
        if (regPar.indexIn(str1) >= 0)
            mtBright->setTextLabel(regPar.cap(1));
        regPar = QRegExp("<mtSaturB>([^\r\n]*)");
        if (regPar.indexIn(str1) >= 0)
            mtSaturB->setTextLabel(regPar.cap(1));
        regPar = QRegExp("<mtSaturR>([^\r\n]*)");
        if (regPar.indexIn(str1) >= 0)
            mtSaturR->setTextLabel(regPar.cap(1));
        regPar = QRegExp("<mtHue>([^\r\n]*)");
        if (regPar.indexIn(str1) >= 0)
            mtHue->setTextLabel(regPar.cap(1));
    }
}


//----------------------------------------------------

void MCamera::setParamAdv7180(int item, int value)
{
    if ((item>=0) && (item<PARAM_ADV7180_QUANT)) {
        if (value < TBL_REG_SET_COLOR[item].min) value = TBL_REG_SET_COLOR[item].min;
        else
            if (value > TBL_REG_SET_COLOR[item].max) value = TBL_REG_SET_COLOR[item].max;
        quint8 tempSet = (quint8)value;
        tempSet += TBL_REG_SET_COLOR[item].ofs;
        QString str1 = QString("i2cset -f -y 0 0x21 0x%1 0x%2").arg(TBL_REG_SET_COLOR[item].addr, 2, 16, QLatin1Char('0')).arg(tempSet, 2, 16, QLatin1Char('0'));
        QProcess *proc = new QProcess();
        if (proc) {
            proc->start(str1);
            if (proc->waitForStarted()) {
                while (proc->waitForFinished());
                proc->close();
                TBL_REG_SET_COLOR[item].slider->setValue(value);
//                TBL_REG_SET_COLOR[item].slider->update();
            }
            delete proc;
        }
    }
}

void MCamera::slotSetParamAdv7180(int item, int add)
{
    int tempI = 0;
//    QPalette sliderPalette;
     QSlider *tmpS;
     MLabel *tmpL;
    if ((activeItem >=0) && (item != activeItem))
    {
        tmpS = TBL_REG_SET_COLOR[activeItem].slider;
        tmpS->setStyleSheet ("QSlider::handle:vertical {background:QColor(0xc0,0xc0,0xc0)}");
        tmpL = TBL_REG_SET_COLOR[activeItem].labSl;
        tmpL->setColorText(QColor(Qt::lightGray));

    }
    if ((item>=0) &&  (item < QUANT_ITEM_SET_ADV7180)) {
        //if (item != activeItem)
        {
            tmpS = TBL_REG_SET_COLOR[item].slider;
            tmpS->setStyleSheet ("QSlider::handle:vertical {background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #fdff72, stop: 1 #ffff00);}");
            tmpL = TBL_REG_SET_COLOR[item].labSl;
            tmpL->setColorText(QColor(Qt::yellow));
            activeItem = item;
        }
        tempI = TBL_REG_SET_COLOR[item].slider->value();
//        laySetCamera->setCurrentWidget(TBL_REG_SET_COLOR[item].slider);
        TBL_REG_SET_COLOR[item].slider->setFocusPolicy(Qt::TabFocus);

        tempI += add*TBL_REG_SET_COLOR[item].step;
        if (tempI < TBL_REG_SET_COLOR[item].min) tempI = TBL_REG_SET_COLOR[item].min;
        else
            if (tempI > TBL_REG_SET_COLOR[item].max) tempI = TBL_REG_SET_COLOR[item].max;
        if (add != 0)
            setParamAdv7180(item, tempI);
    }
#ifdef _QDEBUG

    qDebug() << "Set ADV7180" << QString::number(item, 16) << tempI;
#endif
}




//------------------------------------------
void MCamera::slotCameraDetect(quint32 timeDraw)
{
    if (manDataMdb->getStateDin(Dalg_Cam_Play) != ST_DIN_ON)
    {
    quint8 status;
    quint8 stGray, stYell, stGreen, stRed;
    stGray = ST_DIN_OFF;
    stYell = ST_DIN_OFF;
    stGreen = ST_DIN_OFF;
    stRed = ST_DIN_ON;
    if (getStatusAdc(&status) == 0)
    {
        manDataMdb->setStateAin(Aalg_ADN7180_autodet, status);
        quint8 temp8 = ((status & 0x0f) == 0x07) ? ST_DIN_ON : ST_DIN_OFF;
/*        if ((temp8==ST_DIN_ON) && (manDataMdb->getStateDin(Dalg_CamDetect) != ST_DIN_ON)) {
            slotSetVideoIn(cfgAdc.stdVideoIn);
            setParamAdv7180(0, cfgAdc.contrast);
            setParamAdv7180(1, cfgAdc.brightness);
            setParamAdv7180(2, cfgAdc.saturationB);
            setParamAdv7180(3, cfgAdc.saturationR);
            setParamAdv7180(4, cfgAdc.hue);

        }
        */
        manDataMdb->setStateDin(Dalg_CamDetect, temp8);
        if (temp8 == ST_DIN_ON) {
            stRed = ST_DIN_OFF;
            if (cfgCamera.ei & CAMERA_EI_REVERSE) stGreen = ST_DIN_ON;
            else {
                if (cfgCamera.ei & CAMERA_EI_KBD) stYell = ST_DIN_ON;
                else stGray = ST_DIN_ON;
            }
        }
#ifdef _QDEBUG

        qDebug() << "ADV7180 0x10 = " << QString::number(temp8, 16);
#endif
    }
    manDataMdb->setStateDin(Dalg_CamGray, stGray);
    manDataMdb->setStateDin(DalgCamYelloy, stYell);
    manDataMdb->setStateDin(DalgCamGreen, stGreen);
    manDataMdb->setStateDin(DalgCamRed, stRed);
    timerDetect->start(timeDraw);

    }
}

//----------------------------------------
// установка типа видеовхода
void MCamera::slotSetVideoIn(int std)
{
    if (std < QUANT_SW_VIDEO_IN)  {
        QProcess *proc = new QProcess(this);
        if (proc){
            QString str1 = QString("i2cset -f -y 0 0x21 0x00 0x%1").arg(getCodeVideoIn(std), 2, 16, QLatin1Char('0'));
            proc->start(str1);
            if (proc->waitForStarted())
                while (proc->waitForFinished());
            proc->close();

            delete proc;
        }

     }
}
//------------------------------------
//выключить камеру
void MCamera::cameraOff()
{
    if (binCamera) {
      gst_element_set_state (binCamera, GST_STATE_NULL);
      gst_object_unref (binCamera);
    }
    if (lbCamera)
        lbCamera->hide();
    stateCamera = false;
    manDataMdb->setStateDin(Dalg_Cam_Play, ST_DIN_OFF);
    slotSaveSetAdv7180_Off(false);
#ifdef _QDEBUG
    qDebug() << "Camera = " << manDataMdb->getStateDin(Dalg_Cam_Play);
#endif
}

//-------------------------------------
// включить камеру
void MCamera::slotCameraPlay(quint8 event)
{
   bool res = false;
   if ((manDataMdb->getStateDin(Dalg_CamDetect)==ST_DIN_ON)  && ((cfgCamera.ei & event) || (event == CAMERA_EI_ALL)))
   {
       GstElement *sink;
       gst_init (NULL, NULL);
       GError *error = NULL;
       GstStateChangeReturn ret;
#ifdef _QDEBUG
       qDebug() << "Camera ON" << pipeline;
#endif
       const gchar *pipeCam =  pipeline.toUtf8();
       binCamera = (GstElement *)gst_parse_launch (pipeCam, &error);
       if (binCamera) {
           sink = gst_bin_get_by_name (GST_BIN (binCamera), "my_sink");
           if (sink) {
               ret = gst_element_set_state (binCamera, GST_STATE_PLAYING);
               if (ret != GST_STATE_CHANGE_FAILURE){
                   res = true;
                   lbCamera->show();
               }  else {
#ifdef _QDEBUG
                   qDebug() << "Error gStreamer: No my play";
#endif

                   gst_object_unref (binCamera);
               }
           }
           else {
#ifdef _QDEBUG
               qDebug() << "Error gStreamer: No my sink";

#endif
               gst_object_unref (binCamera);
           }
       } else {
#ifdef _QDEBUG
           qDebug() << "Error gStreamer: No my bin";
#endif
       }
    }   else cameraOff() ;
    stateCamera = res;
    quint8 temp8 = (res) ? ST_DIN_ON  : ST_DIN_OFF;
    manDataMdb->setStateDin(Dalg_Cam_Play, temp8);
#ifdef _QDEBUG
    qDebug() << "Camera = " << manDataMdb->getStateDin(Dalg_Cam_Play);
#endif
}


//-----------------------------------------------
// переключить состояние камеры по кнопке
void MCamera::slotCameraKbd(quint8 state)
{
    if (cfgCamera.ei & CAMERA_EI_KBD)
    {
        if (state == ST_DIN_ON) {
            if (!stateCamera) {
                slotCameraPlay(CAMERA_EI_ALL);

            }

        }
        else {
            if (stateCamera /*&& (eventState == CAMERA_EI_KBD)*/ ){
                cameraOff();
            }
        }
        eventState = CAMERA_EI_KBD;
    }

}

//--------------------------------------
// переключить состояние (по реверсу: выключить только если не была включена по кнопке)
void MCamera::slotCameraReverse(quint8 state)
{
    if (cfgCamera.ei & CAMERA_EI_REVERSE)
    {
        if (state == ST_DIN_ON) {
            if (!stateCamera) {
                slotCameraPlay(CAMERA_EI_ALL);
                eventState = CAMERA_EI_REVERSE;
            }
        }
        else {
            if (stateCamera && (eventState == CAMERA_EI_REVERSE) ){
                cameraOff();
            }
        }
    }
}

//--------------------------------------
// показать настройки камеры
void MCamera::slotVisibleSetAdv7180_On()
{
    if (grSetting){
        grSetting->setVisible(true);
    manDataMdb->setStateDin(Dalg_SetCam_Visible, ST_DIN_ON);
//    activeItem = -1;
    slotSetParamAdv7180(0, 0);
    }
}

//------------------------------------
// скрыть настройки камеры с сохранением/без
void MCamera::slotSaveSetAdv7180_Off(bool save)
{
    if (save) {
        for (int i=0; i<QUANT_ITEM_SET_ADV7180; i++)
            *TBL_REG_SET_COLOR[i].addrCfg = TBL_REG_SET_COLOR[i].slider->value();
      saveSettingAdv7180();
    }
    if (grSetting){
        grSetting->setVisible(false);
 //       activeItem = -1;

    }
    manDataMdb->setStateDin(Dalg_SetCam_Visible, ST_DIN_OFF);
}


//-------------------------------------------------------
//Сохранение настроек ADV7180
void MCamera::saveSettingAdv7180()
{
    QFile file(FILE_CFG_CAMERA_SAVE_SETING);//"cameraSetMan.cfg");
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << cfgAdc.contrast << cfgAdc.brightness << cfgAdc.saturationB << cfgAdc.saturationR << cfgAdc.hue << cfgAdc.eiCameraManual;
        file.close();
    }
}
//-------------------------------------------------------
void MCamera::slotCameraEi(quint32 _ei)
{
    cfgAdc.eiCameraManual = _ei | CAMERA_EI_ATTR;
    cfgCamera.ei = _ei & 0xff;

    saveSettingAdv7180();
    slotCameraDetect(4000);
}



//--------------------------------------------
// Перерисовка
void MCamera::slotSettingUpdate()
{
    if (grSetting)
        grSetting->update();
}

//--------------------------------------------
//Настройки DEF
void MCamera::slotSettingDef()
{
    if (cfgAdc.contrast != DEF_CONTRAST){
        cfgAdc.contrast = DEF_CONTRAST;
        setParamAdv7180(0, cfgAdc.contrast);  // контрасность по умолчанию
    }
    if (cfgAdc.brightness != DEF_Brightness) {
        cfgAdc.brightness = DEF_Brightness;    // яркость
        setParamAdv7180(1, cfgAdc.brightness);
    }
    if (cfgAdc.saturationB != DEF_SATURATION) {
        cfgAdc.saturationB = DEF_SATURATION; // насыщенность
        setParamAdv7180(2, cfgAdc.saturationB);
    }
    if (cfgAdc.saturationR != DEF_SATURATION) {
        cfgAdc.saturationR = DEF_SATURATION;  // насыщенность
        setParamAdv7180(3, cfgAdc.saturationR);
    }
   if (cfgAdc.hue != DEF_HUE) {
       cfgAdc.hue = DEF_HUE;          // отттенок
       setParamAdv7180(4, cfgAdc.hue);
   }

}
