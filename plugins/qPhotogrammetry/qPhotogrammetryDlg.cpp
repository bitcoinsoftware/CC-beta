#include "qPhotogrammetryDlg.h"
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonValue>
#include <iostream>
#include <string>
#include <QPlainTextEdit>
#include "SocketStub.h"
#include <QApplication>
#include <QTime>

qPhotogrammetryDlg::qPhotogrammetryDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::qPhotogrammetryDlg) {
    ui->setupUi(this);
    connect(this, SIGNAL(get_connected()), this, SLOT(connectToHost()));
    //connect(t->socket(), SIGNAL(get_disconnected()), this, SLOT(sockDisconnected()));
    //MyTelnetWidget mtw;
    //ui->verticalLayout_6->addWidget(mtw);
}

qPhotogrammetryDlg::~qPhotogrammetryDlg()
{
    logfile.close();
    delete ui;
}

void qPhotogrammetryDlg::on_pushButton_clicked()
{

    pathToFolder = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "~/",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    ui->label_11->setText("Input directory: "+pathToFolder);
}

QJsonObject qPhotogrammetryDlg::get_sparse_recon_settings()
{
    QJsonObject computefeatures
    {
          {"describerMethod",   p1describer_method[ui->comboBox_2->currentIndex()]},
          {"describerPreset",   p1describer_preset[ui->comboBox_3->currentIndex()]},
          {"upright",           (ui->checkBox->isChecked()?"1":"0")},
          {"numThreads",        ui->spinBox->value()},
          {"force",             (ui->checkBox_2->isChecked()?"1":"0")}
    };
    QJsonObject computematches
    {
          {"ratio",                         ui->doubleSpinBox->value()},
          {"geometric_model",               p2geometric_model[ui->comboBox_4->currentIndex()]},
          {"video_mode_matching",           (ui->checkBox_3->isChecked()?ui->spinBox_2->value():0)},
          {"nearest_matching_method",       p2nearest_matching_method[ui->comboBox_5->currentIndex()]}
    };
    QJsonObject globalsfm
    {
          {"rotationAveraging",     2-ui->comboBox_6->currentIndex()},
          {"translationAveraging",  3-ui->comboBox_7->currentIndex()},
          {"refineIntrinsics",      p3refine_intrinsic[ui->comboBox_8->currentIndex()]}
    };
    QJsonObject computestructurefromknownposes
    {
          {"_1",   (ui->checkBox_5->isChecked()?"-b":"")},
          {"residual_threshold",  ui->doubleSpinBox_2->value()}
    };

    QJsonObject settings
    {
          {"ComputeFeatures",               computefeatures},
          {"ComputeMatches",                computematches},
          {"GlobalSfM",                     globalsfm},
          {"compute_robust",                ui->checkBox_4->isChecked()},
          {"ComputeStructureFromKnownPoses",computestructurefromknownposes}
    };
    return settings;
}

QJsonObject qPhotogrammetryDlg::get_dense_recon_settings()
{
    QString scale="-s"+QString::number(ui->doubleSpinBox_4->value());
    QString neighbors="-n"+QString::number(ui->spinBox_5->value());
    QString pixels="--max-pixels="+QString::number(ui->spinBox_6->value());
    QString filter="-f"+QString::number(ui->spinBox_6->value());
    QJsonObject dmrecon
    {
        {"_scale",          scale},
        {"_max pixels",     pixels},
        {"_neighbors",      neighbors},
        {"_filter width",   filter},
        {"_dz",             (ui->checkBox_6->isChecked()?"--keep-dz":"")},
        {"_conf",           (ui->checkBox_7->isChecked()?"--keep_conf":"")},
        {"_force",          (ui->checkBox_8->isChecked()?"--force":"")}
    };
    QString scale_factor="-S"+QString::number(ui->doubleSpinBox_3->value());
    QJsonObject scene2pset
    {
        {"_normals",        (ui->checkBox_9->isChecked()?"-n":"")},
        {"_scale",          (ui->checkBox_10->isChecked()?"-s":"")},
        {"_conf",           (ui->checkBox_11->isChecked()?"-c":"")},
        {"_poisson normals",(ui->checkBox_12->isChecked()?"-p":"")},
        {"_scale factor",   scale_factor}
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
    QString scale_factor="-s"+QString::number(ui->doubleSpinBox_6->value());
    QString refine="-r"+QString::number(ui->spinBox_7->value());
    QJsonObject fssrecon
    {
        {"_scale factor",   scale_factor},
        {"refine-octree",   refine},
        {"interpolation",   p4interpolation[ui->comboBox_9->currentIndex()]}
    };
    QString threshold="-t"+QString::number(ui->doubleSpinBox_5->value());
    QString percentile=(ui->checkBox_13->isChecked()?QString("-p"+QString::number(ui->spinBox_8->value())):QString(""));
    QString component_size="-c"+QString::number(ui->spinBox_9->value());
    
    QJsonObject meshclean
    {
        {"_threshold",      threshold},
        {"_percentile",     percentile},
        {"_component size", component_size},
        {"_delete scale",   (ui->checkBox_14->isChecked()?"--delete-scale":"")},
        {"_delete conf",    (ui->checkBox_15->isChecked()?"--delete-conf":"")},
        {"_delete color",   (ui->checkBox_16->isChecked()?"--delete-color":"")}
    };
    QString removal="-o"+p5outlier_removal[ui->comboBox_10->currentIndex()];
    QJsonObject texrecon
    {
        {"_outlier_removal",                 removal},
        {"_skip_geometric_visibility_test", (ui->checkBox_17->isChecked()?"--skip_geometric_visibility_test":"")},
        {"_skip_global_seam_leveling",      (ui->checkBox_18->isChecked()?"--skip_global_seam_leveling":"")},
        {"_skip_local_seam_leveling",       (ui->checkBox_19->isChecked()?"--skip_local_seam_leveling":"")},
        {"_skip_hole_filling",              (ui->checkBox_20->isChecked()?"--skip_hole_filling":"")}
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
    } else if (pathToOutputFolder.trimmed().isEmpty()) {
        QPalette pal = ui->pushButton_2->palette();
        pal.setColor(QPalette::Button, QColor(Qt::red));
        ui->pushButton_2->setAutoFillBackground(true);
        ui->pushButton_2->setPalette(pal);
        ui->pushButton_2->update();
    } else {
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
            {"telnet_port", ui->spinBox_4->value()}
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
    this->setResult(QDialog::Rejected);
    this->close();
}

void qPhotogrammetryDlg::on_comboBox_currentIndexChanged(const QString &arg1)
{
    /*std::string currentPrecision = ui->comboBox->currentText().toStdString();
    std::cout<< "STATE CHANGED TO " << currentPrecision <<"\n";
    if (currentPrecision == "Low"){

        std::cout<< "Seting low params \n";qApp->processEvents();
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
    }*/
}

void qPhotogrammetryDlg::connectToHost()
{
    QString domain = QString(ui->lineEdit->text());
    QString port = QString::number(ui->spinBox_4->value());
    ui->plainTextEdit->setPlainText("");
    SocketStub ss(domain.toStdString(), port.toStdString());
    if (ss.status == -1) {
        if (domain=="127.0.0.1") {
            //start server
        } else {
            ui->plainTextEdit->insertPlainText(QString("Socket status equal -1. Error."));
        }
    } else {
        logfile.setFileName(pathToOutputFolder+"/CClog.txt");
        if (!logfile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
            printf("couldn't open file");
        }
        QTextStream log(&logfile);
        char stages='3';
        if (ui->radioButton->isChecked()) {
            stages='0';
        } else if (ui->radioButton_2->isChecked()) {
            stages='1';
        } else if (ui->radioButton_3->isChecked()) {
            stages='2';
        }
        ui->plainTextEdit->insertPlainText(QString("Conected to server, now sending message...\n"));
        char option=options[ui->comboBox->currentIndex()];
        QString response(ss.send_json(setting_json_obj,option,stages));
        ui->plainTextEdit->insertPlainText("response:"+response+"\n");
        log<<"json sent:\n";
        log<<QString(QJsonDocument(setting_json_obj).toJson(QJsonDocument::Indented));

        QString response2(ss.send_files(pathToFolder));
        ui->plainTextEdit->insertPlainText(response2);
        log<<"files sent\n";

        //get results from server and save to folder
        
        char stage=1;
        while (stage!=-1) {
            char st=stage;
            stage=ss.get_result(stage,"",&log);
            if (st!=stage) {
                QString new_stage("["+QTime::currentTime().toString("hh:mm:ss")+"] stage: "+QString::number(stage)+"\n");
                ui->plainTextEdit->insertPlainText(new_stage);
                log<<new_stage;
            }
            qApp->processEvents();
        }
        ui->plainTextEdit->insertPlainText("gathering results\n");
        log<<"gathering results\n";

        ss.get_result(stage,pathToOutputFolder,&log);
        ss.close_socket();
        QStringList fileList(QDir(pathToOutputFolder).entryList(QDir::Files));
        if (stages>='0') {
            if (!fileList.contains("colorized.ply")) ui->plainTextEdit->insertPlainText("Missing file colorized.ply, from 1st stage\n");
            if (!fileList.contains("log.txt")) ui->plainTextEdit->insertPlainText("Missing file log.txt\n");
            if (!fileList.contains("CClog.txt")) ui->plainTextEdit->insertPlainText("Missing file CClog.txt\n");
            if (!fileList.contains("synth_0.out")) ui->plainTextEdit->insertPlainText("Missing file synth_0.out, from 1st stage\n");
        }
        if (stages>='1') {
            if (!fileList.contains("dense_pointset.ply")) ui->plainTextEdit->insertPlainText("Missing file dense_pointset.ply, from 2nd stage\n");
        }
        if (stages>='2') {
            if (!fileList.contains("surface.ply")) ui->plainTextEdit->insertPlainText("Missing file surface.ply, from 3rd stage\n");
            if (!fileList.contains("clean_surface.ply")) ui->plainTextEdit->insertPlainText("Missing file clean_surface.ply, from 3rd stage\n");
            if (!fileList.contains("textured_clean_surface.obj")) ui->plainTextEdit->insertPlainText("Missing file textured_clean_surface.obj, from 3rd stage");
            if (!fileList.contains("textured_clean_surface.mtl")) ui->plainTextEdit->insertPlainText("Missing file textured_clean_surface.mtl, from 3rd stage");
            if (!fileList.contains("textured_clean_surface.conf")) ui->plainTextEdit->insertPlainText("Missing file textured_clean_surface.conf, from 3rd stage");
            if (!fileList.contains("textured_clean_surface_labeling.vec")) ui->plainTextEdit->insertPlainText("Missing file textured_clean_surface_labeling.vec, from 3rd stage");
            if (!fileList.contains("textured_clean_surface_data_costs.spt")) ui->plainTextEdit->insertPlainText("Missing file textured_clean_surface_data_costs.spt, from 3rd stage");
        }
        if (stages>='3') {
            //if (!fileList.contains("georeference_output")) ui->plainTextEdit->insertPlainText("Missing file dense_pointset.ply\n, from 4th stage");
        }
        ui->plainTextEdit->insertPlainText("finished\n");
        log<<"finished\n";
    }
}

void qPhotogrammetryDlg::closeSession()
{

}

void qPhotogrammetryDlg::on_tabWidget_destroyed()
{

}

void qPhotogrammetryDlg::on_pushButton_2_clicked()
{

    pathToOutputFolder = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "~/",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    ui->label_13->setText("Output directory: "+pathToOutputFolder);
}
