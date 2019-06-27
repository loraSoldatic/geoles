#ifndef MCAMERASETTING_H
#define MCAMERASETTING_H

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QCloseEvent>
#include <QSlider>
#include <QStackedLayout>
#include <QFile>

#include "bcm2835.h"
/*#include "raspicam/raspicam.h"
#include "raspicam/raspicamtypes.h"
#include <fstream>
#include <iostream>
*/
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <glib.h>
#include <gst/controller/gstinterpolationcontrolsource.h>

//#include "mservice.h"



//------------------------------------
//#define CAMERA_PIPELINE "tvsrc name=my_cam ! mfw_isink name=my_sink  sync=false axis-left=%1 axis-top=%2 disp-width=%3 disp-height=%4 rotation=%5"

#define CAMERA_PIPELINE "gst-launch-1.0 rpicamsrc bitrate=1000000 ! filesink location=test.h264"


//------------------------------------
// значения rotation
#define CAM_ROT_NO_FLIP_NO    0 // нет поворота
#define CAM_ROT_NO_FLIP_V     1 // отразить по H
#define CAM_ROT_NO_FLIP_H     2 // отразить по V
#define CAM_ROT_NO_FLIP_HV    3 // отразить по Н и V
#define CAM_ROT_90_FLIP_NO    4 // поворот на 90
#define CAM_ROT_90_FLIP_V     5 // поворот на 90 + отразить по V
#define CAM_ROT_90_FLIP_H     6 // поворот на 90 + отразить по H
#define CAM_ROT_90_FLIP_HV    7 // поворот на 90 + отразить по H и V
//------------------------------
// разрешение для включения камеры
#define CAMERA_EI_NO 0 // запрещено включение
#define CAMERA_EI_KBD 1 // разрешена по кнопке
#define CAMERA_EI_REVERSE 2 // разрешена по заднему
#define CAMERA_EI_ALL     0xff // безусловное разрешение

struct TcfgCamera {
    quint32 ei;       // разрешение включения камеры
    int left; // смещение слева
    int top; // смещение сверху
    int widht; //
    int height;
    int rotate; // поворот
    //-----------------------
    // Widget для настройки
    int setLeft; // смещение слева
    int setTop; // смещение сверху
    int setWidht; //
    int setHeight;

};

#define CAMERA_EI_ATTR  0xea5e0000 // аттибут разрешения камеры
//---------------------
// Item таблицы настройки камеры
struct tSetColor {
    int addr;
    int min;
    int max;
    int def;
    int ofs;
    int step;
    QSlider *slider;
    int *addrCfg;
    MLabel *labSl;
};
#define PARAM_ADV7180_QUANT 5


class MCamera : public QObject
{
    Q_OBJECT
public:
    MCamera(QWidget *parent, QColor colorBack = Qt::black);
    ~MCamera(){}
    bool getCameraEi(int event) {
        return (cfgCamera.ei & event);
    }
    bool getCameraState() {
        return stateCamera;
    }

    void cameraOff();
private:
    TcfgCamera cfgCamera;
    //-----------
    bool stateCamera; // текущее состояние камеры
    quint8 eventState; // владелец события

    GstElement *binCamera;
    QString pipeline;
    bool settingVisible;
    //------------------
    // для вывода изображения с камеры
    QPixmap *pxCamera;
    QLabel *lbCamera;
    // таблица
    // групировка настроек камеры
//    QStackedLayout *laySetCamera;
    QTimer *timerDetect;
    void slotCameraDetect(quint32 timeDraw);
    void slotCameraPlay(quint8 event);

public Q_SLOTS:
   void slotCameraKbd(quint8);
   void slotCameraReverse(quint8);
   void slotSettingUpdate();
   void slotSettingDef();
   void slotEndDetect();
   void slotCameraEi(quint32 _ei);
};



#endif // MCAMERASETTING_H
