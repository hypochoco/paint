//
//  brush_panel.h
//  main
//
//  Created by Daniel Cho on 2/8/26.
//

#pragma once

#include <QObject>
#include <QLabel>

class BrushesPanel : public QWidget {
    Q_OBJECT
    
public:
    explicit BrushesPanel(QWidget* parent = nullptr) : QWidget(parent) {
        setMinimumWidth(360);
        setMinimumHeight(360);
        
        createWidgets();
    }
    
    void createWidgets() {
        
        // brush size
        
        QLabel *brushSizeLabel = new QLabel("Brush size", this);
        
        QSlider* brushSizeSlider = new QSlider(Qt::Horizontal, this);
        brushSizeSlider->setRange(1, 100);
        brushSizeSlider->setValue(25);
        
        connect(brushSizeSlider, &QSlider::valueChanged,
                this, &BrushesPanel::onBrushSizeSliderValueChanged);
        
        QLabel *brushSizeValueLabel = new QLabel("0.25", this);
        
        connect(brushSizeSlider, &QSlider::valueChanged,
                this, [brushSizeValueLabel](int value) {
            brushSizeValueLabel->setText(QString::number(value / 100.0, 'f', 2));
        });

        QHBoxLayout *brushSizeLayout = new QHBoxLayout();
        brushSizeLayout->addWidget(brushSizeSlider);
        brushSizeLayout->addWidget(brushSizeValueLabel);
        
        // brush spacing
        
        QLabel *brushSpacingLabel = new QLabel("Brush spacing", this);
        
        QSlider* brushSpacingSlider = new QSlider(Qt::Horizontal, this);
        brushSpacingSlider->setRange(1, 100);
        brushSpacingSlider->setValue(5);
        
        connect(brushSpacingSlider, &QSlider::valueChanged,
                this, &BrushesPanel::onBrushSpacingSliderValueChanged);
        
        QLabel *brushSpacingValueLabel = new QLabel("0.05", this);
        
        connect(brushSpacingSlider, &QSlider::valueChanged,
                this, [brushSpacingValueLabel](int value) {
            brushSpacingValueLabel->setText(QString::number(value / 100.0, 'f', 2));
        });
        
        QHBoxLayout *brushSpacingLayout = new QHBoxLayout();
        brushSpacingLayout->addWidget(brushSpacingSlider);
        brushSpacingLayout->addWidget(brushSpacingValueLabel);
        
        // brush opacity
        
        QLabel *brushOpacityLabel = new QLabel("Brush opacity", this);
        
        QSlider* brushOpacitySlider = new QSlider(Qt::Horizontal, this);
        brushOpacitySlider->setRange(0, 100);
        brushOpacitySlider->setValue(80);
        
        connect(brushOpacitySlider, &QSlider::valueChanged,
                this, &BrushesPanel::onBrushOpacitySliderValueChanged);
        
        QLabel *brushOpacityValueLabel = new QLabel("0.80", this);
        
        connect(brushOpacitySlider, &QSlider::valueChanged,
                this, [brushOpacityValueLabel](int value) {
            brushOpacityValueLabel->setText(QString::number(value / 100.0, 'f', 2));
        });
        
        QHBoxLayout *brushOpacityLayout = new QHBoxLayout();
        brushOpacityLayout->addWidget(brushOpacitySlider);
        brushOpacityLayout->addWidget(brushOpacityValueLabel);
        
        // layout
        
        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(brushSizeLabel);
        layout->addLayout(brushSizeLayout);
        layout->addWidget(brushSpacingLabel);
        layout->addLayout(brushSpacingLayout);
        layout->addWidget(brushOpacityLabel);
        layout->addLayout(brushOpacityLayout);
        this->setLayout(layout);
        
    }
    
public slots:
    void onBrushSize(std::function<void(float)> reply) {
        reply(brushSizeValue);
    }
    void onBrushSpacing(std::function<void(float)> reply) {
        reply(brushSpacingValue);
    }
    void onBrushOpacity(std::function<void(float)> reply) {
        reply(brushOpacityValue);
    }

private slots:
    void onBrushSizeSliderValueChanged(int value) {
        brushSizeValue = value / 100.f;
        qDebug() << "[brushes panel] brush size slider value changed to:" << brushSizeValue;
    }
    void onBrushSpacingSliderValueChanged(int value) {
        brushSpacingValue = value / 100.f;
        qDebug() << "[brushes panel] brush spacing slider value changed to:" << brushSpacingValue;
    }
    void onBrushOpacitySliderValueChanged(int value) {
        brushOpacityValue = value / 100.f;
        qDebug() << "[brushes panel] brush opacity slider value changed to:" << brushOpacityValue;
    }
    
private:
    float brushSizeValue = 0.25f;
    float brushSpacingValue = 0.05f;
    float brushOpacityValue = 0.8f;
    
};
