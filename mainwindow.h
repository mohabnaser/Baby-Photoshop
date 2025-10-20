#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStackedWidget>
#include <QInputDialog>
#include <QImage>
#include <QPixmap>
#include <QColorDialog>
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <QButtonGroup>
#include <QPainter>
#include <QStyleFactory>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>
#include "Image_Class.h"
#include <random>

class ImageLabel : public QLabel {
    Q_OBJECT
public:
    ImageLabel(QWidget *parent = nullptr);
    void setCropMode(bool enabled);
    QRect getCropRect() const { return cropRect; }
    void clearCropRect();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void cropAreaSelected(QRect rect);

private:
    bool cropMode;
    QPoint origin;
    QRect cropRect;
    bool selecting;
};

class FilterButton : public QPushButton {
    Q_OBJECT
public:
    FilterButton(const QString &text, QWidget *parent = nullptr);
};

class FrameDialog : public QDialog {
    Q_OBJECT
public:
    FrameDialog(QWidget *parent = nullptr);
    int getFrameType() const;
    QColor getColor1() const;
    QColor getColor2() const;
    int getThickness1() const;
    int getThickness2() const;

private:
    QComboBox *typeCombo;
    QPushButton *color1Btn;
    QPushButton *color2Btn;
    QSpinBox *thickness1Spin;
    QSpinBox *thickness2Spin;
    QColor color1;
    QColor color2;
    void updateColorButton(QPushButton* button, const QColor& color);
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadImage();
    void saveImage();
    void applyGrayscale();
    void applyBlackAndWhite();
    void applyInvert();
    void applyFlip();
    void applyRotate90CW();
    void applyRotate90CCW();
    void applyEdgeDetection();
    void applyOilPaint();
    void applyTVEffect();
    void applyMerge();
    void applyFrame();
    void applyResize();
    void startCropSelection();
    void finishCrop(QRect rect);
    void showBrightnessControl();
    void showBlurControl();
    void showSkewControl();
    void applyBrightness();
    void applyBlur();
    void applySkew();
    void applyInfrared();
    void applyNightPurple();
    void applyNeon();
    void applySolarize();
    void clearFilters();
    void deleteImage();
    void undo();
    void redo();

private:
    void setupUI();
    void setupControlPanel();
    void displayImageCentered();
    void updateHistoryButtons();
    void pushHistory();
    void showControlPanel(int index);
    void hideControlPanel();

    void applyBrightnessValue(int value);
    void applyBlurValue(int value);
    void applySkewValue(double value);

    // UI Members (Defined as members to avoid scope errors)
    ImageLabel *imageLabel;
    QLabel *imageSizeLabel;
    QScrollArea *scrollArea;
    QWidget *leftPanel;

    QWidget *controlWrapper;
    QWidget *controlPanel;
    QStackedWidget *controlStack;
    QSlider *brightnessSlider;
    QSlider *blurSlider;
    QDoubleSpinBox *skewSpinBox;
    QLabel *brightnessValueLabel;
    QLabel *blurValueLabel;
    QPushButton *applyBtn;
    QPushButton *undoBtn;
    QPushButton *redoBtn;

    // Image Data and History
    Image currentImage;
    Image previewImage;
    QString currentFileName;
    bool hasImage;
    bool cropMode;

    static const int MAX_HISTORY = 10;
    std::vector<Image> historyStates;
    int historyIndex;

    // --- Core Image Processing Functions ---
    void Grayscale(Image &img);
    void BlackAndWhite(Image &img);
    void Invert(Image &img);
    void FlipHorizontal(Image &img);
    void FlipVertical(Image &img);
    void Rotate(Image &img, int deg);
    void EdgeDetect(Image &img);
    void OilPaint(Image &img);
    void TVEffect(Image &img);
    void Brightness(Image &img, int percentage);
    void Blur(Image &img, int radius);
    void Skew(Image &img, double angle);
    void Frame(Image &img, int thickness, QColor color);
    void FrameDouble(Image &img, int t1, int t2, QColor c1, QColor c2);
    void FrameFancy(Image &img);
    void Resize(Image &img, int newW, int newH);
    void Crop(Image &img, int x1, int y1, int x2, int y2);
    Image MergeImages(Image a, Image b);
    void Infrared(Image &img);
    void NightPurple(Image &img);
    void Neon(Image &img, QColor color);
    void Solarize(Image &img);
};

#endif 
