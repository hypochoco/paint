//
//  layers_panel.h
//  main
//
//  Created by Daniel Cho on 1/28/26.
//

#pragma once

#include <QUuid>
#include <QObject>
#include <QListWidget>
#include <QToolButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QTreeView>
#include <QAbstractItemModel>
#include <QMimeData>
#include <QStyledItemDelegate>
#include <QUndoStack>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QSortFilterProxyModel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <limits>

// todo: connect everything externally
    // dirty flags? vector representation? etc.
// todo: undo wrappers

// ---

// todo: open any number of canvas's, drawing a blank canvas
// todo: saving an image (pdf + jpg)

// todo: that should be the MVP, then it's adding features, cleaning up (keep under 200mb idle), etc.

enum class LayerType {
    Raster,
    Vector,
    Group,
    Adjustment
};

enum class BlendMode {
    Normal,
    Multiply,
    Screen,
    Overlay,
};

inline QString toString(BlendMode blendMode) {
    switch(blendMode) {
        case BlendMode::Normal:
            return "Normal";
        case BlendMode::Multiply:
            return "Multiply";
        default:
            return "Normal";
    }
}

inline BlendMode toBlendMode(QString blendMode) {
    static QHash<QString, BlendMode> stringToBlendModeMap {
        {"Normal", BlendMode::Normal},
        {"Multiply", BlendMode::Multiply},
    };
    if (stringToBlendModeMap.contains(blendMode)) {
        return stringToBlendModeMap[blendMode];
    } else {
        return BlendMode::Normal;
    }
}

struct LayerDataNode {

    QString name;
    QUuid id = QUuid::createUuid();
    LayerType type = LayerType::Raster;
    
    LayerDataNode* parent = nullptr;
    QList<LayerDataNode*> children;

    float opacity = 1.0f;
    BlendMode blendMode = BlendMode::Normal;
    bool visible = true;
//    bool            locked      = false;
//    QImage          thumbnail;
    
    ~LayerDataNode() {
        qDeleteAll(children);
    }
    
    bool isGroup() const { return type == LayerType::Group; }
    bool isRoot() const { return parent == nullptr; }
    
    int row() const {
        // index within parent->children
        if (!parent) return 0;
        for (int i = 0; i < parent->childCount(); i++) {
            if (parent->children[i] == this) {
                return i;
            }
        }
        return -1;
    }
    
    LayerDataNode* child(int row) const {
        if (row < 0 || row > childCount()) return nullptr;
        return children[row];
    }
    
    size_t childCount() const {
        return children.size();
    }
    
};

class LayerModel : public QAbstractItemModel {
    Q_OBJECT
    
public:
    explicit LayerModel(QObject* parent = nullptr) : QAbstractItemModel(parent) {
        root = new LayerDataNode;
        root->name = "root";
        root->type = LayerType::Group;
    }
    
    ~LayerModel() override {
        delete root; // recursively deletes entire tree
    }
    
    enum Roles {
        OpacityRole = Qt::UserRole + 1,
        VisibleRole,
        ParentVisibleRole,
//        LockedRole,
        BlendModeRole,
//        ThumbnailRole,
        LayerTypeRole,
//        NodePtrRole,
    };
    
    QModelIndex addLayer(const QModelIndex& parentIdx, int row, LayerType type) {
                        
        LayerDataNode* parentNode = nodeFromIndex(parentIdx);
    
        auto newNode = new LayerDataNode;
        newNode->type = type;
        newNode->parent = parentNode;
        
        switch (type) {
            case LayerType::Raster:
                newNode->name = "Layer " + QString::number(layerCount++);
                break;
                
            case LayerType::Group:
                newNode->name = "Group " + QString::number(groupCount++);
                break;
                
            default:
                return;
        }
        
        beginInsertRows(parentIdx, row, row);
        parentNode->children.insert(row, newNode);
        endInsertRows();

        return index(row, 0, parentIdx);
        
    }
    
    void removeLayer(const QModelIndex& parentIdx, const QModelIndex& selected) {
        
        // note: group deletion not supported for now
        
        int row = selected.row();
        
        LayerDataNode* parentNode = nodeFromIndex(parentIdx);
        LayerDataNode* node = parentNode->child(row);
                
        switch (node->type) {
            case LayerType::Raster:
                beginRemoveRows(parentIdx, row, row);
                parentNode->children.removeAt(row);
                delete node;
                endRemoveRows();
                break;
                
            case LayerType::Group: {
                
                int childCount = (int) node->childCount();
                                
                if (childCount > 0) {
                    beginInsertRows(parentIdx, row, row + childCount);
                    
                    for (auto* child : node->children) {
                        child->parent = parentNode;
                        parentNode->children.insert(row++, child);
                    }
                    node->children.clear();
                    delete node;

                    endInsertRows();
                }
                
                beginRemoveRows(parentIdx, row, row);
                parentNode->children.removeAt(row);
                endRemoveRows();
                                
                break;

            }
                
            default:
                qDebug() << "[layer model] layer type not supported for removal";
                break;
        }
        
    }
    
    LayerDataNode* nodeFromIndex(const QModelIndex& index) const {
        if (!index.isValid()) return root;
        return static_cast<LayerDataNode*>(index.internalPointer());
    }
    
    QModelIndex indexFromNode(LayerDataNode* node) const {
        if (!node || node == root) return QModelIndex();
        return createIndex(node->row(), 0, node);
    }
    
    QModelIndex index(int row, int col, const QModelIndex& parentIndex) const override {
        LayerDataNode* parentNode = nodeFromIndex(parentIndex);
        if (!parentNode || row < 0 || row >= parentNode->childCount())
            return {};
        return createIndex(row, col, parentNode->child(row));
    }
    
    QModelIndex parent(const QModelIndex& index) const override {
        if (!index.isValid()) return {};
        LayerDataNode* node = nodeFromIndex(index);
        LayerDataNode* parent = node->parent;
        if (!parent || parent == root) return {};
        return createIndex(parent->row(), 0, parent);
    }
    
    int rowCount(const QModelIndex& parent) const override {
        return (int) nodeFromIndex(parent)->childCount();
    }
    
    int columnCount(const QModelIndex&) const override { return 1; }
    
    QVariant data(const QModelIndex& index, int role) const override {
        if (!index.isValid()) return {};
        LayerDataNode* node = nodeFromIndex(index);
        switch (role) {
            case Qt::DisplayRole: return node->name;
            case Qt::EditRole: return node->name;
            case VisibleRole: return node->visible;
            case ParentVisibleRole: {
                LayerDataNode* parent = node->parent;
                while (parent) {
                    if (parent->visible) {
                        parent = parent->parent;
                    } else {
                        return false;
                    }
                }
                return true;
            }
//            case LockedRole:         return node->locked;
            case OpacityRole:        return node->opacity;
            case BlendModeRole:      return static_cast<int>(node->blendMode);
//            case ThumbnailRole:      return node->thumbnail;
            case LayerTypeRole: return static_cast<int>(node->type);
//            case NodePtrRole:        return QVariant::fromValue(static_cast<void*>(node));
            default: return {};
        }
    }
    
    void notifySubtreeChanged(const QModelIndex& parentIdx) {
        LayerDataNode* parentNode = nodeFromIndex(parentIdx);
        if (parentNode->children.isEmpty()) return;

        QModelIndex topLeft     = index(0, 0, parentIdx);
        QModelIndex bottomRight = index((int) parentNode->childCount() - 1, 0, parentIdx);
        emit dataChanged(topLeft, bottomRight);

        for (int i = 0; i < parentNode->childCount(); i++)
            notifySubtreeChanged(index(i, 0, parentIdx));
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role) override {
                
        if (!index.isValid()) return false;
        LayerDataNode* node = nodeFromIndex(index);
                
        switch (role) {
            case Qt::EditRole:
                node->name = value.toString();
                emit dataChanged(index, index, {role});
                return true;
            case OpacityRole:
                node->opacity = value.toFloat();
                emit dataChanged(index, index, {role});
                return true;
            case VisibleRole:
                node->visible = value.toBool();
                notifySubtreeChanged(index);
                return true;
            case BlendModeRole:
                node->blendMode = toBlendMode(value.toString());
                emit dataChanged(index, index, {role});
                return true;
            default:
                return false;
        }
        return true;
    }
    
    Qt::ItemFlags flags(const QModelIndex& index) const override {
        if (!index.isValid()) return Qt::ItemIsDropEnabled;  // root accepts drops
        Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable
                        | Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
        if (nodeFromIndex(index)->isGroup())
            f |= Qt::ItemIsDropEnabled;
        return f;
    }
    
    QStringList mimeTypes() const override {
        return { "application/x-paintapp-layer" };
    }
    
    LayerDataNode* findNodeById(LayerDataNode* node, const QUuid& id) const {
        if (node->id == id) return node;
        for (auto* child : node->children) {
            auto* found = findNodeById(child, id);
            if (found) return found;
        }
        return nullptr;
    }
    
    void moveNodeIntoGroup(LayerDataNode* node, const QModelIndex& newParentIdx, int insertRow) {
        LayerDataNode* newParent = nodeFromIndex(newParentIdx);
        LayerDataNode* oldParent = node->parent;
        int oldRow = node->row();

        QModelIndex oldParentIdx = oldParent == root
            ? QModelIndex()
            : createIndex(oldParent->row(), 0, oldParent);

        if (oldParent == newParent && oldRow < insertRow) insertRow--;

        beginRemoveRows(oldParentIdx, oldRow, oldRow);
        oldParent->children.removeAt(oldRow);
        endRemoveRows();

        insertRow = qBound(0, insertRow, (int) newParent->childCount());

        beginInsertRows(newParentIdx, insertRow, insertRow);
        node->parent = newParent;
        newParent->children.insert(insertRow, node);
        endInsertRows();
    }

    QMimeData* mimeData(const QModelIndexList& indexes) const override {
        auto* mime = new QMimeData;
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        for (const auto& idx : indexes)
            stream << nodeFromIndex(idx)->id;
        mime->setData("application/x-paintapp-layer", data);
        return mime;
    }

    bool dropMimeData(const QMimeData* mime,
                      Qt::DropAction action,
                      int row,
                      int /*col*/,
                      const QModelIndex& parentIdx) override {

        if (!mime->hasFormat("application/x-paintapp-layer")) return false;

        QByteArray data = mime->data("application/x-paintapp-layer");
        QDataStream stream(&data, QIODevice::ReadOnly);

        QList<LayerDataNode*> nodes;
        while (!stream.atEnd()) {
            QUuid id; stream >> id;
            LayerDataNode* node = findNodeById(root, id);
            if (node) nodes << node;
        }

        // if row == -1, drop was onto the parent item itself (into the group)
        int insertRow = (row == -1) ? 0 : row;
                
        for (int i = (int) nodes.size() - 1; i >= 0; i--) {
            moveNodeIntoGroup(nodes[i], parentIdx, insertRow);
        }

        return true;
    }

    Qt::DropActions supportedDropActions() const override {
        return Qt::MoveAction;
    }
    
private:
    int layerCount;
    int groupCount;
    LayerDataNode* root;
    
};

class LayerDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit LayerDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
    
    void paint(QPainter* p,
               const QStyleOptionViewItem& opt,
               const QModelIndex& idx) const override {
        
        bool visible = idx.data(LayerModel::VisibleRole).toBool();
        bool parentVisible = idx.data(LayerModel::ParentVisibleRole).toBool();
//        bool  locked  = idx.data(LayerModel::LockedRole).toBool();
//        QImage thumb  = idx.data(LayerModel::ThumbnailRole).value<QImage>();
        QString name = idx.data(Qt::DisplayRole).toString();
        auto type = static_cast<LayerType>(idx.data(LayerModel::LayerTypeRole).toInt());
        
        p->setRenderHint(QPainter::Antialiasing);
        
        // selected color
        
        if (opt.state & QStyle::State_Selected) {
            p->fillRect(opt.rect, opt.palette.highlight());
        }
                
        // visible icon
    
        p->setPen(Qt::white); // outline
        p->setBrush(Qt::white); // fill
        if (!visible) {
            p->setCompositionMode(QPainter::CompositionMode_Clear);
        } else if (!parentVisible) {
            p->setPen(Qt::darkGray);
            p->setBrush(Qt::darkGray);
        }
        p->drawEllipse(eyeIconRect(opt).center(), 5, 5);
        p->setCompositionMode(QPainter::CompositionMode_SourceOver);
        
    //    // Lock icon
    //    QIcon lockIcon = locked ? QIcon(":/icons/lock.svg") : QIcon(":/icons/unlock.svg");
    //    lockIcon.paint(p, lockIconRect(opt));
    
    //    // Thumbnail
    //    if (!thumb.isNull())
    //        p->drawImage(thumbnailRect(opt), thumb);
    //    else
    //        p->fillRect(thumbnailRect(opt), Qt::darkGray);
        
        // name and group indicator
        
        QRect nameRect = opt.rect.adjusted(PADDING*2 + ICON_SIZE + PADDING + THUMB_SIZE + PADDING, 0, 0, 0);
        
        switch (type) {
            case LayerType::Raster:
                p->setPen(Qt::white);
                break;
                
            case LayerType::Group:
                p->setPen(Qt::gray);
                break;
                
            default:
                return;
        }
        
        p->drawText(nameRect, Qt::AlignVCenter | Qt::AlignLeft, name);

    }
    
    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override {
        return QSize(0, ROW_HEIGHT);
    }
    
    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const override {
        auto* lineEdit = qobject_cast<QLineEdit*>(editor);
        if (lineEdit) {
            model->setData(index, lineEdit->text(), Qt::EditRole);
        }
    }
    
    bool editorEvent(QEvent* event,
                     QAbstractItemModel* model,
                     const QStyleOptionViewItem& opt,
                     const QModelIndex& idx) override {
        
        if (event->type() == QEvent::MouseButtonPress) {
            
            auto* me = static_cast<QMouseEvent*>(event);
            
            if (eyeIconRect(opt).contains(me->pos())) {
                qDebug() << "[layer delegate] name " << idx.data(Qt::DisplayRole).toString();
                emit visibilityToggled(idx);
                return true;
            }
            
//            if (lockIconRect(opt).contains(me->pos())) {
//                emit lockToggled(idx);
//                return true;
//            }
            
        }
        
        return QStyledItemDelegate::editorEvent(event, model, opt, idx);

    }

signals:
    void visibilityToggled(const QModelIndex&);
    void lockToggled(const QModelIndex&);

private:
    static constexpr int ROW_HEIGHT = 24;
    static constexpr int THUMB_SIZE = 18;
    static constexpr int ICON_SIZE = 16;
    static constexpr int PADDING = 6;
    
    QRect eyeIconRect(const QStyleOptionViewItem& opt) const {
        return QRect(opt.rect.left() + PADDING,
                     opt.rect.top() + (opt.rect.height() - ICON_SIZE) / 2,
                     ICON_SIZE,
                     ICON_SIZE);
    }
    
    QRect lockIconRect(const QStyleOptionViewItem&) const;
    QRect thumbnailRect(const QStyleOptionViewItem&) const;

};

//class ReversedLayerProxy : public QSortFilterProxyModel {
//    
//public:
//    ReversedLayerProxy(QWidget* parent = nullptr) : QSortFilterProxyModel(parent) {}
//    
//protected:
//    
//    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override {
//        if (!proxyIndex.isValid()) return {};
//        QModelIndex sourceParent = mapToSource(proxyIndex.parent());
//        int reversedRow = sourceModel()->rowCount(sourceParent) - 1 - proxyIndex.row();
//        return sourceModel()->index(reversedRow, proxyIndex.column(), sourceParent);
//    }
//
//    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override {
//        if (!sourceIndex.isValid()) return {};
//        QModelIndex proxyParent = mapFromSource(sourceIndex.parent());
//        int reversedRow = sourceModel()->rowCount(sourceIndex.parent()) - 1 - sourceIndex.row();
//        return createIndex(reversedRow, sourceIndex.column(), sourceIndex.internalPointer());
//    }
//
//};

class LayersTreeView : public QTreeView {
    Q_OBJECT
    
public:
    explicit LayersTreeView(QWidget* parent = nullptr) : QTreeView(parent) {}

protected:
    void mousePressEvent(QMouseEvent* event) override {
        QModelIndex idx = indexAt(event->pos());
        if (!idx.isValid()) {
            // do not deselect layer
            return;
        }
        QTreeView::mousePressEvent(event);
    }
    
    void dropEvent(QDropEvent* event) override {
        QModelIndex dropTarget = indexAt(event->position().toPoint());
        QAbstractItemView::DropIndicatorPosition pos = dropIndicatorPosition();

        QTreeView::dropEvent(event); // let the model do the move first

        // if dropped *onto* an item (into a group), expand it
        if (dropTarget.isValid() && pos == QAbstractItemView::OnItem) {
            expand(dropTarget);
        }
    }
    
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override {
        QTreeView::selectionChanged(selected, deselected);
        emit signalSelectionChanged();
        return;
    }
    
signals:
    void signalSelectionChanged();
    
};

class LayersTreePanel : public QWidget {
    Q_OBJECT
    
public:
    explicit LayersTreePanel(QWidget* parent = nullptr) : QWidget(parent) {
        
        // main view
        
        view = new LayersTreeView(this);
        
        model = new LayerModel(this);

//        auto* proxy = new ReversedLayerProxy(this);
//        proxy->setSourceModel(model);
//        view->setModel(proxy);
        
        view->setModel(model);
        
        delegate = new LayerDelegate(this);
        
        view->setItemDelegate(delegate);
        view->setDragDropMode(QAbstractItemView::InternalMove);
        view->setSelectionMode(QAbstractItemView::ExtendedSelection);
        
        view->setDefaultDropAction(Qt::MoveAction);
        view->setHeaderHidden(true);
        view->setIndentation(16);
        view->setAnimated(true);
        view->setUniformRowHeights(true); // important for perf
        
        connect(delegate, &LayerDelegate::visibilityToggled,
                this, [this](const QModelIndex& idx) {
            bool cur = idx.data(LayerModel::VisibleRole).toBool();
            qDebug() << "[layers tree panel] visibility toggled " << idx.data(Qt::DisplayRole).toString() << ", curr: " << cur;
            model->setData(idx, !cur, LayerModel::VisibleRole);
        });
        
        // menu
        
        // todo: connect these menu items with the selected layer
                
        blendModeDropdown = new QComboBox(this);
        blendModeDropdown->addItem(toString(BlendMode::Normal));
        blendModeDropdown->addItem(toString(BlendMode::Multiply));
        
        // todo: mapping between values, ints, etc.
        
        blendModeDropdown->setCurrentIndex(0);
                
        connect(blendModeDropdown, &QComboBox::currentTextChanged,
                this, [this](const QString& value) {
            
            qDebug() << "[layers tree panel] blend mode changed " << value;
            
            QModelIndexList selected = view->selectionModel()->selectedIndexes();
            
            if (selected.size() != 1) return;
                        
            model->setData(selected.first(),
                           value,
                           LayerModel::BlendModeRole);
                        
        });

        opacityLineEdit = new QLineEdit(this);
        opacityValidator = new QDoubleValidator(0.0, 1.0, 2, opacityLineEdit);
        opacityValidator->setNotation(QDoubleValidator::StandardNotation);
        opacityLineEdit->setText("1.00");
        
        connect(opacityLineEdit, &QLineEdit::editingFinished,
                this, [this](){
        
            int pos = 0;
            QString value = opacityLineEdit->text();
            QModelIndexList selected = view->selectionModel()->selectedIndexes();
            
            if (selected.size() != 1) return; // invalid selection
            
            if (opacityValidator->validate(value, pos) == QValidator::Acceptable) {
                
                qDebug() << "[layers tree panel] opacity valid";
                
                float opacity = value.toFloat();
                opacityLineEdit->setText(QString::number(opacity, 'f', 2));
                opacityLineEdit->deselect();
                opacityLineEdit->clearFocus();
                
                model->setData(selected.first(),
                               opacity,
                               LayerModel::OpacityRole);
                
                setDirtyFlag(true);
                
            } else { // invalid value
                // todo: popup on why this is invalid
                // todo: restore previous value
                qDebug() << "[layers tree panel] opacity invalid";
                opacityLineEdit->setText("1.00");
            }

        });
        
        // start disabled
        
        blendModeDropdown->setDisabled(true);
        opacityLineEdit->setDisabled(true);
        
        connect(view, &LayersTreeView::signalSelectionChanged,
                this, [this]() {
            
            qDebug() << "[layers tree panel] selection changed";
            
            QModelIndexList selected = view->selectionModel()->selectedIndexes();
            
            if (selected.size() != 1) { // disable if num layer is != 1
                blendModeDropdown->setDisabled(true);
                opacityLineEdit->setDisabled(true);
                
                // todo: placeholder values

            } else {
                blendModeDropdown->setDisabled(false);
                opacityLineEdit->setDisabled(false);
                                
                int blendMode = model->data(selected.first(),
                                            LayerModel::BlendModeRole).toInt();
                float opacity = model->data(selected.first(),
                                            LayerModel::OpacityRole).toFloat();
                blendModeDropdown->setCurrentIndex(blendMode);
                opacityLineEdit->setText(QString::number(opacity, 'f', 2));
                
            }

        });
        
        // buttons
        
        groupButton = new QToolButton(this);
        groupButton->setText("g");
        
        connect(groupButton, &QToolButton::clicked, this, [this]() {
            
            QModelIndexList selected = view->selectionModel()->selectedIndexes();
            
            // todo: undo stack
            
            if (selected.size() <= 1) { // empty group
                
                QModelIndex idx = view->selectionModel()->currentIndex();
                
                QModelIndex parentIdx;
                int insertRow;
                if (!idx.isValid()) {
                    parentIdx = QModelIndex();
                    insertRow = 0;
                } else {
                    parentIdx = idx.parent();
                    insertRow = idx.row();
                }
                model->addLayer(parentIdx, insertRow, LayerType::Group);
                
            } else { // group with selected
                
                // build group
                
                int insertRow = std::numeric_limits<int>::max();
                QList<LayerDataNode*> nodesToMove;
                QModelIndex parentIdx = selected.first().parent();
                
                std::sort(selected.begin(), selected.end(), [](const QModelIndex& index1, const QModelIndex& index2) {
                    return index1.row() > index2.row();
                });
                                
                for (const auto& idx : selected) {
                    insertRow = std::min(insertRow, idx.row());
                    nodesToMove << model->nodeFromIndex(idx);
                    if (idx.parent() != parentIdx) {
                        qDebug() << "layers must have the same parent";
                        return;
                    }
                }
                                
                QModelIndex groupIdx = model->addLayer(parentIdx, insertRow, LayerType::Group);
                
                // move selected nodes
                
                for (LayerDataNode* node : nodesToMove) {
                    model->moveNodeIntoGroup(node, groupIdx, -1);
                }
                
                LayerDataNode* groupNode = model->nodeFromIndex(groupIdx);
                QModelIndex freshGroupIdx = model->indexFromNode(groupNode);
                view->expand(freshGroupIdx);
                view->selectionModel()->setCurrentIndex(freshGroupIdx, QItemSelectionModel::ClearAndSelect);
                view->scrollTo(freshGroupIdx);
                
            }
            
        });

        addButton = new QToolButton(this);
        addButton->setText("+");
                
        connect(addButton, &QToolButton::clicked,
                this, [this]() {
                        
            QModelIndex selected = view->selectionModel()->currentIndex();
            
            QModelIndex parentIdx;
            int insertRow;

            if (!selected.isValid()) {
                parentIdx = QModelIndex();
                insertRow = 0;
            } else {
                parentIdx = selected.parent();
                insertRow = selected.row();
            }

            // todo: undo stack
            QModelIndex newIdx = model->addLayer(parentIdx, insertRow, LayerType::Raster);
            
            view->selectionModel()->setCurrentIndex(newIdx, QItemSelectionModel::ClearAndSelect);
            view->scrollTo(newIdx);
            
            setDirtyFlag(true);
            
        });

        removeButton = new QToolButton(this);
        removeButton->setText("-");
        
        connect(removeButton, &QToolButton::clicked,
                this, [this]() {
            
            QModelIndexList selectedList = view->selectionModel()->selectedRows();
            
            if (selectedList.size() > 1 || selectedList.size() <= 0) {
                qDebug() << "[layers tree panel] removing multiple or no selected rows is not supported";
                return;
            }
            
            QModelIndex selected = selectedList[0];
            
            QModelIndex parentIdx;
            if (!selected.isValid()) {
                qDebug() << "[layers tree panel] selection is not valid";
            } else {
                parentIdx = selected.parent();
            }

            // todo: undo stack
//            m_undoStack->push(new AddLayerCommand(m_model, parentIdx, insertRow));
            model->removeLayer(parentIdx, selected);
            
            setDirtyFlag(true);
                        
        });

        // layout
        
        auto* menuLayout = new QHBoxLayout;
        menuLayout->addWidget(blendModeDropdown);
        menuLayout->addWidget(opacityLineEdit);

        auto* buttonsLayout = new QHBoxLayout;
        buttonsLayout->addStretch();
        buttonsLayout->addWidget(groupButton);
        buttonsLayout->addWidget(addButton);
        buttonsLayout->addWidget(removeButton);

        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->addLayout(menuLayout);
        mainLayout->addWidget(view);
        mainLayout->addLayout(buttonsLayout);

        setLayout(mainLayout);

    }
    
public slots:
    void onQuery() { // if another service queries this panel ...
        // return all the layers, also
        // query dirty?
    }
    
    void onSync() { // if we induct another canvas ...
        
    }
    
private:
    LayersTreeView* view;
    
    LayerDelegate* delegate;
    LayerModel* model;
    QUndoStack* undoStack;
    
    bool dirty;
        
    QComboBox* blendModeDropdown;

    QLineEdit* opacityLineEdit;
    QDoubleValidator* opacityValidator;
    
    QToolButton* groupButton;
    QToolButton* addButton;
    QToolButton* removeButton;
    
    void setDirtyFlag(bool flag) {
        dirty = flag;
    }

};























// ---

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
    
    void onVisibilityChanged(bool visibility) {
        qDebug() << "[layer panel] visibility changed";
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
        
        // todo: connect visibility signal
        
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
