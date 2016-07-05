#ifndef QPHOTOGRAMMETRYDLG_H
#define QPHOTOGRAMMETRYDLG_H

#include <QJsonObject>
#include <QFile>
#include "ui_qPhotogrammetryDlg.h"

const QString p1describer_method[]={"SIFT","AKAZE_FLOAT","AKAZE_MLDB"};
const QString p1describer_preset[]={"NORMAL","HIGH","ULTRA"};
const QString p2geometric_model[]={"f","e","h"};
const QString p2nearest_matching_method[]={"AUTO","BRUTEFORCEL2","ANNL2","CASCADEHASHINGL2","FASTCASCADEHASHINGL2","BRUTEFORCEHAMMING"};
const QString p3refine_intrinsic[]={"ADJUST_ALL","NONE","ADJUST_FOCAL_LENGTH","ADJUST_PRINCIPAL_POINT","ADJUST_DISTORTION","ADJUST_FOCAL_LENGTH|ADJUST_PRINCIPAL_POINT","ADJUST_FOCAL_LENGTH|ADJUST_DISTORTION","ADJUST_PRINCIPAL_POINT|ADJUST_DISTORTION"};
const QString p4interpolation[]={"cubic","linear","scaling","lsderiv"};
const QString p5outlier_removal[]={"none","gauss_damping","gauss_clamping"};
const char options[]={'0','1','2','c'};

namespace Ui {
class qPhotogrammetryDlg;
}

class qPhotogrammetryDlg : public QDialog, public Ui::qPhotogrammetryDlg
{
    Q_OBJECT

public:
    QFile logfile;
    QString pathToFolder = QString();
    QString pathToOutputFolder = QString();
    QJsonObject setting_json_obj;
    explicit qPhotogrammetryDlg(QWidget *parent = 0);
    ~qPhotogrammetryDlg();

    QString getPath(){
        return pathToFolder;
    }


    void closeSession();

private slots:
    void on_pushButton_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    QJsonObject get_sparse_recon_settings();

    QJsonObject get_dense_recon_settings();

    QJsonObject get_mesh_tex_settings();

    QJsonObject get_georeference_settings();

    void on_tabWidget_destroyed();
    void connectToHost();

private:
    Ui::qPhotogrammetryDlg *ui;

signals:
    void get_connected();
    void connected();
    void get_disconnected();
    void disconnected();
};

#endif // QPHOTOGRAMMETRYDLG_H

