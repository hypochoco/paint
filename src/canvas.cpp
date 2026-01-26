//
//  canvas.cpp
//  main
//
//  Created by Daniel Cho on 12/25/25.
//

#include "paint/canvas.h"

void CanvasWindow::exposeEvent(QExposeEvent *event) {
    qDebug() << "[canvas window] exposed event " << isExposed();
    QWindow::exposeEvent(event);
    if (isExposed()) {
        requestUpdate();
    }
    emit exposed(isExposed());
}

bool CanvasWindow::event(QEvent* event) {
    
//    if (event->type() == QEvent::UpdateRequest) {
//        event->accept();
//        return true;
//    }

    if (event->type() == QEvent::PlatformSurface) {
        auto *surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event);

        switch (surfaceEvent->surfaceEventType()) { // on start
            case QPlatformSurfaceEvent::SurfaceCreated:
                qDebug() << "[canvas window] surface created event";
                event->accept();
                emit surfaceCreated(this);
                return true;
                
            case QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed: // on close
                qDebug() << "[canvas window] surface about to be destroyed event";
                event->accept();
                emit surfaceAboutToBeDestroyed();
                return true;
                
            default:
                break;
        }
    }

    return QWindow::event(event);

}

void CanvasWindow::resizeEvent(QResizeEvent *event) {
    qDebug() << "[canvas window] resize event";
    QWindow::resizeEvent(event);
    emit resized(event->size().width(), event->size().height());
}

void CanvasWindow::mousePressEvent(QMouseEvent* event) {
    QWindow::mousePressEvent(event);
    if (event->button() ==  Qt::LeftButton) {
        qDebug() << "[canvas window] left mouse press event";
        leftMouseDown = true;
    }
    emit leftButtonPressed(event->pos().x(), event->pos().y());
}

void CanvasWindow::mouseReleaseEvent(QMouseEvent* event) {
    QWindow::mouseReleaseEvent(event);
    if (event->button() ==  Qt::LeftButton) {
        qDebug() << "[canvas window] left mouse release event";
        leftMouseDown = false;
    }
    emit leftButtonReleased(event->pos().x(), event->pos().y());
}

void CanvasWindow::mouseMoveEvent(QMouseEvent *event) {
    QWindow::mouseMoveEvent(event);
    if (leftMouseDown) {
        emit mouseMoved(event->pos().x(), event->pos().y());
    }
}

void CanvasWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Q) {
        qDebug() << "[canvas window] q key was pressed";
        event->accept();
    } else {
        QWindow::keyPressEvent(event);
    }
}
