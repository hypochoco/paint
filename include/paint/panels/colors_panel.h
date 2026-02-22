//
//  colors_panel.h
//  main
//
//  Created by Daniel Cho on 2/13/26.
//

#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

#include <algorithm>

class HueIndicator : public QWidget {
    Q_OBJECT
    
public:
    explicit HueIndicator(QWidget* parent = nullptr)
    : QWidget(parent), hue(0), saturation(0), value(255) {
        setFixedWidth(25);
        setFixedHeight(25);
    }
    
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);
        update();
    }
        
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        // todo: max image size
        QImage indicatorCache = QImage(size(), QImage::Format_RGB32);
        QPainter p(&indicatorCache);
        p.fillRect(rect(), QColor::fromHsv(hue, saturation, value));
        painter.drawImage(0, 0, indicatorCache);
    }
    
public slots:
    void onHueChange(int hue) {
        this->hue = hue;
        update();
    }
    void onSVChange(int saturation, int value) {
        this->saturation = saturation;
        this->value = value;
        update();
    }

private:
    int hue, saturation, value;
    
};

class SVSquare : public QWidget {
    Q_OBJECT
    
public:
    explicit SVSquare(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumWidth(50);
        setMinimumHeight(50);
    }
    
public slots:
    void onHueChanged(int hue) {
        this->hue = hue;
        renderGradientCache();
        update();
    }

signals:
    void colorChanged(int s, int v);

protected:
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);

        renderGradientCache();
        update();
    }
    
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        
//        painter.drawImage(event->rect(), gradientCache, event->rect());
        painter.drawImage(0, 0, gradientCache); // temp
        
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(lastPosition.y() > (height() / 2.f) ? Qt::white : Qt::black);
        painter.drawEllipse(lastPosition, 5, 5);
    }

    void mousePressEvent(QMouseEvent *event) override { handleMouse(event); }
    void mouseMoveEvent(QMouseEvent *event) override { handleMouse(event); }

private:
    int hue;
    QPointF lastPosition;
    QImage gradientCache;
    
    void handleMouse(QMouseEvent *event) {
        if (event->position() == lastPosition) return;
            
//        QRect oldRect(lastPosition.x() - 3, lastPosition.y() - 3, // bottom left
//                      6, 6); // size
//
        lastPosition = QPointF(qBound(0, (int) event->position().rx(), width()),
                               qBound(0, (int) event->position().ry(), height()));

//        lastPosition = event->position();
//        QRect newRect(lastPosition.x() - 3, lastPosition.y() - 3,
//                      6, 6);
//
//        update(oldRect);
//        update(newRect);
        update();

        emit colorChanged(qBound(0, ((int) lastPosition.x() * 255) / width(), 255),
                          qBound(0, 255 - ((int) (lastPosition.y() * 255) / height()), 255));
    }
    
    void renderGradientCache() {
        // todo: fix image size
        gradientCache = QImage(size(), QImage::Format_RGB32);
        QPainter p(&gradientCache);
        
        p.fillRect(rect(), QColor::fromHsv(hue, 255, 255));

        QLinearGradient sGrad(rect().left(), 0, rect().right(), 0);
        sGrad.setColorAt(0, Qt::white);
        sGrad.setColorAt(1, Qt::transparent);
        p.fillRect(rect(), sGrad);

        QLinearGradient vGrad(0, rect().top(), 0, rect().bottom());
        vGrad.setColorAt(0, Qt::transparent);
        vGrad.setColorAt(1, Qt::black);
        p.fillRect(rect(), vGrad);
    }

};

class HueSlider : public QWidget {
    Q_OBJECT
    
public:
    explicit HueSlider(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedWidth(25);
        setMinimumHeight(50);
    }
    
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);
        
        spectrumCache = QPixmap(size()); // todo: max image size
        QPainter p(&spectrumCache);
        
        QLinearGradient grad(0, 0, 0, height());
        grad.setColorAt(0.0,  Qt::red);
        grad.setColorAt(0.16, Qt::yellow);
        grad.setColorAt(0.33, Qt::green);
        grad.setColorAt(0.5,  Qt::cyan);
        grad.setColorAt(0.66, Qt::blue);
        grad.setColorAt(0.83, Qt::magenta);
        grad.setColorAt(1.0,  Qt::red);
        
        p.fillRect(rect(), grad);
    }
        
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        
        painter.drawPixmap(event->rect(), spectrumCache, event->rect());
        painter.setPen(QPen(Qt::white, 1));
        painter.drawLine(0, lastPosition, width(), lastPosition);
    }
    
    void handleMouseEvent(QMouseEvent* event) {
        int newPosition = qBound(0, (int) event->position().y(), height() - 1);
        int newHue = (newPosition * 359) / height(); // hue map calculation

        if (newHue != hue) {
            QRect oldRect(0, lastPosition - 2, width(), 5);
            
            hue = newHue;
            lastPosition = newPosition;
            
            QRect newRect(0, lastPosition - 2, width(), 5);
            
            update(oldRect);
            update(newRect);
            
            emit hueChanged(hue);
        }
    }
        
    void mousePressEvent(QMouseEvent* event) override {
        handleMouseEvent(event);
    }
    
    void mouseMoveEvent(QMouseEvent *event) override {
        handleMouseEvent(event);
    }
    
signals:
    void hueChanged(int hue);
    
private:
    int hue, lastPosition;
    QPixmap spectrumCache;
    
};

class ColorsPanel : public QWidget {
    Q_OBJECT
    
public:
    explicit ColorsPanel(QWidget* parent = nullptr) : QWidget(parent) {
        setMinimumWidth(100);
        setMinimumHeight(100);
        
        createColorPicker();
    }
    
    void createColorPicker() {
                
        //    QWidget* colorWidget = new QWidget(this);
        //    QVBoxLayout* colorLayout = new QVBoxLayout(colorWidget);
        //    QPushButton* colorButton = new QPushButton(colorWidget);
        //    colorButton->setFixedSize(30, 30);
        //    colorButton->setStyleSheet(
        //        "QPushButton {"
        //        "background-color: #3498db;"
        //        "border-radius: 15px;"
        //        "}"
        //    );
        //    colorLayout->addWidget(colorButton, 0, Qt::AlignTop | Qt::AlignLeft);
        //    colorWidget->setLayout(colorLayout);
        
        HueIndicator* hueIndicator = new HueIndicator(this);
        SVSquare* svSquare = new SVSquare(this);
        HueSlider* hueSlider = new HueSlider(this);
        
        connect(hueSlider, &HueSlider::hueChanged,
                svSquare, &SVSquare::onHueChanged);
        connect(hueSlider, &HueSlider::hueChanged,
                hueIndicator, &HueIndicator::onHueChange);
        connect(svSquare, &SVSquare::colorChanged,
                hueIndicator, &HueIndicator::onSVChange);
        
        QHBoxLayout* colorLayout = new QHBoxLayout();
        // todo: fixed spacing between components
        colorLayout->addWidget(hueIndicator);
        colorLayout->addWidget(svSquare);
        colorLayout->addWidget(hueSlider);
        this->setLayout(colorLayout);
        
    }

};
