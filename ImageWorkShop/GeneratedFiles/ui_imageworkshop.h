/********************************************************************************
** Form generated from reading UI file 'imageworkshop.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEWORKSHOP_H
#define UI_IMAGEWORKSHOP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImageWorkShopClass
{
public:
    QAction *actionOpen;
    QAction *actionSave_as;
    QAction *actionStageSave;
    QAction *actionStageBack;
    QAction *actionCompare;
    QAction *actionImage_last_saved;
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QTabWidget *tabWidget;
    QWidget *tabFilter;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButtonMedian;
    QSpinBox *spinBoxMedian;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *labelVignette;
    QSlider *horizontalSliderVignette;
    QLineEdit *lineEditVignette;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelGauss;
    QSlider *horizontalSliderGauss;
    QLineEdit *lineEditGauss;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButtonSharp;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *labelBalance;
    QSlider *horizontalSliderBalance;
    QLineEdit *lineEditBalance;
    QHBoxLayout *horizontalLayout_6;
    QLabel *labelSaturation;
    QSlider *horizontalSliderSaturation;
    QLineEdit *lineEditSaturation;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushButtonLiquify;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButtonLiquifyRadius;
    QSpinBox *spinBoxLiquifyRadius;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_12;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_Lomo;
    QComboBox *comboBox_Lomo;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_9;
    QLabel *labelLomoR;
    QSlider *horizontalSliderLomoR;
    QLineEdit *lineEditLomoR;
    QHBoxLayout *horizontalLayout_10;
    QLabel *labelGauss_3;
    QSlider *horizontalSliderLomoG;
    QLineEdit *lineEditLomoG;
    QHBoxLayout *horizontalLayout_11;
    QLabel *labelGauss_4;
    QSlider *horizontalSliderLomoB;
    QLineEdit *lineEditLomoB;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_7;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_5;
    QSlider *horizontalSliderWhitenSkinMask;
    QLineEdit *lineEditWhitenSkinMask;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_4;
    QSlider *horizontalSliderThreshold;
    QLineEdit *lineEditWhitenThreshold;
    QHBoxLayout *horizontalLayout_13;
    QPushButton *pushButton_Exfoliating;
    QSpacerItem *horizontalSpacer_7;
    QLabel *label_3;
    QSpinBox *spinBox_Vdev;
    QSpacerItem *horizontalSpacer_6;
    QLabel *label_2;
    QSpinBox *spinBox_Sdev;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label;
    QSlider *horizontalSliderWhitenSkin;
    QLineEdit *lineEditWhitenSkin;
    QSpacerItem *verticalSpacer;
    QWidget *tabFix;
    QLineEdit *lineEditBrushRadiusImageCopy;
    QPushButton *pushButtonMovePatch;
    QSlider *horizontalSliderBrushRadiusImageCopy;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QSlider *horizontalSliderBrushRadiusImageInpaintHole;
    QLineEdit *lineEditBrushRadiusImageInpaintHole;
    QPushButton *pushButtonImageInpaintHoleResult;
    QLabel *label_10;
    QSlider *horizontalSliderBrushRadiusImageInpaintHoleRestrict;
    QLineEdit *lineEditBrushRadiusImageInpaintHoleRestrict;
    QPushButton *pushButtonImageInpaintHoleRestrictResult;
    QPushButton *pushButtonErodeRadius;
    QSpinBox *spinBoxErodeRadius;
    QPushButton *pushButtonImageInpaintHoleRestrictResultOut;
    QComboBox *comboBox_ifGpu;
    QLabel *label_11;
    QPushButton *pushButton_ClearRestrict;
    QPushButton *pushButton_ClearRegion;
    QWidget *tabColorMap;
    QPushButton *pushButton_Source;
    QPushButton *pushButton_Target;
    QPushButton *pushButton_Result;
    QPushButton *pushButton_ResultGrayToRGB;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuStage;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ImageWorkShopClass)
    {
        if (ImageWorkShopClass->objectName().isEmpty())
            ImageWorkShopClass->setObjectName(QStringLiteral("ImageWorkShopClass"));
        ImageWorkShopClass->resize(502, 604);
        actionOpen = new QAction(ImageWorkShopClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionSave_as = new QAction(ImageWorkShopClass);
        actionSave_as->setObjectName(QStringLiteral("actionSave_as"));
        actionStageSave = new QAction(ImageWorkShopClass);
        actionStageSave->setObjectName(QStringLiteral("actionStageSave"));
        actionStageBack = new QAction(ImageWorkShopClass);
        actionStageBack->setObjectName(QStringLiteral("actionStageBack"));
        actionCompare = new QAction(ImageWorkShopClass);
        actionCompare->setObjectName(QStringLiteral("actionCompare"));
        actionImage_last_saved = new QAction(ImageWorkShopClass);
        actionImage_last_saved->setObjectName(QStringLiteral("actionImage_last_saved"));
        centralWidget = new QWidget(ImageWorkShopClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabFilter = new QWidget();
        tabFilter->setObjectName(QStringLiteral("tabFilter"));
        layoutWidget = new QWidget(tabFilter);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 440, 323));
        verticalLayout_5 = new QVBoxLayout(layoutWidget);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        pushButtonMedian = new QPushButton(layoutWidget);
        pushButtonMedian->setObjectName(QStringLiteral("pushButtonMedian"));

        horizontalLayout_3->addWidget(pushButtonMedian);

        spinBoxMedian = new QSpinBox(layoutWidget);
        spinBoxMedian->setObjectName(QStringLiteral("spinBoxMedian"));
        spinBoxMedian->setMinimumSize(QSize(50, 0));
        spinBoxMedian->setMinimum(1);
        spinBoxMedian->setMaximum(7);

        horizontalLayout_3->addWidget(spinBoxMedian);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        labelVignette = new QLabel(layoutWidget);
        labelVignette->setObjectName(QStringLiteral("labelVignette"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelVignette->sizePolicy().hasHeightForWidth());
        labelVignette->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(labelVignette);

        horizontalSliderVignette = new QSlider(layoutWidget);
        horizontalSliderVignette->setObjectName(QStringLiteral("horizontalSliderVignette"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(horizontalSliderVignette->sizePolicy().hasHeightForWidth());
        horizontalSliderVignette->setSizePolicy(sizePolicy1);
        horizontalSliderVignette->setMinimumSize(QSize(300, 0));
        horizontalSliderVignette->setMaximum(28);
        horizontalSliderVignette->setSliderPosition(28);
        horizontalSliderVignette->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(horizontalSliderVignette);

        lineEditVignette = new QLineEdit(layoutWidget);
        lineEditVignette->setObjectName(QStringLiteral("lineEditVignette"));
        sizePolicy1.setHeightForWidth(lineEditVignette->sizePolicy().hasHeightForWidth());
        lineEditVignette->setSizePolicy(sizePolicy1);
        lineEditVignette->setMaximumSize(QSize(60, 16777215));

        horizontalLayout->addWidget(lineEditVignette);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        labelGauss = new QLabel(layoutWidget);
        labelGauss->setObjectName(QStringLiteral("labelGauss"));
        sizePolicy.setHeightForWidth(labelGauss->sizePolicy().hasHeightForWidth());
        labelGauss->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(labelGauss);

        horizontalSliderGauss = new QSlider(layoutWidget);
        horizontalSliderGauss->setObjectName(QStringLiteral("horizontalSliderGauss"));
        sizePolicy1.setHeightForWidth(horizontalSliderGauss->sizePolicy().hasHeightForWidth());
        horizontalSliderGauss->setSizePolicy(sizePolicy1);
        horizontalSliderGauss->setMinimumSize(QSize(300, 0));
        horizontalSliderGauss->setMinimum(1);
        horizontalSliderGauss->setMaximum(10);
        horizontalSliderGauss->setSliderPosition(1);
        horizontalSliderGauss->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(horizontalSliderGauss);

        lineEditGauss = new QLineEdit(layoutWidget);
        lineEditGauss->setObjectName(QStringLiteral("lineEditGauss"));
        sizePolicy1.setHeightForWidth(lineEditGauss->sizePolicy().hasHeightForWidth());
        lineEditGauss->setSizePolicy(sizePolicy1);
        lineEditGauss->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_2->addWidget(lineEditGauss);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        pushButtonSharp = new QPushButton(layoutWidget);
        pushButtonSharp->setObjectName(QStringLiteral("pushButtonSharp"));

        horizontalLayout_5->addWidget(pushButtonSharp);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_5);


        verticalLayout_2->addLayout(verticalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        labelBalance = new QLabel(layoutWidget);
        labelBalance->setObjectName(QStringLiteral("labelBalance"));
        sizePolicy.setHeightForWidth(labelBalance->sizePolicy().hasHeightForWidth());
        labelBalance->setSizePolicy(sizePolicy);

        horizontalLayout_4->addWidget(labelBalance);

        horizontalSliderBalance = new QSlider(layoutWidget);
        horizontalSliderBalance->setObjectName(QStringLiteral("horizontalSliderBalance"));
        sizePolicy1.setHeightForWidth(horizontalSliderBalance->sizePolicy().hasHeightForWidth());
        horizontalSliderBalance->setSizePolicy(sizePolicy1);
        horizontalSliderBalance->setMinimumSize(QSize(300, 0));
        horizontalSliderBalance->setMinimum(1);
        horizontalSliderBalance->setMaximum(30);
        horizontalSliderBalance->setPageStep(10);
        horizontalSliderBalance->setSliderPosition(1);
        horizontalSliderBalance->setOrientation(Qt::Horizontal);

        horizontalLayout_4->addWidget(horizontalSliderBalance);

        lineEditBalance = new QLineEdit(layoutWidget);
        lineEditBalance->setObjectName(QStringLiteral("lineEditBalance"));
        sizePolicy1.setHeightForWidth(lineEditBalance->sizePolicy().hasHeightForWidth());
        lineEditBalance->setSizePolicy(sizePolicy1);
        lineEditBalance->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_4->addWidget(lineEditBalance);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        labelSaturation = new QLabel(layoutWidget);
        labelSaturation->setObjectName(QStringLiteral("labelSaturation"));
        sizePolicy.setHeightForWidth(labelSaturation->sizePolicy().hasHeightForWidth());
        labelSaturation->setSizePolicy(sizePolicy);

        horizontalLayout_6->addWidget(labelSaturation);

        horizontalSliderSaturation = new QSlider(layoutWidget);
        horizontalSliderSaturation->setObjectName(QStringLiteral("horizontalSliderSaturation"));
        sizePolicy1.setHeightForWidth(horizontalSliderSaturation->sizePolicy().hasHeightForWidth());
        horizontalSliderSaturation->setSizePolicy(sizePolicy1);
        horizontalSliderSaturation->setMinimumSize(QSize(300, 0));
        horizontalSliderSaturation->setMinimum(1);
        horizontalSliderSaturation->setMaximum(100);
        horizontalSliderSaturation->setPageStep(10);
        horizontalSliderSaturation->setSliderPosition(1);
        horizontalSliderSaturation->setOrientation(Qt::Horizontal);

        horizontalLayout_6->addWidget(horizontalSliderSaturation);

        lineEditSaturation = new QLineEdit(layoutWidget);
        lineEditSaturation->setObjectName(QStringLiteral("lineEditSaturation"));
        sizePolicy1.setHeightForWidth(lineEditSaturation->sizePolicy().hasHeightForWidth());
        lineEditSaturation->setSizePolicy(sizePolicy1);
        lineEditSaturation->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_6->addWidget(lineEditSaturation);


        verticalLayout_2->addLayout(horizontalLayout_6);


        verticalLayout_4->addLayout(verticalLayout_2);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        pushButtonLiquify = new QPushButton(layoutWidget);
        pushButtonLiquify->setObjectName(QStringLiteral("pushButtonLiquify"));

        horizontalLayout_7->addWidget(pushButtonLiquify);

        horizontalSpacer_2 = new QSpacerItem(198, 17, QSizePolicy::Maximum, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_2);

        pushButtonLiquifyRadius = new QPushButton(layoutWidget);
        pushButtonLiquifyRadius->setObjectName(QStringLiteral("pushButtonLiquifyRadius"));

        horizontalLayout_7->addWidget(pushButtonLiquifyRadius);

        spinBoxLiquifyRadius = new QSpinBox(layoutWidget);
        spinBoxLiquifyRadius->setObjectName(QStringLiteral("spinBoxLiquifyRadius"));
        spinBoxLiquifyRadius->setMinimum(10);
        spinBoxLiquifyRadius->setMaximum(100);
        spinBoxLiquifyRadius->setValue(20);

        horizontalLayout_7->addWidget(spinBoxLiquifyRadius);


        verticalLayout_4->addLayout(horizontalLayout_7);


        verticalLayout_5->addLayout(verticalLayout_4);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        label_Lomo = new QLabel(layoutWidget);
        label_Lomo->setObjectName(QStringLiteral("label_Lomo"));

        horizontalLayout_8->addWidget(label_Lomo);

        comboBox_Lomo = new QComboBox(layoutWidget);
        comboBox_Lomo->setObjectName(QStringLiteral("comboBox_Lomo"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(comboBox_Lomo->sizePolicy().hasHeightForWidth());
        comboBox_Lomo->setSizePolicy(sizePolicy2);
        comboBox_Lomo->setMinimumSize(QSize(100, 0));
        comboBox_Lomo->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_8->addWidget(comboBox_Lomo);


        horizontalLayout_12->addLayout(horizontalLayout_8);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_4);


        verticalLayout_3->addLayout(horizontalLayout_12);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        labelLomoR = new QLabel(layoutWidget);
        labelLomoR->setObjectName(QStringLiteral("labelLomoR"));
        sizePolicy.setHeightForWidth(labelLomoR->sizePolicy().hasHeightForWidth());
        labelLomoR->setSizePolicy(sizePolicy);

        horizontalLayout_9->addWidget(labelLomoR);

        horizontalSliderLomoR = new QSlider(layoutWidget);
        horizontalSliderLomoR->setObjectName(QStringLiteral("horizontalSliderLomoR"));
        sizePolicy1.setHeightForWidth(horizontalSliderLomoR->sizePolicy().hasHeightForWidth());
        horizontalSliderLomoR->setSizePolicy(sizePolicy1);
        horizontalSliderLomoR->setMinimumSize(QSize(300, 0));
        horizontalSliderLomoR->setMinimum(1);
        horizontalSliderLomoR->setMaximum(255);
        horizontalSliderLomoR->setValue(127);
        horizontalSliderLomoR->setSliderPosition(127);
        horizontalSliderLomoR->setOrientation(Qt::Horizontal);

        horizontalLayout_9->addWidget(horizontalSliderLomoR);

        lineEditLomoR = new QLineEdit(layoutWidget);
        lineEditLomoR->setObjectName(QStringLiteral("lineEditLomoR"));
        sizePolicy1.setHeightForWidth(lineEditLomoR->sizePolicy().hasHeightForWidth());
        lineEditLomoR->setSizePolicy(sizePolicy1);
        lineEditLomoR->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_9->addWidget(lineEditLomoR);


        verticalLayout_3->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        labelGauss_3 = new QLabel(layoutWidget);
        labelGauss_3->setObjectName(QStringLiteral("labelGauss_3"));
        sizePolicy.setHeightForWidth(labelGauss_3->sizePolicy().hasHeightForWidth());
        labelGauss_3->setSizePolicy(sizePolicy);

        horizontalLayout_10->addWidget(labelGauss_3);

        horizontalSliderLomoG = new QSlider(layoutWidget);
        horizontalSliderLomoG->setObjectName(QStringLiteral("horizontalSliderLomoG"));
        sizePolicy1.setHeightForWidth(horizontalSliderLomoG->sizePolicy().hasHeightForWidth());
        horizontalSliderLomoG->setSizePolicy(sizePolicy1);
        horizontalSliderLomoG->setMinimumSize(QSize(300, 0));
        horizontalSliderLomoG->setMinimum(1);
        horizontalSliderLomoG->setMaximum(255);
        horizontalSliderLomoG->setValue(127);
        horizontalSliderLomoG->setSliderPosition(127);
        horizontalSliderLomoG->setOrientation(Qt::Horizontal);

        horizontalLayout_10->addWidget(horizontalSliderLomoG);

        lineEditLomoG = new QLineEdit(layoutWidget);
        lineEditLomoG->setObjectName(QStringLiteral("lineEditLomoG"));
        sizePolicy1.setHeightForWidth(lineEditLomoG->sizePolicy().hasHeightForWidth());
        lineEditLomoG->setSizePolicy(sizePolicy1);
        lineEditLomoG->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_10->addWidget(lineEditLomoG);


        verticalLayout_3->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        labelGauss_4 = new QLabel(layoutWidget);
        labelGauss_4->setObjectName(QStringLiteral("labelGauss_4"));
        sizePolicy.setHeightForWidth(labelGauss_4->sizePolicy().hasHeightForWidth());
        labelGauss_4->setSizePolicy(sizePolicy);

        horizontalLayout_11->addWidget(labelGauss_4);

        horizontalSliderLomoB = new QSlider(layoutWidget);
        horizontalSliderLomoB->setObjectName(QStringLiteral("horizontalSliderLomoB"));
        sizePolicy1.setHeightForWidth(horizontalSliderLomoB->sizePolicy().hasHeightForWidth());
        horizontalSliderLomoB->setSizePolicy(sizePolicy1);
        horizontalSliderLomoB->setMinimumSize(QSize(300, 0));
        horizontalSliderLomoB->setMinimum(1);
        horizontalSliderLomoB->setMaximum(255);
        horizontalSliderLomoB->setValue(127);
        horizontalSliderLomoB->setSliderPosition(127);
        horizontalSliderLomoB->setOrientation(Qt::Horizontal);

        horizontalLayout_11->addWidget(horizontalSliderLomoB);

        lineEditLomoB = new QLineEdit(layoutWidget);
        lineEditLomoB->setObjectName(QStringLiteral("lineEditLomoB"));
        sizePolicy1.setHeightForWidth(lineEditLomoB->sizePolicy().hasHeightForWidth());
        lineEditLomoB->setSizePolicy(sizePolicy1);
        lineEditLomoB->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_11->addWidget(lineEditLomoB);


        verticalLayout_3->addLayout(horizontalLayout_11);


        verticalLayout_5->addLayout(verticalLayout_3);

        layoutWidget1 = new QWidget(tabFilter);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(0, 330, 456, 159));
        verticalLayout_7 = new QVBoxLayout(layoutWidget1);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setSpacing(6);
        horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
        label_5 = new QLabel(layoutWidget1);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_14->addWidget(label_5);

        horizontalSliderWhitenSkinMask = new QSlider(layoutWidget1);
        horizontalSliderWhitenSkinMask->setObjectName(QStringLiteral("horizontalSliderWhitenSkinMask"));
        sizePolicy1.setHeightForWidth(horizontalSliderWhitenSkinMask->sizePolicy().hasHeightForWidth());
        horizontalSliderWhitenSkinMask->setSizePolicy(sizePolicy1);
        horizontalSliderWhitenSkinMask->setMinimumSize(QSize(300, 0));
        horizontalSliderWhitenSkinMask->setMinimum(2);
        horizontalSliderWhitenSkinMask->setMaximum(50);
        horizontalSliderWhitenSkinMask->setValue(5);
        horizontalSliderWhitenSkinMask->setSliderPosition(5);
        horizontalSliderWhitenSkinMask->setOrientation(Qt::Horizontal);

        horizontalLayout_14->addWidget(horizontalSliderWhitenSkinMask);

        lineEditWhitenSkinMask = new QLineEdit(layoutWidget1);
        lineEditWhitenSkinMask->setObjectName(QStringLiteral("lineEditWhitenSkinMask"));
        sizePolicy1.setHeightForWidth(lineEditWhitenSkinMask->sizePolicy().hasHeightForWidth());
        lineEditWhitenSkinMask->setSizePolicy(sizePolicy1);
        lineEditWhitenSkinMask->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_14->addWidget(lineEditWhitenSkinMask);


        verticalLayout_6->addLayout(horizontalLayout_14);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setSpacing(6);
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        label_4 = new QLabel(layoutWidget1);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_15->addWidget(label_4);

        horizontalSliderThreshold = new QSlider(layoutWidget1);
        horizontalSliderThreshold->setObjectName(QStringLiteral("horizontalSliderThreshold"));
        sizePolicy1.setHeightForWidth(horizontalSliderThreshold->sizePolicy().hasHeightForWidth());
        horizontalSliderThreshold->setSizePolicy(sizePolicy1);
        horizontalSliderThreshold->setMinimumSize(QSize(300, 0));
        horizontalSliderThreshold->setMinimum(1);
        horizontalSliderThreshold->setMaximum(15);
        horizontalSliderThreshold->setValue(1);
        horizontalSliderThreshold->setSliderPosition(1);
        horizontalSliderThreshold->setOrientation(Qt::Horizontal);

        horizontalLayout_15->addWidget(horizontalSliderThreshold);

        lineEditWhitenThreshold = new QLineEdit(layoutWidget1);
        lineEditWhitenThreshold->setObjectName(QStringLiteral("lineEditWhitenThreshold"));
        sizePolicy1.setHeightForWidth(lineEditWhitenThreshold->sizePolicy().hasHeightForWidth());
        lineEditWhitenThreshold->setSizePolicy(sizePolicy1);
        lineEditWhitenThreshold->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_15->addWidget(lineEditWhitenThreshold);


        verticalLayout_6->addLayout(horizontalLayout_15);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        pushButton_Exfoliating = new QPushButton(layoutWidget1);
        pushButton_Exfoliating->setObjectName(QStringLiteral("pushButton_Exfoliating"));

        horizontalLayout_13->addWidget(pushButton_Exfoliating);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_7);

        label_3 = new QLabel(layoutWidget1);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_13->addWidget(label_3);

        spinBox_Vdev = new QSpinBox(layoutWidget1);
        spinBox_Vdev->setObjectName(QStringLiteral("spinBox_Vdev"));
        spinBox_Vdev->setMinimum(8);
        spinBox_Vdev->setMaximum(12);
        spinBox_Vdev->setValue(10);

        horizontalLayout_13->addWidget(spinBox_Vdev);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_6);

        label_2 = new QLabel(layoutWidget1);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_13->addWidget(label_2);

        spinBox_Sdev = new QSpinBox(layoutWidget1);
        spinBox_Sdev->setObjectName(QStringLiteral("spinBox_Sdev"));
        spinBox_Sdev->setMinimum(20);
        spinBox_Sdev->setMaximum(500);
        spinBox_Sdev->setValue(50);

        horizontalLayout_13->addWidget(spinBox_Sdev);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_5);


        verticalLayout_6->addLayout(horizontalLayout_13);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setSpacing(6);
        horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
        label = new QLabel(layoutWidget1);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_16->addWidget(label);

        horizontalSliderWhitenSkin = new QSlider(layoutWidget1);
        horizontalSliderWhitenSkin->setObjectName(QStringLiteral("horizontalSliderWhitenSkin"));
        sizePolicy1.setHeightForWidth(horizontalSliderWhitenSkin->sizePolicy().hasHeightForWidth());
        horizontalSliderWhitenSkin->setSizePolicy(sizePolicy1);
        horizontalSliderWhitenSkin->setMinimumSize(QSize(300, 0));
        horizontalSliderWhitenSkin->setMinimum(2);
        horizontalSliderWhitenSkin->setMaximum(5);
        horizontalSliderWhitenSkin->setValue(5);
        horizontalSliderWhitenSkin->setSliderPosition(5);
        horizontalSliderWhitenSkin->setOrientation(Qt::Horizontal);

        horizontalLayout_16->addWidget(horizontalSliderWhitenSkin);

        lineEditWhitenSkin = new QLineEdit(layoutWidget1);
        lineEditWhitenSkin->setObjectName(QStringLiteral("lineEditWhitenSkin"));
        sizePolicy1.setHeightForWidth(lineEditWhitenSkin->sizePolicy().hasHeightForWidth());
        lineEditWhitenSkin->setSizePolicy(sizePolicy1);
        lineEditWhitenSkin->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_16->addWidget(lineEditWhitenSkin);


        verticalLayout_6->addLayout(horizontalLayout_16);


        verticalLayout_7->addLayout(verticalLayout_6);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(verticalSpacer);

        tabWidget->addTab(tabFilter, QString());
        tabFix = new QWidget();
        tabFix->setObjectName(QStringLiteral("tabFix"));
        lineEditBrushRadiusImageCopy = new QLineEdit(tabFix);
        lineEditBrushRadiusImageCopy->setObjectName(QStringLiteral("lineEditBrushRadiusImageCopy"));
        lineEditBrushRadiusImageCopy->setGeometry(QRect(410, 390, 51, 20));
        pushButtonMovePatch = new QPushButton(tabFix);
        pushButtonMovePatch->setObjectName(QStringLiteral("pushButtonMovePatch"));
        pushButtonMovePatch->setGeometry(QRect(10, 450, 75, 23));
        horizontalSliderBrushRadiusImageCopy = new QSlider(tabFix);
        horizontalSliderBrushRadiusImageCopy->setObjectName(QStringLiteral("horizontalSliderBrushRadiusImageCopy"));
        horizontalSliderBrushRadiusImageCopy->setGeometry(QRect(100, 390, 301, 19));
        horizontalSliderBrushRadiusImageCopy->setMinimum(1);
        horizontalSliderBrushRadiusImageCopy->setMaximum(50);
        horizontalSliderBrushRadiusImageCopy->setValue(10);
        horizontalSliderBrushRadiusImageCopy->setOrientation(Qt::Horizontal);
        label_6 = new QLabel(tabFix);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 360, 71, 16));
        label_7 = new QLabel(tabFix);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(10, 390, 81, 16));
        label_8 = new QLabel(tabFix);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(10, 80, 54, 12));
        label_9 = new QLabel(tabFix);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(10, 150, 61, 16));
        horizontalSliderBrushRadiusImageInpaintHole = new QSlider(tabFix);
        horizontalSliderBrushRadiusImageInpaintHole->setObjectName(QStringLiteral("horizontalSliderBrushRadiusImageInpaintHole"));
        horizontalSliderBrushRadiusImageInpaintHole->setGeometry(QRect(100, 150, 301, 19));
        horizontalSliderBrushRadiusImageInpaintHole->setMinimum(1);
        horizontalSliderBrushRadiusImageInpaintHole->setMaximum(50);
        horizontalSliderBrushRadiusImageInpaintHole->setValue(10);
        horizontalSliderBrushRadiusImageInpaintHole->setOrientation(Qt::Horizontal);
        lineEditBrushRadiusImageInpaintHole = new QLineEdit(tabFix);
        lineEditBrushRadiusImageInpaintHole->setObjectName(QStringLiteral("lineEditBrushRadiusImageInpaintHole"));
        lineEditBrushRadiusImageInpaintHole->setGeometry(QRect(410, 150, 51, 20));
        pushButtonImageInpaintHoleResult = new QPushButton(tabFix);
        pushButtonImageInpaintHoleResult->setObjectName(QStringLiteral("pushButtonImageInpaintHoleResult"));
        pushButtonImageInpaintHoleResult->setGeometry(QRect(10, 250, 75, 23));
        label_10 = new QLabel(tabFix);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(10, 190, 71, 16));
        horizontalSliderBrushRadiusImageInpaintHoleRestrict = new QSlider(tabFix);
        horizontalSliderBrushRadiusImageInpaintHoleRestrict->setObjectName(QStringLiteral("horizontalSliderBrushRadiusImageInpaintHoleRestrict"));
        horizontalSliderBrushRadiusImageInpaintHoleRestrict->setGeometry(QRect(100, 190, 301, 19));
        horizontalSliderBrushRadiusImageInpaintHoleRestrict->setMinimum(1);
        horizontalSliderBrushRadiusImageInpaintHoleRestrict->setMaximum(50);
        horizontalSliderBrushRadiusImageInpaintHoleRestrict->setValue(10);
        horizontalSliderBrushRadiusImageInpaintHoleRestrict->setOrientation(Qt::Horizontal);
        lineEditBrushRadiusImageInpaintHoleRestrict = new QLineEdit(tabFix);
        lineEditBrushRadiusImageInpaintHoleRestrict->setObjectName(QStringLiteral("lineEditBrushRadiusImageInpaintHoleRestrict"));
        lineEditBrushRadiusImageInpaintHoleRestrict->setGeometry(QRect(410, 190, 51, 20));
        pushButtonImageInpaintHoleRestrictResult = new QPushButton(tabFix);
        pushButtonImageInpaintHoleRestrictResult->setObjectName(QStringLiteral("pushButtonImageInpaintHoleRestrictResult"));
        pushButtonImageInpaintHoleRestrictResult->setGeometry(QRect(110, 250, 131, 23));
        pushButtonErodeRadius = new QPushButton(tabFix);
        pushButtonErodeRadius->setObjectName(QStringLiteral("pushButtonErodeRadius"));
        pushButtonErodeRadius->setGeometry(QRect(10, 110, 141, 23));
        spinBoxErodeRadius = new QSpinBox(tabFix);
        spinBoxErodeRadius->setObjectName(QStringLiteral("spinBoxErodeRadius"));
        spinBoxErodeRadius->setGeometry(QRect(160, 110, 50, 20));
        spinBoxErodeRadius->setMinimumSize(QSize(50, 0));
        spinBoxErodeRadius->setMinimum(0);
        spinBoxErodeRadius->setMaximum(5);
        pushButtonImageInpaintHoleRestrictResultOut = new QPushButton(tabFix);
        pushButtonImageInpaintHoleRestrictResultOut->setObjectName(QStringLiteral("pushButtonImageInpaintHoleRestrictResultOut"));
        pushButtonImageInpaintHoleRestrictResultOut->setGeometry(QRect(260, 250, 161, 23));
        comboBox_ifGpu = new QComboBox(tabFix);
        comboBox_ifGpu->setObjectName(QStringLiteral("comboBox_ifGpu"));
        comboBox_ifGpu->setGeometry(QRect(50, 20, 61, 22));
        comboBox_ifGpu->setEditable(false);
        comboBox_ifGpu->setMaxCount(10);
        label_11 = new QLabel(tabFix);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(10, 20, 31, 16));
        pushButton_ClearRestrict = new QPushButton(tabFix);
        pushButton_ClearRestrict->setObjectName(QStringLiteral("pushButton_ClearRestrict"));
        pushButton_ClearRestrict->setGeometry(QRect(10, 210, 91, 23));
        pushButton_ClearRegion = new QPushButton(tabFix);
        pushButton_ClearRegion->setObjectName(QStringLiteral("pushButton_ClearRegion"));
        pushButton_ClearRegion->setGeometry(QRect(10, 410, 91, 23));
        tabWidget->addTab(tabFix, QString());
        tabColorMap = new QWidget();
        tabColorMap->setObjectName(QStringLiteral("tabColorMap"));
        pushButton_Source = new QPushButton(tabColorMap);
        pushButton_Source->setObjectName(QStringLiteral("pushButton_Source"));
        pushButton_Source->setGeometry(QRect(60, 70, 75, 23));
        pushButton_Target = new QPushButton(tabColorMap);
        pushButton_Target->setObjectName(QStringLiteral("pushButton_Target"));
        pushButton_Target->setGeometry(QRect(160, 70, 75, 23));
        pushButton_Result = new QPushButton(tabColorMap);
        pushButton_Result->setObjectName(QStringLiteral("pushButton_Result"));
        pushButton_Result->setGeometry(QRect(60, 140, 75, 23));
        pushButton_ResultGrayToRGB = new QPushButton(tabColorMap);
        pushButton_ResultGrayToRGB->setObjectName(QStringLiteral("pushButton_ResultGrayToRGB"));
        pushButton_ResultGrayToRGB->setGeometry(QRect(160, 140, 75, 23));
        tabWidget->addTab(tabColorMap, QString());

        gridLayout_2->addWidget(tabWidget, 0, 1, 1, 1);

        ImageWorkShopClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ImageWorkShopClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 502, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuStage = new QMenu(menuBar);
        menuStage->setObjectName(QStringLiteral("menuStage"));
        ImageWorkShopClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ImageWorkShopClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        ImageWorkShopClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ImageWorkShopClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ImageWorkShopClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuStage->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave_as);
        menuStage->addAction(actionStageSave);
        menuStage->addAction(actionStageBack);
        menuStage->addAction(actionImage_last_saved);

        retranslateUi(ImageWorkShopClass);

        tabWidget->setCurrentIndex(1);
        comboBox_Lomo->setCurrentIndex(-1);
        comboBox_ifGpu->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(ImageWorkShopClass);
    } // setupUi

    void retranslateUi(QMainWindow *ImageWorkShopClass)
    {
        ImageWorkShopClass->setWindowTitle(QApplication::translate("ImageWorkShopClass", "ImageWorkshop", 0));
        actionOpen->setText(QApplication::translate("ImageWorkShopClass", "open", 0));
        actionSave_as->setText(QApplication::translate("ImageWorkShopClass", "save as", 0));
        actionStageSave->setText(QApplication::translate("ImageWorkShopClass", "save", 0));
        actionStageBack->setText(QApplication::translate("ImageWorkShopClass", "back", 0));
        actionCompare->setText(QApplication::translate("ImageWorkShopClass", "compare", 0));
        actionImage_last_saved->setText(QApplication::translate("ImageWorkShopClass", "back image", 0));
        pushButtonMedian->setText(QApplication::translate("ImageWorkShopClass", "median", 0));
        labelVignette->setText(QApplication::translate("ImageWorkShopClass", "vignette", 0));
        labelGauss->setText(QApplication::translate("ImageWorkShopClass", " gauss ", 0));
        pushButtonSharp->setText(QApplication::translate("ImageWorkShopClass", "sharp", 0));
        labelBalance->setText(QApplication::translate("ImageWorkShopClass", "balance", 0));
        labelSaturation->setText(QApplication::translate("ImageWorkShopClass", "saturation", 0));
        pushButtonLiquify->setText(QApplication::translate("ImageWorkShopClass", "liquify", 0));
        pushButtonLiquifyRadius->setText(QApplication::translate("ImageWorkShopClass", "setRadius", 0));
        label_Lomo->setText(QApplication::translate("ImageWorkShopClass", "lomo", 0));
        labelLomoR->setText(QApplication::translate("ImageWorkShopClass", "R_mid", 0));
        labelGauss_3->setText(QApplication::translate("ImageWorkShopClass", "G_mid", 0));
        labelGauss_4->setText(QApplication::translate("ImageWorkShopClass", "B_mid", 0));
        label_5->setText(QApplication::translate("ImageWorkShopClass", "draw skinmask", 0));
        label_4->setText(QApplication::translate("ImageWorkShopClass", "filter radius", 0));
        pushButton_Exfoliating->setText(QApplication::translate("ImageWorkShopClass", "exfoliating", 0));
        label_3->setText(QApplication::translate("ImageWorkShopClass", "value dev", 0));
        label_2->setText(QApplication::translate("ImageWorkShopClass", "spatial dev", 0));
        label->setText(QApplication::translate("ImageWorkShopClass", "whiten skin", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabFilter), QApplication::translate("ImageWorkShopClass", "filter", 0));
        pushButtonMovePatch->setText(QApplication::translate("ImageWorkShopClass", "move patch", 0));
        label_6->setText(QApplication::translate("ImageWorkShopClass", "patch copy", 0));
        label_7->setText(QApplication::translate("ImageWorkShopClass", "select region", 0));
        label_8->setText(QApplication::translate("ImageWorkShopClass", "inpaint", 0));
        label_9->setText(QApplication::translate("ImageWorkShopClass", "add hole", 0));
        pushButtonImageInpaintHoleResult->setText(QApplication::translate("ImageWorkShopClass", "fill hole", 0));
        label_10->setText(QApplication::translate("ImageWorkShopClass", "add restric", 0));
        pushButtonImageInpaintHoleRestrictResult->setText(QApplication::translate("ImageWorkShopClass", "fill hole in restrict", 0));
        pushButtonErodeRadius->setText(QApplication::translate("ImageWorkShopClass", "set hole erode radius", 0));
        pushButtonImageInpaintHoleRestrictResultOut->setText(QApplication::translate("ImageWorkShopClass", "fill hole out of restrict", 0));
        label_11->setText(QApplication::translate("ImageWorkShopClass", "mode", 0));
        pushButton_ClearRestrict->setText(QApplication::translate("ImageWorkShopClass", "clear restrict", 0));
        pushButton_ClearRegion->setText(QApplication::translate("ImageWorkShopClass", "clear region", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabFix), QApplication::translate("ImageWorkShopClass", "patchmatch", 0));
        pushButton_Source->setText(QApplication::translate("ImageWorkShopClass", "source", 0));
        pushButton_Target->setText(QApplication::translate("ImageWorkShopClass", "target", 0));
        pushButton_Result->setText(QApplication::translate("ImageWorkShopClass", "show result", 0));
        pushButton_ResultGrayToRGB->setText(QApplication::translate("ImageWorkShopClass", "gray2rgb", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabColorMap), QApplication::translate("ImageWorkShopClass", "color map", 0));
        menuFile->setTitle(QApplication::translate("ImageWorkShopClass", "File", 0));
        menuStage->setTitle(QApplication::translate("ImageWorkShopClass", "stage", 0));
    } // retranslateUi

};

namespace Ui {
    class ImageWorkShopClass: public Ui_ImageWorkShopClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEWORKSHOP_H
