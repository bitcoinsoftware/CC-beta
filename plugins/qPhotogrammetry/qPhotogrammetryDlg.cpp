#include "qPhotogrammetryDlg.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <iostream>
#include <string>
#include "SocketStub.h"

qPhotogrammetryDlg::qPhotogrammetryDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::qPhotogrammetryDlg)
{
    ui->setupUi(this);
    connect(this, SIGNAL(get_connected()), this, SLOT(connectToHost()));
    //connect(t->socket(), SIGNAL(get_disconnected()), this, SLOT(sockDisconnected()));
    //MyTelnetWidget mtw;
    //ui->verticalLayout_6->addWidget(mtw);
}

qPhotogrammetryDlg::~qPhotogrammetryDlg()
{
    delete ui;
}

void qPhotogrammetryDlg::on_pushButton_clicked()
{

    pathToFolder = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "~/",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    ui->label_11->setText(pathToFolder);
}

QJsonObject qPhotogrammetryDlg::get_sparse_recon_settings()
{
    QJsonObject computefeatures
    {
          {"describerMethod",   ui->comboBox_2->currentText()},
          {"describerPreset",   ui->comboBox_3->currentText()},
          {"upright",           ui->checkBox->isChecked()},
          {"numThreads",        ui->spinBox->value()},
          {"force",             ui->checkBox_2->isChecked()}
    };
    QJsonObject computematches
    {
          {"ratio",                         ui->doubleSpinBox->value()},
          {"geometric_model",               ui->comboBox_4->currentText()},
          {"video_mode_matching",           ui->checkBox_3->isChecked()},
          {"video_mode_matching-overlap",   ui->spinBox_2->value()},
          {"nearest_matching_method",       ui->comboBox_5->currentText()}
    };
    QJsonObject globalsfm
    {
          {"rotationAveraging",     ui->comboBox_6->currentText()},
          {"translationAveraging",  ui->comboBox_7->currentText()},
          {"refineIntrinsics",      ui->comboBox_8->currentText()}
    };
    QJsonObject computestructurefromknownposes
    {
          {"enabled",             ui->checkBox_4->isChecked()},
          {"bundle_adjustment",   ui->checkBox_5->isChecked()},
          {"residual_threshold",  ui->doubleSpinBox_2->value()}
    };

    QJsonObject settings
    {
          {"ComputeFeatures",               computefeatures},
          {"ComputeMatches",                computematches},
          {"GlobalSfM",                     globalsfm},
          {"ComputeStructureFromKnownPoses",computestructurefromknownposes}
    };
    return settings;
}

QJsonObject qPhotogrammetryDlg::get_dense_recon_settings()
{
    QJsonObject dmrecon
    {
        {"scale",           ui->doubleSpinBox_4->value()},
        {"max-pixels",      ui->spinBox_3->value()},
        {"local-neighbors", ui->spinBox_5->value()},
        {"filter-width",    ui->spinBox_6->value()},
        {"keep-dz",         ui->checkBox_6->isChecked()},
        {"keep-conf",       ui->checkBox_7->isChecked()},
        {"force",           ui->checkBox_8->isChecked()}
    };
    QJsonObject scene2pset
    {
        {"with-normals",    ui->checkBox_9->isChecked()},
        {"with-scale",      ui->checkBox_10->isChecked()},
        {"with-conf",       ui->checkBox_11->isChecked()},
        {"poisson-normals", ui->checkBox_12->isChecked()},
        {"scale-factor",    ui->doubleSpinBox_3->value()}
    };

    QJsonObject settings
    {
        {"dmrecon",         dmrecon},
        {"scene2pset",      scene2pset}
    };
    return settings;
}

QJsonObject qPhotogrammetryDlg::get_mesh_tex_settings()
{
    QJsonObject fssrecon
    {
        {"scale-factor",    ui->doubleSpinBox_6->value()},
        {"refine-octree",   ui->spinBox_7->value()},
        {"interpolation",   ui->comboBox_9->currentText()}
    };
    QJsonObject meshclean
    {
        {"threshold",           ui->doubleSpinBox_5->value()},
        {"percentile-enabled",  ui->checkBox_13->isChecked()},
        {"percentile",          ui->spinBox_8->value()},
        {"component-size",      ui->spinBox_9->value()},
        {"delete-scale",        ui->checkBox_14->isChecked()},
        {"delete-conf",         ui->checkBox_15->isChecked()},
        {"delete-color",        ui->checkBox_16->isChecked()}
    };
    QJsonObject texrecon
    {
        {"outlier_removal",                 ui->comboBox_10->currentText()},
        {"skip_geometric_visibility_test",  ui->checkBox_17->isChecked()},
        {"skip_global_seam_leveling",       ui->checkBox_18->isChecked()},
        {"skip_local_seam_leveling",        ui->checkBox_19->isChecked()},
        {"skip_hole_filling",               ui->checkBox_20->isChecked()}
    };
    QJsonObject settings
    {
        {"fssrecon",    fssrecon},
        {"meshclean",   meshclean},
        {"texrecon",    texrecon}
    };
    return settings;
}

QJsonObject qPhotogrammetryDlg::get_georeference_settings()
{
    QJsonObject settings;
    return settings;
}

//void qPhotogrammetryDlg::on_buttonBox_accepted()
void qPhotogrammetryDlg::on_buttonBox_accepted()
{
    //if no folder with photos was choosen
    if (pathToFolder.trimmed().isEmpty()){
        QPalette pal = ui->pushButton->palette();
        pal.setColor(QPalette::Button, QColor(Qt::red));
        ui->pushButton->setAutoFillBackground(true);
        ui->pushButton->setPalette(pal);
        ui->pushButton->update();
    }else
    {
        //TODO read all widget states
        if (ui->radioButton->isChecked()){  //if only initial reconstruction choosen
            QJsonObject object
            {
                {"sparse_recon",    true},
                {"dense_recon",     false},
                {"mesh_&_tex",      false},
                {"georeference",    false}
            };
            setting_json_obj = object;
            setting_json_obj.insert(QString("sparse_recon_params"), get_sparse_recon_settings());
        }
        else if(ui->radioButton_2->isChecked()) { //if densification choosen
            QJsonObject object
            {
                {"sparse_recon",    true},
                {"dense_recon",     true},
                {"mesh_&_tex",      false},
                {"georeference",    false}
            };
            setting_json_obj = object;
            setting_json_obj.insert(QString("sparse_recon_params"), get_sparse_recon_settings());
            setting_json_obj.insert(QString("dense_recon_params"),  get_dense_recon_settings());
        }
        else if(ui->radioButton_3->isChecked()){ //if meshing & texturing choosen
            QJsonObject object
            {
                {"sparse_recon",    true},
                {"dense_recon",     true},
                {"mesh_&_tex",      true},
                {"georeference",    false}
            };
            setting_json_obj = object;
            setting_json_obj.insert(QString("sparse_recon_params"), get_sparse_recon_settings());
            setting_json_obj.insert(QString("dense_recon_params"),  get_dense_recon_settings());
            setting_json_obj.insert(QString("mesh_&_tex_params"),          get_mesh_tex_settings());
        }
        else if(ui->radioButton_4->isChecked()){ //if georeferencing choosen
            QJsonObject object
            {
                {"sparse_recon",    true},
                {"dense_recon",     true},
                {"mesh_&_tex",      true},
                {"georeference",    true}
            };
            setting_json_obj = object;
            setting_json_obj.insert(QString("sparse_recon_params"), get_sparse_recon_settings());
            setting_json_obj.insert(QString("dense_recon_params"),  get_dense_recon_settings());
            setting_json_obj.insert(QString("mesh_&_tex_params"),          get_mesh_tex_settings());
            setting_json_obj.insert(QString("georeference_params"),        get_georeference_settings());
        }
        QJsonObject object
        {
            {"path_to_photo_folder", ui->label_11->text()},
            {"domain", ui->lineEdit->text()},
            {"telnet_port", ui->spinBox_4->value()},
            {"ftp_port", ui->spinBox_10->value()}
        };
        //setting_json_obj.insert(QString("server"), object);

        //QFile saveFile(QStringLiteral("save.json"));
        //if (!saveFile.open(QIODevice::WriteOnly)) {
        //    qWarning("Couldn't open save file.");
        //    return;
        //    }
        //TODO send via whaever
        //saveFile.write(saveDoc.toJson());

        //this->setResult(QDialog::Accepted); this->close(); //CLOSE WINDOW
        //connectToHost(ui->lineEdit->text(), ui->spinBox_4->value());
        emit get_connected();
    }
    //QJsonDocument saveDoc(setting_json_obj);
    //QString strJson(saveDoc.toJson());
    //return saveDoc.toJson();
}

void qPhotogrammetryDlg::on_buttonBox_rejected()
{
    this->setResult(QDialog::Rejected);this->close();
}

void qPhotogrammetryDlg::on_comboBox_currentIndexChanged(const QString &arg1)
{
    std::string currentPrecision = ui->comboBox->currentText().toStdString();
    std::cout<< "STATE CHANGED TO " << currentPrecision <<"\n";
    if (currentPrecision == "Low"){

        std::cout<< "Seting low params \n";
        //sparse recon
            ui->comboBox_3->setCurrentIndex(0);//Compute features set to normal
            //global structure from motion
            ui->comboBox_6->setCurrentIndex(0); //L2 minimization
            ui->comboBox_7->setCurrentIndex(1); //Translation Averaging L2
            ui->comboBox_8->setCurrentIndex(1); //Refine intrinsic set to None
       //dense recon
            ui->doubleSpinBox_4->setValue(3); //set scale
            ui->doubleSpinBox_3->setValue(3);
       //floating scale surface reconstruction
            ui->doubleSpinBox_6->setValue(3); //scale factor
            ui->spinBox_7->setValue(0); // refine octree
       //texturing
            ui->comboBox_10->setCurrentIndex(0);


    }
    else if (currentPrecision == "Normal"){
        std::cout<< "Seting normal params \n";
        //sparse recon
            ui->comboBox_3->setCurrentIndex(0);//Compute features set to normal
            //global structure from motion
            ui->comboBox_6->setCurrentIndex(0);//L2 minimization
            ui->comboBox_7->setCurrentIndex(0); //Translation Averaging SoftL1
            ui->comboBox_8->setCurrentIndex(0);// Refine intrinsic set to Adjust_All
        //dense recon
            ui->doubleSpinBox_4->setValue(2); //set scale
            ui->doubleSpinBox_3->setValue(2);
        //floating scale surface reconstruction
            ui->doubleSpinBox_6->setValue(2); //scale factor
            ui->spinBox_7->setValue(1); // refine octree
        //texturing
            ui->comboBox_10->setCurrentIndex(1);

    }
    else if (currentPrecision == "High"){
        std::cout<< "Seting high params \n";
        //sparse recon
            ui->comboBox_3->setCurrentIndex(1);//Compute features set to high
            //global structure from motion
            ui->comboBox_6->setCurrentIndex(0);//L2 minimization
            ui->comboBox_7->setCurrentIndex(0); //Translation Averaging SoftL1
            ui->comboBox_8->setCurrentIndex(0);//Refine intrinsic set to Adjust_All
        //dense recon
            ui->doubleSpinBox_4->setValue(1); //set scale
            ui->doubleSpinBox_3->setValue(1);
        //floating scale surface reconstruction
            ui->doubleSpinBox_6->setValue(1); //scale factor
            ui->spinBox_7->setValue(2); // refine octree
        //texturing
            ui->comboBox_10->setCurrentIndex(1);
    }
    else if (currentPrecision == "Ultra"){
        std::cout<< "Seting ultra params \n";
        //sparse recon
            ui->comboBox_3->setCurrentIndex(2); //Compute features set to ultra
            //global structure from motion
            ui->comboBox_6->setCurrentIndex(1);//L2 minimization
            ui->comboBox_7->setCurrentIndex(0); //Translation Averaging SoftL1
            ui->comboBox_8->setCurrentIndex(0);//Refine intrinsic set to Adjust_All
       //dense recon
            ui->doubleSpinBox_4->setValue(1); //set scale
            ui->doubleSpinBox_3->setValue(1);
       //floating scale surface reconstruction
            ui->doubleSpinBox_6->setValue(1); //scale factor
            ui->spinBox_7->setValue(3); // refine octree
       //texturing
            ui->comboBox_10->setCurrentIndex(1);
    }
}

void qPhotogrammetryDlg::connectToHost()
{
    QString domain = QString(ui->lineEdit->text());
    QString port = QString::number(ui->spinBox_4->value());
    ui->plainTextEdit->insertPlainText(QString("Connecting to :")+ domain + QString(" On port :") + port + QString("..."));

    SocketStub ss(domain.toStdString(), port.toStdString());
    if (ss.status == -1){
        ui->plainTextEdit->insertPlainText(QString("\nSocket status equal -1. Error."));
    }
    else{
        ui->plainTextEdit->insertPlainText(QString("\nConected to server, now sending message..."));
        int buffLen = 1000;
        //char response[buffLen];
        std::string response(ss.send_command(setting_json_obj, buffLen));
        ui->plainTextEdit->insertPlainText(QString::fromStdString(response));
        ss.close_socket();
        ui->plainTextEdit->insertPlainText("/nclosed");
    }
}

void qPhotogrammetryDlg::closeSession()
{

}

void qPhotogrammetryDlg::on_tabWidget_destroyed()
{

}
