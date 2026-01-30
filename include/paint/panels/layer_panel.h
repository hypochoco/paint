//
//  layers_panel.h
//  main
//
//  Created by Daniel Cho on 1/28/26.
//

#pragma once

#include <QObject>
#include <QListWidget>
#include <QToolButton>
#include <QCheckBox>
#include <QLineEdit>

#include <QHBoxLayout>
#include <QVBoxLayout>

class LayersPanel : public QWidget {
    Q_OBJECT
    
public:
    explicit LayersPanel(QWidget* parent = nullptr) : QWidget(parent) {
        createUI();
        connectSignals();
    }
    
signals:
    void layerAdded(int index);
    void layerRemoved(int index);
    void layerSelected(int index);

private slots:
    void addLayer() {
        int count = m_layerList->count();
        addLayerItem(QString("Layer %1").arg(count + 1));

         emit layerAdded(count);
    }

    void removeSelectedLayer() {
        int row = m_layerList->currentRow();
        if (row < 0)
            return;

        delete m_layerList->takeItem(row);
         emit layerRemoved(row);
    }
    
    void onSelectionChanged() {
        int row = m_layerList->currentRow();
        if (row >= 0)
             emit layerSelected(row);
    }
    
private:
    QListWidget* m_layerList;
    QToolButton* m_addButton;
    QToolButton* m_removeButton;
    
    void createUI() {
        m_layerList = new QListWidget(this);
        m_layerList->setSelectionMode(QAbstractItemView::SingleSelection);
        m_layerList->setUniformItemSizes(true);
        
        m_addButton = new QToolButton(this);
        m_addButton->setText("+");

        m_removeButton = new QToolButton(this);
        m_removeButton->setText("-");

        auto* buttonsLayout = new QHBoxLayout;
        buttonsLayout->addStretch();
        buttonsLayout->addWidget(m_addButton);
        buttonsLayout->addWidget(m_removeButton);

        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(m_layerList);
        mainLayout->addLayout(buttonsLayout);

        setLayout(mainLayout);

    }
    
    static QWidget* createLayerWidget(const QString& name) {
        auto* widget = new QWidget;
        auto* layout = new QHBoxLayout(widget);
        layout->setContentsMargins(4, 2, 4, 2);

        auto* visibility = new QCheckBox;
        visibility->setChecked(true);

        auto* label = new QLineEdit(name);

        layout->addWidget(visibility);
        layout->addWidget(label);
        layout->addStretch();

        return widget;
    }
    
    void addLayerItem(const QString& name) {
        auto* item = new QListWidgetItem;
        item->setSizeHint(QSize(0, 24)); // fixed height like Photoshop

        m_layerList->addItem(item);
        m_layerList->setItemWidget(item, createLayerWidget(name));
        m_layerList->setCurrentItem(item);
    }
    
    void connectSignals() {
        connect(m_addButton, &QToolButton::clicked,
                this, &LayersPanel::addLayer);

        connect(m_removeButton, &QToolButton::clicked,
                this, &LayersPanel::removeSelectedLayer);

        connect(m_layerList, &QListWidget::currentRowChanged,
                this, &LayersPanel::onSelectionChanged);
    }
    
};
