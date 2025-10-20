#include "mainwindow.h"
#include <QInputDialog>
#include <QPainter>
#include <QPen>
#include <QGraphicsDropShadowEffect>
#include <cmath>
#include <map>
#include <QStyle>
#include <QDebug>
#include <cstring>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <random>

using namespace std;

// --- ImageLabel Implementation ---
ImageLabel::ImageLabel(QWidget *parent)
    : QLabel(parent), cropMode(false), selecting(false) {
    setMouseTracking(true);
    setAlignment(Qt::AlignCenter);
    setStyleSheet("background: transparent;");
}

void ImageLabel::setCropMode(bool enabled) {
    cropMode = enabled;
    if (!enabled) {
        cropRect = QRect();
        selecting = false;
        setCursor(Qt::ArrowCursor);
    } else {
        setCursor(Qt::CrossCursor);
    }
    update();
}

void ImageLabel::clearCropRect() {
    cropRect = QRect();
    update();
}

void ImageLabel::mousePressEvent(QMouseEvent *event) {
    if (cropMode && event->button() == Qt::LeftButton) {
        origin = event->pos();
        cropRect = QRect(origin, QSize());
        selecting = true;
        update();
    }
    QLabel::mousePressEvent(event);
}

void ImageLabel::mouseMoveEvent(QMouseEvent *event) {
    if (cropMode && selecting) {
        cropRect = QRect(origin, event->pos()).normalized();
        update();
    }
    QLabel::mouseMoveEvent(event);
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *event) {
    if (cropMode && event->button() == Qt::LeftButton && selecting) {
        selecting = false;
        if (cropRect.width() > 10 && cropRect.height() > 10) {
            emit cropAreaSelected(cropRect);
        } else {
            cropRect = QRect();
            update();
        }
    }
    QLabel::mouseReleaseEvent(event);
}

void ImageLabel::paintEvent(QPaintEvent *event) {
    QLabel::paintEvent(event);
    if (cropMode && !cropRect.isNull()) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(QColor(255, 107, 107), 3, Qt::DashLine));
        painter.setBrush(QColor(0, 0, 0, 150));
        QRegion maskRegion = rect();
        maskRegion -= cropRect;
        painter.setClipRegion(maskRegion);
        painter.drawRect(rect());

        painter.setClipRect(rect());
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor(255, 107, 107), 3, Qt::DashLine));
        painter.drawRect(cropRect);
    }
}

// --- FilterButton Implementation ---
FilterButton::FilterButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent) {
    setMinimumHeight(60);
    setCursor(Qt::PointingHandCursor);
    setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "       stop:0 #5DADE2, stop:1 #4A235A);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 15px;"
        "   padding: 15px;"
        "   font-size: 15px;"
        "   font-weight: bold;"
        "   text-align: left;"
        "   padding-left: 20px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "       stop:0 #85C1E9, stop:1 #6C3483);"
        "   border: 2px solid #D1F2EB;"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "       stop:0 #3498DB, stop:1 #3C1053);"
        "}"
        );
}

// --- FrameDialog Implementation ---
void FrameDialog::updateColorButton(QPushButton* button, const QColor& color) {
    button->setStyleSheet(
        QString("QPushButton { padding: 10px; border-radius: 8px; font-weight: bold; color: white; background-color: %1; border: 2px solid white; }")
            .arg(color.name())
        );
    button->setText(color.name());
}

FrameDialog::FrameDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("🎨 Frame Designer");
    setMinimumWidth(450);

    color1 = QColor(25, 118, 210);
    color2 = QColor(255, 179, 0);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("✨ Design Your Frame");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #5DADE2;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    typeCombo = new QComboBox();
    typeCombo->addItem("🖼️ Simple Elegant Frame (Color/Thickness)");
    typeCombo->addItem("🌟 Double Luxury Frame (Two Separate Layers)");
    typeCombo->addItem("🌈 Rainbow Fantasy Frame (Creative)");
    typeCombo->setStyleSheet(
        "QComboBox { padding: 15px; font-size: 15px; border-radius: 10px; "
        "background: #1C2833; color: white; border: 2px solid #5DADE2; font-weight: bold; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background: #212F3C; color: white; selection-background-color: #5DADE2; }"
        );
    layout->addWidget(typeCombo);

    QGridLayout *optionsGrid = new QGridLayout();
    optionsGrid->setSpacing(15);
    layout->addLayout(optionsGrid);

    QLabel *color1Label = new QLabel("Outer Color:");
    color1Label->setStyleSheet("color: #E5E7E9; font-size: 14px; font-weight: bold;");
    optionsGrid->addWidget(color1Label, 0, 0);

    color1Btn = new QPushButton();
    updateColorButton(color1Btn, color1);
    optionsGrid->addWidget(color1Btn, 0, 1);

    connect(color1Btn, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(color1, this, "Choose Outer Frame Color");
        if (c.isValid()) {
            color1 = c;
            updateColorButton(color1Btn, color1);
        }
    });

    QLabel *thick1Label = new QLabel("Outer Thickness (px):");
    thick1Label->setStyleSheet("color: #E5E7E9; font-size: 14px; font-weight: bold;");
    optionsGrid->addWidget(thick1Label, 1, 0);

    thickness1Spin = new QSpinBox();
    thickness1Spin->setRange(5, 150);
    thickness1Spin->setValue(20);
    thickness1Spin->setStyleSheet(
        "QSpinBox { padding: 10px; background: #212F3C; color: white; "
        "border-radius: 8px; font-size: 14px; border: 2px solid #5DADE2; }"
        );
    optionsGrid->addWidget(thickness1Spin, 1, 1);

    QLabel *color2Label = new QLabel("Inner Color:");
    color2Label->setStyleSheet("color: #E5E7E9; font-size: 14px; font-weight: bold;");
    optionsGrid->addWidget(color2Label, 2, 0);

    color2Btn = new QPushButton();
    updateColorButton(color2Btn, color2);
    optionsGrid->addWidget(color2Btn, 2, 1);

    connect(color2Btn, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(color2, this, "Choose Inner Frame Color");
        if (c.isValid()) {
            color2 = c;
            updateColorButton(color2Btn, color2);
        }
    });

    QLabel *thick2Label = new QLabel("Inner Thickness (px):");
    thick2Label->setStyleSheet("color: #E5E7E9; font-size: 14px; font-weight: bold;");
    optionsGrid->addWidget(thick2Label, 3, 0);

    thickness2Spin = new QSpinBox();
    thickness2Spin->setRange(5, 100);
    thickness2Spin->setValue(12);
    thickness2Spin->setStyleSheet(
        "QSpinBox { padding: 10px; background: #212F3C; color: white; "
        "border-radius: 8px; font-size: 14px; border: 2px solid #5DADE2; }"
        );
    optionsGrid->addWidget(thickness2Spin, 3, 1);

    auto setSecondaryVisibility = [=](int index) {
        bool simple = (index == 0);
        bool dual = (index == 1);

        color1Label->setVisible(simple || dual);
        color1Btn->setVisible(simple || dual);
        thick1Label->setVisible(simple || dual);
        thickness1Spin->setVisible(simple || dual);

        color2Label->setVisible(dual);
        color2Btn->setVisible(dual);
        thick2Label->setVisible(dual);
        thickness2Spin->setVisible(dual);
    };

    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), setSecondaryVisibility);

    setSecondaryVisibility(typeCombo->currentIndex());

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(15);

    QPushButton *cancelBtn = new QPushButton("✖ Cancel");
    QPushButton *okBtn = new QPushButton("✓ Apply Frame");

    cancelBtn->setStyleSheet(
        "QPushButton { padding: 15px 30px; background: #566573; color: white; "
        "border-radius: 10px; font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background: #4A5568; }"
        );

    okBtn->setStyleSheet(
        "QPushButton { padding: 15px 30px; background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #5DADE2, stop:1 #27AE60); color: white; border-radius: 10px; "
        "font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #85C1E9, stop:1 #2ECC71); }"
        );

    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    layout->addLayout(btnLayout);

    setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #17202A, stop:1 #0D1117); }"
        );
}

int FrameDialog::getFrameType() const { return typeCombo->currentIndex(); }
QColor FrameDialog::getColor1() const { return color1; }
QColor FrameDialog::getColor2() const { return color2; }
int FrameDialog::getThickness1() const { return thickness1Spin->value(); }
int FrameDialog::getThickness2() const { return thickness2Spin->value(); }

// --- MainWindow Setup and Slots (Updated) ---
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), hasImage(false), historyIndex(-1), cropMode(false) {
    setupUI();
    setWindowTitle("✨ Pixify Pro - Ultimate Image Studio");
    resize(1600, 1000);
    updateHistoryButtons();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Left Panel (Filters)
    leftPanel = new QWidget();
    leftPanel->setFixedWidth(420);
    leftPanel->setStyleSheet(
        "QWidget { background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "stop:0 #17202A, stop:1 #0D1117); }"
        );

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(25, 25, 25, 25);
    leftLayout->setSpacing(15);

    QLabel *titleLabel = new QLabel("✨FILTERS");
    titleLabel->setStyleSheet(
        "color: white; font-size: 26px; font-weight: bold; "
        "padding: 20px; background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #5DADE2, stop:1 #4A235A); "
        "border-radius: 15px; text-align: center;"
        );
    titleLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(titleLabel);

    QScrollArea *filterScroll = new QScrollArea();
    filterScroll->setWidgetResizable(true);
    filterScroll->setStyleSheet(
        "QScrollArea { border: none; background: transparent; }"
        "QScrollBar:vertical { background: #212F3C; width: 12px; border-radius: 6px; }"
        "QScrollBar::handle:vertical { background: #5DADE2; border-radius: 6px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        );

    QWidget *filterContainer = new QWidget();
    QGridLayout *filterGrid = new QGridLayout(filterContainer);
    filterGrid->setSpacing(15);
    filterGrid->setContentsMargins(0, 0, 0, 0);

    // Filter Buttons (20 total, including 1 placeholder)
    FilterButton *btnGray      = new FilterButton("🩶 Grayscale");
    FilterButton *btnBW        = new FilterButton("⚫⚪ Black & White");
    FilterButton *btnTV        = new FilterButton("📺 Old TV");
    FilterButton *btnInvert    = new FilterButton("🔄 Invert ");
    FilterButton *btnEdge      = new FilterButton("✴️ Edge Detect");
    FilterButton *btnOil       = new FilterButton("🖌️ Oil Painting");
    FilterButton *btnPurple    = new FilterButton("🌌 Night Purple");
    FilterButton *btnInfrared  = new FilterButton("🔴 Infrared");
    FilterButton *btnFlip      = new FilterButton("↕️↔️ Flip ");
    FilterButton *btnSkew      = new FilterButton("⚡ Skew ");
    FilterButton *btnBright    = new FilterButton("💡 Brightness");
    FilterButton *btnCrop      = new FilterButton("✂️ Crop");
    FilterButton *btnBlur      = new FilterButton("🌫️ Blur");
    FilterButton *btnResize    = new FilterButton("📐 Resize");
    FilterButton *btnMerge     = new FilterButton("🧩 Merge");
    FilterButton *btnNeon      = new FilterButton("💥 Neon");
    FilterButton *btnFrame     = new FilterButton("🖼️ Frame");
    FilterButton *btnSolarize  = new FilterButton("☀️ Solarize ");

    // Placeholder button for 20 total, to keep layout consistent
    FilterButton *btnEmpty     = new FilterButton(" ");
    btnEmpty->setStyleSheet("background: transparent; border: none;");


    connect(btnGray, &QPushButton::clicked, this, &MainWindow::applyGrayscale);
    connect(btnBW, &QPushButton::clicked, this, &MainWindow::applyBlackAndWhite);
    connect(btnTV, &QPushButton::clicked, this, &MainWindow::applyTVEffect);
    connect(btnInvert, &QPushButton::clicked, this, &MainWindow::applyInvert);
    connect(btnEdge, &QPushButton::clicked, this, &MainWindow::applyEdgeDetection);
    connect(btnOil, &QPushButton::clicked, this, &MainWindow::applyOilPaint);
    connect(btnPurple, &QPushButton::clicked, this, &MainWindow::applyNightPurple);
    connect(btnInfrared, &QPushButton::clicked, this, &MainWindow::applyInfrared);
    connect(btnFlip, &QPushButton::clicked, this, &MainWindow::applyFlip);
    connect(btnSkew, &QPushButton::clicked, this, &MainWindow::showSkewControl);
    connect(btnBright, &QPushButton::clicked, this, &MainWindow::showBrightnessControl);
    connect(btnCrop, &QPushButton::clicked, this, &MainWindow::startCropSelection);
    connect(btnBlur, &QPushButton::clicked, this, &MainWindow::showBlurControl);
    connect(btnResize, &QPushButton::clicked, this, &MainWindow::applyResize);
    connect(btnMerge, &QPushButton::clicked, this, &MainWindow::applyMerge);
    connect(btnNeon, &QPushButton::clicked, this, &MainWindow::applyNeon);
    connect(btnFrame, &QPushButton::clicked, this, &MainWindow::applyFrame);
    connect(btnSolarize, &QPushButton::clicked, this, &MainWindow::applySolarize);

    int row = 0, col = 0;
    auto addBtn = [&](FilterButton* btn) {
        filterGrid->addWidget(btn, row, col);
        col++;
        if (col >= 2) { col = 0; row++; }
    };

    addBtn(btnGray); addBtn(btnBW); addBtn(btnInvert); addBtn(btnEdge);
    addBtn(btnBright); addBtn(btnBlur); addBtn(btnSkew); addBtn(btnOil);
    addBtn(btnTV); addBtn(btnPurple); addBtn(btnInfrared); addBtn(btnNeon);
    addBtn(btnFrame); addBtn(btnCrop); addBtn(btnResize); addBtn(btnMerge);
    addBtn(btnFlip); addBtn(btnSolarize);
    addBtn(btnEmpty);

    filterScroll->setWidget(filterContainer);
    leftLayout->addWidget(filterScroll, 1);
    mainLayout->addWidget(leftPanel);

    // Right Panel (Image Display Area)
    QWidget *rightPanel = new QWidget();
    rightPanel->setStyleSheet(
        "QWidget { background: #1C2833; }"
        );

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(30, 30, 30, 30);
    rightLayout->setSpacing(20);

    // --- History/Undo/Redo (New Location: Above the main image) ---
    QHBoxLayout *historyLayout = new QHBoxLayout();
    historyLayout->setSpacing(10);

    // Undo/Redo Buttons
    undoBtn = new QPushButton("↶");
    redoBtn = new QPushButton("↷");

    QString histBtnStyle =
        "QPushButton { background: rgba(93, 173, 226, 0.4); color: white; border: none; "
        "border-radius: 8px; font-size: 20px; font-weight: bold; padding: 5px 12px; min-height: 35px; }"
        "QPushButton:hover { background: #5DADE2; }"
        "QPushButton:disabled { background: #34495E; color: #566573; }";

    undoBtn->setStyleSheet(histBtnStyle);
    redoBtn->setStyleSheet(histBtnStyle);

    connect(undoBtn, &QPushButton::clicked, this, &MainWindow::undo);
    connect(redoBtn, &QPushButton::clicked, this, &MainWindow::redo);

    historyLayout->addWidget(undoBtn);
    historyLayout->addWidget(redoBtn);
    historyLayout->addStretch();

    imageSizeLabel = new QLabel("📸 No image loaded - Start your creative journey!");
    imageSizeLabel->setStyleSheet(
        "color: white; font-size: 18px; font-weight: bold; "
        "padding: 10px 18px; background: rgba(93, 173, 226, 0.2); "
        "border: 2px solid #5DADE2; border-radius: 12px;"
        );
    imageSizeLabel->setAlignment(Qt::AlignCenter);
    historyLayout->addWidget(imageSizeLabel);

    rightLayout->addLayout(historyLayout);
    // ----------------------------------------

    scrollArea = new QScrollArea();
    scrollArea->setStyleSheet(
        "QScrollArea { border: 3px solid rgba(93, 173, 226, 0.5); "
        "background: rgba(0,0,0,0.3); border-radius: 20px; }"
        "QScrollBar:vertical { background: #212F3C; width: 14px; border-radius: 7px; }"
        "QScrollBar::handle:vertical { background: #5DADE2; border-radius: 7px; }"
        "QScrollBar:horizontal { background: #212F3C; height: 14px; border-radius: 7px; }"
        "QScrollBar::handle:horizontal { background: #5DADE2; border-radius: 7px; }"
        );
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);

    imageLabel = new ImageLabel();
    scrollArea->setWidget(imageLabel);
    connect(imageLabel, &ImageLabel::cropAreaSelected, this, &MainWindow::finishCrop);

    rightLayout->addWidget(scrollArea, 1);

    setupControlPanel();
    rightLayout->addWidget(controlWrapper);

    // **Style Variable for Main Buttons**
    QString actionBtnStyle =
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "stop:0 #2C3E50, stop:1 #34495E); color: white; border: none; "
        "border-radius: 15px; font-size: 16px; font-weight: bold; "
        "padding: 15px 25px; min-height: 50px; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "stop:0 #34495E, stop:1 #566573); }"
        "QPushButton:pressed { background: #1C2833; }";


    // 1. Rotate Row (Centered)
    QHBoxLayout *rotateLayout = new QHBoxLayout();
    rotateLayout->setSpacing(20);

    QPushButton *rotateLeft = new QPushButton("⟲"); // 90 CCW
    QPushButton *rotateRight = new QPushButton("⟳"); // 90 CW

    QString rotateBtnStyle = actionBtnStyle.replace("16px", "24px").replace("25px", "15px");
    rotateLeft->setStyleSheet(rotateBtnStyle.replace("#2C3E50", "#4A235A").replace("#34495E", "#5DADE2"));
    rotateRight->setStyleSheet(rotateBtnStyle.replace("#2C3E50", "#4A235A").replace("#34495E", "#5DADE2"));

    connect(rotateLeft, &QPushButton::clicked, this, &MainWindow::applyRotate90CCW);
    connect(rotateRight, &QPushButton::clicked, this, &MainWindow::applyRotate90CW);

    rotateLayout->addStretch();
    rotateLayout->addWidget(rotateLeft);
    rotateLayout->addWidget(rotateRight);
    rotateLayout->addStretch();

    rightLayout->addLayout(rotateLayout);

    // 2. Main Action Row (Centered)
    QHBoxLayout *mainActionsLayout = new QHBoxLayout();
    mainActionsLayout->setSpacing(20);

    QPushButton *loadBtn = new QPushButton("📁 Load Image");
    QPushButton *saveBtn = new QPushButton("💾 Save Image");
    QPushButton *deleteBtn = new QPushButton("🗑️ Delete");
    QPushButton *clearBtn = new QPushButton("🧹 Reset Filters");

    // Apply main styles using the now-accessible actionBtnStyle
    loadBtn->setStyleSheet(actionBtnStyle.replace("#2C3E50", "#5DADE2").replace("#34495E", "#4A235A"));
    saveBtn->setStyleSheet(actionBtnStyle.replace("#2C3E50", "#85C1E9").replace("#34495E", "#5DADE2"));
    deleteBtn->setStyleSheet(actionBtnStyle.replace("#2C3E50", "#E74C3C").replace("#34495E", "#CB4335"));
    clearBtn->setStyleSheet(actionBtnStyle);

    mainActionsLayout->addStretch();
    mainActionsLayout->addWidget(loadBtn);
    mainActionsLayout->addWidget(saveBtn);
    mainActionsLayout->addWidget(deleteBtn);
    mainActionsLayout->addWidget(clearBtn);
    mainActionsLayout->addStretch();

    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveImage);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteImage);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearFilters);

    rightLayout->addLayout(mainActionsLayout);
    mainLayout->addWidget(rightPanel, 1);
    setStyleSheet("QMainWindow { background: #0D1117; }");
}

void MainWindow::setupControlPanel() {
    controlWrapper = new QWidget();
    QVBoxLayout *wrapperLayout = new QVBoxLayout(controlWrapper);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);

    controlStack = new QStackedWidget();
    controlStack->setStyleSheet(
        "QStackedWidget > QWidget { background: transparent; }"
        );

    // --- Brightness Widget ---
    QWidget *brightnessWidget = new QWidget();
    QVBoxLayout *brightLayout = new QVBoxLayout(brightnessWidget);
    QLabel *brightLabel = new QLabel("💡 Adjust Brightness Level ( -100% to +100%)");
    brightLabel->setStyleSheet("color: #5DADE2; font-size: 20px; font-weight: bold;");
    brightnessSlider = new QSlider(Qt::Horizontal);
    brightnessSlider->setRange(-100, 100);
    brightnessSlider->setValue(0);
    brightnessSlider->setMinimumHeight(35); // Slightly smaller overall slider area
    brightnessSlider->setStyleSheet(
        "QSlider::groove:horizontal { background: #212F3C; height: 10px; border-radius: 5px; }" // Reduced groove height
        "QSlider::handle:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "stop:0 #5DADE2, stop:1 #4A235A); width: 20px; height: 20px; " // Reduced handle size
        "margin: -5px 0; border-radius: 10px; }" // Adjusted margin/radius to center handle
        "QSlider::sub-page:horizontal { background: #5DADE2; border-radius: 5px; }"
        );
    brightnessValueLabel = new QLabel("0%");
    brightnessValueLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    brightnessValueLabel->setAlignment(Qt::AlignCenter);
    connect(brightnessSlider, &QSlider::valueChanged, [this](int val) {
        brightnessValueLabel->setText(QString::number(val) + "%");
        applyBrightnessValue(val);
    });
    brightLayout->addWidget(brightLabel);
    brightLayout->addWidget(brightnessValueLabel);
    brightLayout->addWidget(brightnessSlider);

    // --- Blur Widget ---
    QWidget *blurWidget = new QWidget();
    QVBoxLayout *blurLayout = new QVBoxLayout(blurWidget);
    QLabel *blurLabel = new QLabel("🌫️ Blur Intensity (Radius)");
    blurLabel->setStyleSheet("color: #5DADE2; font-size: 20px; font-weight: bold;");
    blurSlider = new QSlider(Qt::Horizontal);
    blurSlider->setRange(1, 15);
    blurSlider->setValue(5);
    blurSlider->setMinimumHeight(35); // Slightly smaller overall slider area
    blurSlider->setStyleSheet(
        "QSlider::groove:horizontal { background: #212F3C; height: 10px; border-radius: 5px; }"
        "QSlider::handle:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "stop:0 #5DADE2, stop:1 #4A235A); width: 20px; height: 20px; "
        "margin: -5px 0; border-radius: 10px; }"
        "QSlider::sub-page:horizontal { background: #5DADE2; border-radius: 5px; }"
        );
    blurValueLabel = new QLabel("5");
    blurValueLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    blurValueLabel->setAlignment(Qt::AlignCenter);
    connect(blurSlider, &QSlider::valueChanged, [this](int val) {
        blurValueLabel->setText(QString::number(val));
        applyBlurValue(val);
    });
    blurLayout->addWidget(blurLabel);
    blurLayout->addWidget(blurValueLabel);
    blurLayout->addWidget(blurSlider);

    // --- Skew Widget ---
    QWidget *skewWidget = new QWidget();
    QVBoxLayout *skewLayout = new QVBoxLayout(skewWidget);
    QLabel *skewLabel = new QLabel("⚡ Skew Angle (Horizontal Shear)");
    skewLabel->setStyleSheet("color: #5DADE2; font-size: 20px; font-weight: bold;");

    skewSpinBox = new QDoubleSpinBox();
    skewSpinBox->setRange(-45.0, 45.0);
    skewSpinBox->setValue(0.0);
    skewSpinBox->setSuffix("°");
    skewSpinBox->setDecimals(1);
    skewSpinBox->setSingleStep(0.5);
    skewSpinBox->setMinimumHeight(50);
    skewSpinBox->setStyleSheet(
        "QDoubleSpinBox { padding: 15px; background: #17202A; color: white; "
        "border-radius: 10px; font-size: 18px; font-weight: bold; "
        "border: 2px solid #5DADE2; }"
        );
    connect(skewSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::applySkewValue);
    skewLayout->addWidget(skewLabel);
    skewLayout->addWidget(skewSpinBox);

    // --- Add to Stack ---
    controlStack->addWidget(brightnessWidget);
    controlStack->addWidget(blurWidget);
    controlStack->addWidget(skewWidget);

    // --- Apply Button ---
    applyBtn = new QPushButton("✓ Apply Effect");
    applyBtn->setMinimumHeight(55);
    applyBtn->setCursor(Qt::PointingHandCursor);
    applyBtn->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "stop:0 #5DADE2, stop:1 #85C1E9); color: white; border: none; "
        "border-radius: 12px; font-size: 18px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "stop:0 #85C1E9, stop:1 #5DADE2); }"
        "QPushButton:pressed { background: #3498DB; }"
        );

    controlPanel = new QWidget();
    controlPanel->setStyleSheet(
        "QWidget { background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "stop:0 #212F3C, stop:1 #1C2833); "
        "border-radius: 20px; padding: 25px; border: 2px solid #5DADE2; }"
        );
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
    controlLayout->setSpacing(20);
    controlLayout->addWidget(controlStack);

    wrapperLayout->addWidget(controlPanel);
    wrapperLayout->addWidget(applyBtn);
    controlWrapper->setVisible(false);
}

void MainWindow::loadImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "",
                                                    "Images (*.png *.jpg *.jpeg *.bmp *.tga)");

    if (fileName.isEmpty()) return;

    try {
        currentImage.loadNewImage(fileName.toStdString());
        currentFileName = fileName;
        hasImage = true;

        historyStates.clear();
        historyIndex = -1;
        pushHistory();

        displayImageCentered();
        updateHistoryButtons();
        QMessageBox::information(this, "✓ Success", "Image loaded successfully!");
    } catch (const std::invalid_argument& e) {
        QMessageBox::critical(this, "✗ Error", QString("Failed to load image: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "✗ Error", "An unknown error occurred while loading the image!");
    }
}

void MainWindow::saveImage() {
    if (!hasImage) return;

    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "",
                                                    "PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp);;TGA (*.tga)");

    if (fileName.isEmpty()) return;

    try {
        currentImage.saveImage(fileName.toStdString());
        QMessageBox::information(this, "✓ Success", "Image saved successfully!");
    } catch (const std::invalid_argument& e) {
        QMessageBox::critical(this, "✗ Error", QString("Failed to save image: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "✗ Error", "An unknown error occurred while saving the image!");
    }
}

void MainWindow::displayImageCentered() {
    if (!hasImage) return;

    QImage qimg(currentImage.imageData, currentImage.width, currentImage.height,
                currentImage.width * currentImage.channels, QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage(qimg);

    QSize availableSize = scrollArea->viewport()->size();
    availableSize -= QSize(20, 20);

    QPixmap scaledPixmap;
    scaledPixmap = pixmap.scaled(availableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    imageLabel->setPixmap(scaledPixmap);
    imageLabel->setFixedSize(scaledPixmap.size());
    imageLabel->setAlignment(Qt::AlignCenter);

    // Removed channel info
    imageSizeLabel->setText(QString("📐 %1 × %2 pixels")
                                .arg(currentImage.width)
                                .arg(currentImage.height));
}

void MainWindow::updateHistoryButtons() {
    undoBtn->setEnabled(historyIndex > 0);
    redoBtn->setEnabled(historyIndex < (int)historyStates.size() - 1);
}

void MainWindow::pushHistory() {
    if (historyIndex + 1 < (int)historyStates.size()) {
        historyStates.erase(historyStates.begin() + historyIndex + 1, historyStates.end());
    }
    if ((int)historyStates.size() >= MAX_HISTORY) {
        historyStates.erase(historyStates.begin());
    }
    historyStates.push_back(currentImage);
    historyIndex = historyStates.size() - 1;
    updateHistoryButtons();
}

void MainWindow::undo() {
    if (historyIndex <= 0) {
        QMessageBox::information(this, "ℹ Info", "Nothing to undo!");
        return;
    }
    historyIndex--;
    currentImage = historyStates[historyIndex];
    displayImageCentered();
    updateHistoryButtons();
}

void MainWindow::redo() {
    if (historyIndex + 1 >= (int)historyStates.size()) {
        QMessageBox::information(this, "ℹ Info", "Nothing to redo!");
        return;
    }
    historyIndex++;
    currentImage = historyStates[historyIndex];
    displayImageCentered();
    updateHistoryButtons();
}

// --- Rotate Slots ---
void MainWindow::applyRotate90CW() {
    if (!hasImage) return;
    Rotate(currentImage, 90);
    pushHistory();
    displayImageCentered();
}

void MainWindow::applyRotate90CCW() {
    if (!hasImage) return;
    Rotate(currentImage, 270);
    pushHistory();
    displayImageCentered();
}

// --- Control Panel Slots ---

void MainWindow::showBrightnessControl() {
    if (!hasImage) return;
    hideControlPanel();
    previewImage = currentImage;
    brightnessSlider->setRange(-100, 100);
    brightnessSlider->setValue(0);
    controlStack->setCurrentIndex(0);
    controlWrapper->setVisible(true);
    disconnect(applyBtn, nullptr, nullptr, nullptr);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::applyBrightness);
}

void MainWindow::showBlurControl() {
    if (!hasImage) return;
    hideControlPanel();
    previewImage = currentImage;
    blurSlider->setValue(5);
    controlStack->setCurrentIndex(1);
    controlWrapper->setVisible(true);
    disconnect(applyBtn, nullptr, nullptr, nullptr);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::applyBlur);
}

void MainWindow::showSkewControl() {
    if (!hasImage) return;
    hideControlPanel();
    previewImage = currentImage;
    skewSpinBox->setValue(0.0);
    controlStack->setCurrentIndex(2);
    controlWrapper->setVisible(true);
    disconnect(applyBtn, nullptr, nullptr, nullptr);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::applySkew);
}

void MainWindow::hideControlPanel() {
    if (!controlWrapper->isVisible()) return;

    controlWrapper->setVisible(false);

    if(hasImage) {
        if(historyIndex >= 0 && (size_t)historyIndex < historyStates.size()) {
            currentImage = historyStates[historyIndex];
            displayImageCentered();
        }
    }
}

// --- Preview Value Functions (Apply filter to a copy for real-time display) ---

void MainWindow::applyBrightnessValue(int value) {
    if (!hasImage) return;
    Image tempImage = previewImage;
    Brightness(tempImage, value);
    currentImage = tempImage;
    displayImageCentered();
}

void MainWindow::applyBlurValue(int value) {
    if (!hasImage) return;
    Image tempImage = previewImage;
    Blur(tempImage, value);
    currentImage = tempImage;
    displayImageCentered();
}

void MainWindow::applySkewValue(double value) {
    if (!hasImage) return;
    Image tempImage = previewImage;
    Skew(tempImage, value);
    currentImage = tempImage;
    displayImageCentered();
}

// --- Apply (Finalize) Functions ---

void MainWindow::applyBrightness() {
    Brightness(currentImage, brightnessSlider->value());
    pushHistory();
    hideControlPanel();
}

void MainWindow::applyBlur() {
    Blur(currentImage, blurSlider->value());
    pushHistory();
    hideControlPanel();
}

void MainWindow::applySkew() {
    Skew(currentImage, skewSpinBox->value());
    pushHistory();
    hideControlPanel();
}

// --- Simple Filter Slots (Retained/Updated Logic) ---

void MainWindow::applyGrayscale() {
    if (!hasImage) return;
    Grayscale(currentImage);
    pushHistory();
    displayImageCentered();
}

void MainWindow::applyBlackAndWhite() {
    if (!hasImage) return;
    BlackAndWhite(currentImage);
    pushHistory();
    displayImageCentered();
}

void MainWindow::applyInvert() {
    if (!hasImage) return;
    Invert(currentImage);
    pushHistory();
    displayImageCentered();
}

void MainWindow::applyFlip() {
    if (!hasImage) return;
    QDialog dialog(this);
    dialog.setWindowTitle("Choose Flip Direction");
    dialog.setMinimumWidth(400);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setSpacing(20);
    layout->setContentsMargins(30, 30, 30, 30);

    QLabel *title = new QLabel("Select flip direction:");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    layout->addWidget(title);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(15);

    QPushButton *hBtn = new QPushButton("↔️\nHorizontal");
    QPushButton *vBtn = new QPushButton("↕️\nVertical");

    QString flipBtnStyle =
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "stop:0 #5DADE2, stop:1 #4A235A); color: white; border: none; "
        "border-radius: 15px; font-size: 24px; font-weight: bold; "
        "padding: 40px; min-width: 150px; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "stop:0 #85C1E9, stop:1 #6C3483); }";

    hBtn->setStyleSheet(flipBtnStyle);
    vBtn->setStyleSheet(flipBtnStyle);
    hBtn->setCursor(Qt::PointingHandCursor);
    vBtn->setCursor(Qt::PointingHandCursor);

    btnLayout->addWidget(hBtn);
    btnLayout->addWidget(vBtn);
    layout->addLayout(btnLayout);

    dialog.setStyleSheet("QDialog { background: #1a202c; }");

    connect(hBtn, &QPushButton::clicked, [&]() {
        FlipHorizontal(currentImage);
        dialog.accept();
    });

    connect(vBtn, &QPushButton::clicked, [&]() {
        FlipVertical(currentImage);
        dialog.accept();
    });

    if(dialog.exec() == QDialog::Accepted) {
        pushHistory();
        displayImageCentered();
    }
}

void MainWindow::applyEdgeDetection() {
    if (!hasImage) return;
    EdgeDetect(currentImage);
    pushHistory();
    displayImageCentered();
}

void MainWindow::applyOilPaint() {
    if (!hasImage) return;
    OilPaint(currentImage);
    pushHistory();
    displayImageCentered();
}

void MainWindow::applyTVEffect() {
    if (!hasImage) return;
    TVEffect(currentImage);
    pushHistory();
    displayImageCentered();
}

void MainWindow::applyMerge() {
    if (!hasImage) return;
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image to Merge", "",
                                                    "Images (*.png *.jpg *.jpeg *.bmp *.tga)");

    if (fileName.isEmpty()) return;

    try {
        Image other(fileName.toStdString());
        currentImage = MergeImages(currentImage, other);
        pushHistory();
        displayImageCentered();
    } catch (const std::invalid_argument& e) {
        QMessageBox::critical(this, "✗ Error", QString("Failed to merge images: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "✗ Error", "An unknown error occurred while merging images!");
    }
}

void MainWindow::applyFrame() {
    if (!hasImage) return;

    FrameDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        int type = dialog.getFrameType();

        if (type == 0) {
            // Simple frame - outer only
            Frame(currentImage, dialog.getThickness1(), dialog.getColor1());
        } else if (type == 1) {
            // Double frame - FIXED ORDER: Inner first, then Outer
            FrameDouble(currentImage, dialog.getThickness2(), dialog.getThickness1(),
                        dialog.getColor2(), dialog.getColor1());
        } else if (type == 2) {
            FrameFancy(currentImage);
        }

        pushHistory();
        displayImageCentered();
    }
}

void MainWindow::applyResize() {
    if (!hasImage) return;

    bool ok1, ok2;
    int newWidth = QInputDialog::getInt(this, "📐 Resize Width",
                                        "Enter new width:",
                                        currentImage.width, 1, 10000, 1, &ok1);
    if (!ok1) return;

    int newHeight = QInputDialog::getInt(this, "📐 Resize Height",
                                         "Enter new height:",
                                         currentImage.height, 1, 10000, 1, &ok2);
    if (!ok2) return;

    Resize(currentImage, newWidth, newHeight);
    pushHistory();
    displayImageCentered();
}

void MainWindow::startCropSelection() {
    if (!hasImage) return;
    cropMode = true;
    imageLabel->setCropMode(true);
    QMessageBox::information(this, "✂️ Crop Mode",
                             "Draw a rectangle on the image to select the area to crop.\n"
                             "Click and drag to select the region.");
}

void MainWindow::finishCrop(QRect rect) {
    if (!hasImage || !cropMode) return;

    QPixmap pixmap = imageLabel->pixmap();

    if (pixmap.isNull() || pixmap.width() <= 0 || pixmap.height() <= 0) {
        QMessageBox::warning(this, "⚠ Warning", "Image data is missing or corrupted for cropping.");
        return;
    }

    double scaleX = (double)currentImage.width / pixmap.width();
    double scaleY = (double)currentImage.height / pixmap.height();

    int x1 = (int)(rect.left() * scaleX);
    int y1 = (int)(rect.top() * scaleY);
    int x2 = (int)(rect.right() * scaleX);
    int y2 = (int)(rect.bottom() * scaleY);

    x1 = max(0, min(x1, currentImage.width));
    y1 = max(0, min(y1, currentImage.height));
    x2 = max(0, min(x2, currentImage.width));
    y2 = max(0, min(y2, currentImage.height));

    if (x2 > x1 && y2 > y1) {
        Crop(currentImage, x1, y1, x2, y2);
        pushHistory();
        displayImageCentered();
    } else {
        QMessageBox::warning(this, "⚠ Warning", "Invalid crop area selected.");
    }

    cropMode = false;
    imageLabel->setCropMode(false);
    imageLabel->clearCropRect();
}

void MainWindow::applyInfrared() {
    if (!hasImage) return;
    Infrared(currentImage);
    pushHistory();
    displayImageCentered();
}

void MainWindow::applyNightPurple() {
    if (!hasImage) return;
    NightPurple(currentImage);
    pushHistory();
    displayImageCentered();
}

void MainWindow::applyNeon() {
    if (!hasImage) return;
    QColor color = QColorDialog::getColor(Qt::cyan, this, "💥 Choose Neon Color");
    if(color.isValid()) {
        Neon(currentImage, color);
        pushHistory();
        displayImageCentered();
    }
}

void MainWindow::applySolarize() {
    if (!hasImage) return;
    Solarize(currentImage);
    pushHistory();
    displayImageCentered();
}

void MainWindow::clearFilters() {
    if (!hasImage || historyStates.empty()) return;
    currentImage = historyStates[0];
    historyIndex = 0;
    pushHistory();
    displayImageCentered();
    QMessageBox::information(this, "✓ Success", "All filters cleared!");
}

void MainWindow::deleteImage() {
    if (!hasImage) return;

    auto reply = QMessageBox::question(this, "⚠ Confirm",
                                       "Are you sure you want to delete the current image?",
                                       QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        hasImage = false;
        historyStates.clear();
        historyIndex = -1;
        imageLabel->clear();
        imageLabel->setFixedSize(0, 0);
        imageSizeLabel->setText("📸 No image loaded - Start your creative journey!");
        updateHistoryButtons();
        QMessageBox::information(this, "✓ Success", "Image deleted!");
    }
}

// --- Image Processing Functions (Final Logic) ---

void MainWindow::Grayscale(Image &img) {
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            int r = img(i, j, 0);
            int g = img(i, j, 1);
            int b = img(i, j, 2);
            unsigned char avg = (r * 0.299 + g * 0.587 + b * 0.114);
            img(i, j, 0) = img(i, j, 1) = img(i, j, 2) = avg;
        }
    }
}

void MainWindow::BlackAndWhite(Image &img) {
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            int r = img(i, j, 0);
            int g = img(i, j, 1);
            int b = img(i, j, 2);
            unsigned char gray = (r * 0.299 + g * 0.587 + b * 0.114);
            unsigned char val = (gray > 127) ? 255 : 0;
            img(i, j, 0) = img(i, j, 1) = img(i, j, 2) = val;
        }
    }
}

void MainWindow::Invert(Image &img) {
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            for (int k = 0; k < 3; ++k) {
                img(i, j, k) = 255 - img(i, j, k);
            }
        }
    }
}

void MainWindow::FlipHorizontal(Image &img) {
    for (int i = 0; i < img.width / 2; ++i) {
        for (int j = 0; j < img.height; ++j) {
            for (int k = 0; k < 3; ++k) {
                swap(img(i, j, k), img(img.width - 1 - i, j, k));
            }
        }
    }
}

void MainWindow::FlipVertical(Image &img) {
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height / 2; ++j) {
            for (int k = 0; k < 3; ++k) {
                swap(img(i, j, k), img(i, img.height - 1 - j, k));
            }
        }
    }
}

void MainWindow::Rotate(Image &img, int deg) {
    if (deg == 180) {
        for (int i = 0; i < img.width; ++i) {
            for (int j = 0; j < img.height / 2; ++j) {
                for (int k = 0; k < 3; ++k) {
                    swap(img(i, j, k), img(img.width - 1 - i, img.height - 1 - j, k));
                }
            }
        }
        if (img.height % 2 != 0) {
            int midY = img.height / 2;
            for (int i = 0; i < img.width / 2; ++i) {
                for (int k = 0; k < 3; ++k) {
                    swap(img(i, midY, k), img(img.width - 1 - i, midY, k));
                }
            }
        }
        return;
    }

    if (deg == 90 || deg == 270) {
        Image newImage(img.height, img.width);
        for (int i = 0; i < img.width; ++i) {
            for (int j = 0; j < img.height; ++j) {
                for (int k = 0; k < 3; ++k) {
                    if (deg == 90) {
                        newImage(img.height - 1 - j, i, k) = img(i, j, k);
                    } else if (deg == 270) {
                        newImage(j, img.width - 1 - i, k) = img(i, j, k);
                    }
                }
            }
        }
        img = newImage;
    }
}

void MainWindow::EdgeDetect(Image &img) {
    Image result(img.width, img.height);

    for (int y = 0; y < img.height - 1; ++y) {
        for (int x = 0; x < img.width - 1; ++x) {
            int p1 = (img(x, y, 0) + img(x, y, 1) + img(x, y, 2)) / 3;
            int p2 = (img(x + 1, y, 0) + img(x + 1, y, 1) + img(x + 1, y, 2)) / 3;
            int p3 = (img(x, y + 1, 0) + img(x, y + 1, 1) + img(x, y + 1, 2)) / 3;
            int p4 = (img(x + 1, y + 1, 0) + img(x + 1, y + 1, 1) + img(x + 1, y + 1, 2)) / 3;

            int magnitude = abs(p1 - p4) + abs(p2 - p3);
            magnitude = min(255, magnitude);

            result(x, y, 0) = result(x, y, 1) = result(x, y, 2) = magnitude;
        }
    }
    img = result;
    Invert(img);
}

void MainWindow::OilPaint(Image &img) {
    Image result = img;
    int radius = 2; 
    int levels = 20; 
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            int R_counts[256] = {0};
            int G_counts[256] = {0};
            int B_counts[256] = {0};
            int maxR_count = 0, maxG_count = 0, maxB_count = 0;
            unsigned char modeR = img(x, y, 0), modeG = img(x, y, 1), modeB = img(x, y, 2);
            int x_start = max(0, x - radius);
            int x_end = min(img.width - 1, x + radius);
            int y_start = max(0, y - radius);
            int y_end = min(img.height - 1, y + radius);
            for (int i = x_start; i <= x_end; ++i) {
                for (int j = y_start; j <= y_end; ++j) {
                    unsigned char r_val = img(i, j, 0);
                    unsigned char g_val = img(i, j, 1);
                    unsigned char b_val = img(i, j, 2);
                    R_counts[r_val]++;
                    if (R_counts[r_val] > maxR_count) {
                        maxR_count = R_counts[r_val];
                        modeR = r_val;
                    }
                    G_counts[g_val]++;
                    if (G_counts[g_val] > maxG_count) {
                        maxG_count = G_counts[g_val];
                        modeG = g_val;
                    }
                    B_counts[b_val]++;
                    if (B_counts[b_val] > maxB_count) {
                        maxB_count = B_counts[b_val];
                        modeB = b_val;
                    }
                }
            }
            result(x, y, 0) = modeR;
            result(x, y, 1) = modeG;
            result(x, y, 2) = modeB;
        }
    }
    for (int x = 0; x < img.width; ++x) {
        for (int y = 0; y < img.height; ++y) {
            for (int c = 0; c < 3; ++c) {
                result(x, y, c) = (result(x, y, c) / levels) * levels;
            }
        }
    }

    img = result;
}

void MainWindow::TVEffect(Image &img) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> noise_dist(-15, 15);

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {

            int noise = noise_dist(rng);
            for (int k = 0; k < 3; ++k) {
                int val = img(x, y, k) + noise;
                img(x, y, k) = min(255, max(0, val));
            }

            if ((y % 4) == 0) {
                for (int k = 0; k < 3; ++k) {
                    img(x, y, k) = (int)img(x, y, k) * 75 / 100;
                }
            }

            if ((y % 2) == 0) {
                for (int k = 0; k < 3; ++k) {
                    if (x < img.width - 1) {
                        img(x, y, k) = (img(x, y, k) + img(x + 1, y, k)) / 2;
                    }
                }
            }
        }
    }
}

void MainWindow::Brightness(Image &img, int percentage) {
    double scale = percentage / 100.0;

    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            for (int k = 0; k < 3; ++k) {
                int val = img(i, j, k);
                if (scale > 0) {
                    val = val + (int)((255 - val) * scale);
                } else if (scale < 0) {
                    val = val + (int)(val * scale);
                }
                img(i, j, k) = min(255, max(0, val));
            }
        }
    }
}

void MainWindow::Blur(Image &img, int radius) {
    if (radius <= 0) return;
    int kernelSize = 2 * radius + 1;
    Image temp = img;
    for (int y = 0; y < img.height; ++y) {
        int sumR = 0, sumG = 0, sumB = 0;
        for (int x = -radius; x <= radius; ++x) {
            int nx = max(0, min(x, img.width - 1));
            sumR += img(nx, y, 0);
            sumG += img(nx, y, 1);
            sumB += img(nx, y, 2);
        }
        for (int x = 0; x < img.width; ++x) {
            temp(x, y, 0) = sumR / kernelSize;
            temp(x, y, 1) = sumG / kernelSize;
            temp(x, y, 2) = sumB / kernelSize;
            int x_out = x - radius;
            int x_in = x + radius + 1;
            if (x_out < 0) {
                sumR -= img(0, y, 0);
                sumG -= img(0, y, 1);
                sumB -= img(0, y, 2);
            } else if (x_out < img.width) {
                sumR -= img(x_out, y, 0);
                sumG -= img(x_out, y, 1);
                sumB -= img(x_out, y, 2);
            }
            if (x_in >= img.width) {
                sumR += img(img.width - 1, y, 0);
                sumG += img(img.width - 1, y, 1);
                sumB += img(img.width - 1, y, 2);
            } else if (x_in >= 0) {
                sumR += img(x_in, y, 0);
                sumG += img(x_in, y, 1);
                sumB += img(x_in, y, 2);
            }
        }
    }
    for (int x = 0; x < img.width; ++x) {
        int sumR = 0, sumG = 0, sumB = 0;
        for (int y = -radius; y <= radius; ++y) {
            int ny = max(0, min(y, img.height - 1));
            sumR += temp(x, ny, 0);
            sumG += temp(x, ny, 1);
            sumB += temp(x, ny, 2);
        }
        for (int y = 0; y < img.height; ++y) {
            img(x, y, 0) = sumR / kernelSize;
            img(x, y, 1) = sumG / kernelSize;
            img(x, y, 2) = sumB / kernelSize;
            int y_out = y - radius;
            int y_in = y + radius + 1;
            if (y_out < 0) {
                sumR -= temp(x, 0, 0);
                sumG -= temp(x, 0, 1);
                sumB -= temp(x, 0, 2);
            } else if (y_out < img.height) {
                sumR -= temp(x, y_out, 0);
                sumG -= temp(x, y_out, 1);
                sumB -= temp(x, y_out, 2);
            }
            if (y_in >= img.height) {
                sumR += temp(x, img.height - 1, 0);
                sumG += temp(x, img.height - 1, 1);
                sumB += temp(x, img.height - 1, 2);
            } else if (y_in >= 0) {
                sumR += temp(x, y_in, 0);
                sumG += temp(x, y_in, 1);
                sumB += temp(x, y_in, 2);
            }
        }
    }
}

void MainWindow::Skew(Image &img, double angle) {
    double radians = angle * M_PI / 180.0;
    double tanSkew = tan(radians);
    int offsetMax = (int)abs(img.height * tanSkew);
    int newWidth = img.width + offsetMax;

    Image result(newWidth, img.height);
    memset(result.imageData, 0, newWidth * img.height * 3);

    for (int y = 0; y < img.height; ++y) {
        int offset = (int)(y * tanSkew);
        int finalX_start = (angle > 0) ? offset : offsetMax + offset;

        for (int x = 0; x < img.width; ++x) {
            int newX = x + finalX_start;

            if (newX >= 0 && newX < newWidth) {
                for (int k = 0; k < 3; ++k) {
                    result(newX, y, k) = img(x, y, k);
                }
            }
        }
    }

    img = result;
}


void MainWindow::Frame(Image &img, int thickness, QColor color) {
    int newWidth = img.width + 2 * thickness;
    int newHeight = img.height + 2 * thickness;
    Image result(newWidth, newHeight);

    unsigned char r = color.red();
    unsigned char g = color.green();
    unsigned char b = color.blue();

    for (int i = 0; i < newWidth; ++i) {
        for (int j = 0; j < newHeight; ++j) {
            result(i, j, 0) = r;
            result(i, j, 1) = g;
            result(i, j, 2) = b;
        }
    }

    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            for (int k = 0; k < 3; ++k) {
                result(i + thickness, j + thickness, k) = img(i, j, k);
            }
        }
    }
    img = result;
}

void MainWindow::FrameDouble(Image &img, int innerThickness, int outerThickness, QColor innerColor, QColor outerColor) {

    Frame(img, innerThickness, innerColor);

    Frame(img, outerThickness, outerColor);
}

void MainWindow::FrameFancy(Image &img) {

    int thickness = 25;


    int newWidth = img.width + 2 * thickness;
    int newHeight = img.height + 2 * thickness;
    Image result(newWidth, newHeight);


    for (int i = 0; i < newWidth; ++i) {
        for (int j = 0; j < newHeight; ++j) {
            bool inFrame = (i < thickness || i >= newWidth - thickness ||
                            j < thickness || j >= newHeight - thickness);
            if (inFrame) {
                int distFromEdge = min(min(i, newWidth - 1 - i),
                                       min(j, newHeight - 1 - j));
                double gradientFactor = (double)distFromEdge / thickness;
                double wave = sin(i * 0.3) * sin(j * 0.3) * 0.15 + 0.85;
                unsigned char r, g, b;
                if (gradientFactor < 0.3) {
                    double t = gradientFactor / 0.3;
                    r = (unsigned char)(139 + (218 - 139) * t * wave);
                    g = (unsigned char)(90 + (165 - 90) * t * wave);
                    b = (unsigned char)(0 + (32 - 0) * t * wave);
                } else if (gradientFactor < 0.7) {
                    double t = (gradientFactor - 0.3) / 0.4;
                    r = (unsigned char)(218 + (255 - 218) * sin(t * M_PI) * wave);
                    g = (unsigned char)(165 + (223 - 165) * sin(t * M_PI) * wave);
                    b = (unsigned char)(32 + (120 - 32) * sin(t * M_PI) * wave);
                } else {
                    double t = (gradientFactor - 0.7) / 0.3;
                    r = (unsigned char)(255 - (255 - 205) * t * wave);
                    g = (unsigned char)(223 - (223 - 173) * t * wave);
                    b = (unsigned char)(120 - (120 - 60) * t * wave);
                }
                if (distFromEdge == thickness / 3 || distFromEdge == 2 * thickness / 3) {
                    r = min(255, (int)(r * 1.2));
                    g = min(255, (int)(g * 1.2));
                    b = min(255, (int)(b * 1.2));
                }
                int cornerDist = min(min(i, newWidth - 1 - i),
                                     min(j, newHeight - 1 - j));
                if (cornerDist < thickness / 2) {
                    double cornerEffect = 1.0 + 0.3 * sin(sqrt(i * i + j * j) * 0.5);
                    r = min(255, (int)(r * cornerEffect));
                    g = min(255, (int)(g * cornerEffect));
                    b = min(255, (int)(b * cornerEffect));
                }
                result(i, j, 0) = r;
                result(i, j, 1) = g;
                result(i, j, 2) = b;
            } else {
                result(i, j, 0) = img(i - thickness, j - thickness, 0);
                result(i, j, 1) = img(i - thickness, j - thickness, 1);
                result(i, j, 2) = img(i - thickness, j - thickness, 2);
            }
        }
    }
    img = result;
}

void MainWindow::Resize(Image &img, int newW, int newH) {
    Image result(newW, newH);
    double xRatio = (double)img.width / newW;
    double yRatio = (double)img.height / newH;
    for (int y = 0; y < newH; ++y) {
        for (int x = 0; x < newW; ++x) {
            int srcX = (int)(x * xRatio);
            int srcY = (int)(y * yRatio);
            srcX = min(srcX, img.width - 1);
            srcY = min(srcY, img.height - 1);
            for (int k = 0; k < 3; ++k) {
                result(x, y, k) = img(srcX, srcY, k);
            }
        }
    }
    img = result;
}

void MainWindow::Crop(Image &img, int x1, int y1, int x2, int y2) {
    int newWidth = x2 - x1;
    int newHeight = y2 - y1;
    Image result(newWidth, newHeight);
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            for (int k = 0; k < 3; ++k) {
                result(x, y, k) = img(x1 + x, y1 + y, k);
            }
        }
    }
    img = result;
}

Image MainWindow::MergeImages(Image a, Image b) {
    int maxW = max(a.width, b.width);
    int maxH = max(a.height, b.height);
    if (a.width != maxW || a.height != maxH) Resize(a, maxW, maxH);
    if (b.width != maxW || b.height != maxH) Resize(b, maxW, maxH);
    Image result(maxW, maxH);
    for (int y = 0; y < maxH; ++y) {
        for (int x = 0; x < maxW; ++x) {
            for (int k = 0; k < 3; ++k) {
                result(x, y, k) = (a(x, y, k) + b(x, y, k)) / 2;
            }
        }
    }
    return result;
}

void MainWindow::Infrared(Image &img) {
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            uint8_t r = img(i, j, 0);
            uint8_t g = img(i, j, 1);
            uint8_t b = img(i, j, 2);
            double gray_d = r * 0.3 + g * 0.59 + b * 0.11;
            double newR_d = std::min(255.0, gray_d * 2.0);
            double newG_d = std::min(255.0, gray_d * 0.30);
            double newB_d = std::min(255.0, gray_d * 0.30);
            auto adjust_channel = [](double v) -> int {
                double norm = v / 255.0;
                norm = pow(norm, 0.7);
                int out = static_cast<int>(norm * 255.0 + 0.5);
                return max(0, std::min(255, out));
            };
            int newR = adjust_channel(newR_d);
            int newG = adjust_channel(newG_d);
            int newB = adjust_channel(newB_d);
            img(i, j, 0) = static_cast<uint8_t>(newR);
            img(i, j, 1) = static_cast<uint8_t>(newG);
            img(i, j, 2) = static_cast<uint8_t>(newB);
        }
    }
}

void MainWindow::NightPurple(Image &img) {
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            unsigned char gray = (img(i, j, 0) + img(i, j, 1) + img(i, j, 2)) / 3;
            img(i, j, 0) = min(255, (int)(gray * 0.4));
            img(i, j, 1) = min(255, (int)(gray * 0.2));
            img(i, j, 2) = min(255, (int)(gray * 0.9));
        }
    }
}

void MainWindow::Neon(Image &img, QColor color) {
    Image edgeImage = img;
    EdgeDetect(edgeImage);
    unsigned char r_neon = color.red();
    unsigned char g_neon = color.green();
    unsigned char b_neon = color.blue();
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            unsigned char e = edgeImage(i, j, 0);
            unsigned char intensity = 255 - e;
            double factor = intensity / 255.0;
            img(i, j, 0) = static_cast<unsigned char>(r_neon * factor);
            img(i, j, 1) = static_cast<unsigned char>(g_neon * factor);
            img(i, j, 2) = static_cast<unsigned char>(b_neon * factor);
        }
    }
}

void MainWindow::Solarize(Image &img) {
    int threshold = 128;
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            for (int k = 0; k < 3; ++k) {
                unsigned char val = img(i, j, k);
                if (val < threshold) {
                    img(i, j, k) = 255 - val;
                }
            }
        }
    }
}
