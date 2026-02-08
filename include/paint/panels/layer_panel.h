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
    
    void sync() {
        m_layerList->clear();
        emit signalSync([this](std::string name) {
            addLayerItem(QString::fromStdString(name));
        });
    }
    
public slots:
    void onSelectedLayer(std::function<void(int)> reply) {
        reply(m_layerList->currentRow());
    }
    
private slots:
    
    // todo: layer grouping
    
    void addLayer() {
        int row = m_layerList->currentRow() + 1;
        emit signalAddLayer(row, [this, &row](std::string name) {
            insertLayerItem(row, QString::fromStdString(name));
        });
    }

    void removeSelectedLayer() {
        int row = m_layerList->currentRow();
        if (row < 0) return;
        delete m_layerList->takeItem(row);
        m_layerList->setCurrentRow(row - 1);
        emit signalRemoveLayer(row);
    }
    
    void onSelectionChanged() {
        int row = m_layerList->currentRow();
        qDebug() << "[layer panel] selection changed " << row;
    }
    
    void onRowsMoved(const QModelIndex&,
                     int start,
                     int end,
                     const QModelIndex&,
                     int destinationRow) {
        qDebug() << "[layer panel] rows moved";
        if (start != end) throw std::runtime_error("[layer panel] multi move not implemented");
        if (destinationRow > start)
            destinationRow -= 1;
        emit signalRowsMoved(start, destinationRow);
    }
    
signals:
    void dirty();
    void signalSync(std::function<void(std::string)> reply);
    void signalAddLayer(int index, std::function<void(std::string)> reply);
    void signalRemoveLayer(int index);
    void signalRowsMoved(int from, int to);
        
private:
    QListWidget* m_layerList;
    QToolButton* m_addButton;
    QToolButton* m_removeButton;
    
    void createUI() {
        m_layerList = new QListWidget(this);
        m_layerList->setSelectionMode(QAbstractItemView::SingleSelection);
        m_layerList->setUniformItemSizes(true);
        
        m_layerList->setDragEnabled(true);
        m_layerList->setAcceptDrops(true);
        m_layerList->setDropIndicatorShown(true);
        m_layerList->setDragDropMode(QAbstractItemView::InternalMove);
        
        // todo: bottom up ui
        
        connect(m_layerList->model(), &QAbstractItemModel::rowsMoved,
                this, &LayersPanel::onRowsMoved);
        
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
    
    void connectSignals() { // internal connections
        connect(m_addButton, &QToolButton::clicked,
                this, &LayersPanel::addLayer);

        connect(m_removeButton, &QToolButton::clicked,
                this, &LayersPanel::removeSelectedLayer);

        connect(m_layerList, &QListWidget::currentRowChanged,
                this, &LayersPanel::onSelectionChanged);
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
        item->setSizeHint(QSize(0, 24));
        
        m_layerList->addItem(item);
        m_layerList->setItemWidget(item, createLayerWidget(name)); // automatically parented
        m_layerList->setCurrentItem(item);
    }
    
    void insertLayerItem(int index, const QString& name) {
        auto* item = new QListWidgetItem;
        item->setSizeHint(QSize(0, 24));
        
        m_layerList->insertItem(index, item);
        m_layerList->setItemWidget(item, createLayerWidget(name)); // automatically parented
        m_layerList->setCurrentItem(item);
    }

};
