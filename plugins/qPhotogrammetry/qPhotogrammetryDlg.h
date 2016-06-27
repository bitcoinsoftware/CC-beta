#ifndef QPHOTOGRAMMETRYDLG_H
#define QPHOTOGRAMMETRYDLG_H

#include <QJsonObject>
#include "ui_qPhotogrammetryDlg.h"

namespace Ui {
class qPhotogrammetryDlg;
}

class qPhotogrammetryDlg : public QDialog, public Ui::qPhotogrammetryDlg
{
    Q_OBJECT

public:
    QString pathToFolder = QString();
    QJsonObject setting_json_obj;
    explicit qPhotogrammetryDlg(QWidget *parent = 0);
    ~qPhotogrammetryDlg();

    QString getPath(){
        return pathToFolder;
    }


    void closeSession();

private slots:
    void on_pushButton_clicked();

    //void on_buttonBox_accepted();
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

