#include <QtWidgets>
#include <QDebug>
#include <iostream>

#include "window.h"
#include "image_window.h"
#include "about_window.h"
#include "canvas.h"
#include "slides.h"

GUI::Window::Window(QApplication* app) : _app(app){
  // workspace = new QMdiArea(this);
  // workspace->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  // workspace->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  // setCentralWidget(workspace);

  // as long as we have no toolbar here
  QPixmap icon_png(":Icon/256x256/eagleeye.png");
  // icon_png = icon_png.scaled(this->size(), Qt::IgnoreAspectRatio);
  QPalette palette;
  palette.setBrush(QPalette::Background, icon_png);
  this->setPalette(palette);

  _openPath = QDir::currentPath();

  _newWindowAct = new QAction(tr("&New"), this );
  _newWindowAct->setShortcut(tr("Ctrl+N"));
  _newWindowAct->setStatusTip(tr("Create a new Window"));
  connect(_newWindowAct, SIGNAL(triggered()), this, SLOT(slotNewWindowAction()));

  _dialogWindowAct = new QAction(tr("&About"), this );
  _dialogWindowAct->setShortcut(tr("F1"));
  _dialogWindowAct->setStatusTip(tr("Create a new Window"));
  connect(_dialogWindowAct, SIGNAL(triggered()), this, SLOT(slotDialogWindowAction()));

  _windowMenu = menuBar()->addMenu(tr("&Window"));
  _windowMenu->addAction(_newWindowAct);
  _windowMenu->addAction(_dialogWindowAct);

  slotNewWindowAction();

}

QSize GUI::Window::sizeHint() const {
  return QSize(256, 256);
}


void GUI::Window::slotNewWindowAction() {
  qDebug() << "GUI::Window::slotNewWindowAction()";

  GUI::ImageWindow* tmpWindow = new GUI::ImageWindow(this, this);
  tmpWindow->setMinimumSize(200, 200);
  tmpWindow->show();

  connect( tmpWindow, SIGNAL(sigToggleChained(ImageWindow*, bool)),
           this, SLOT(slotActualizeSubwindowView( ImageWindow*, bool )));

  connect( tmpWindow, SIGNAL(sigCoordToMainwindow( QPoint)),
           this, SLOT( slotUpdateCoords( QPoint)));

  connect( tmpWindow, SIGNAL(sigMarkerToMainwindow( Marker)),
           this, SLOT( slotUpdateMarkers( Marker)));

  connect( tmpWindow, SIGNAL(sigPropertyToMainwindow( Canvas::property_t)),
           this, SLOT( slotUpdateProperties( Canvas::property_t)));

  connect( this, SIGNAL(sigDistributeCoord(QPoint)),
           tmpWindow, SLOT(slotShowCoords(QPoint)));

  connect( this, SIGNAL(sigDistributeMarker(Marker)),
           tmpWindow, SLOT(slotShowMarkers(Marker)));

  connect( this, SIGNAL(sigDistributeProperty(Canvas::property_t)),
           tmpWindow, SLOT(slotShowProperty(Canvas::property_t)));

  tmpWindow->synchronize(true);
  _windows.push_back(tmpWindow);
}
void GUI::Window::slotDialogWindowAction() {
  qDebug() << "GUI::Window::slotDialogWindowAction()";
  AboutWindow* dialog = new AboutWindow(this);
  dialog->setAttribute(Qt::WA_DeleteOnClose);

  dialog->setGeometry(
      QStyle::alignedRect(
          Qt::LeftToRight,
          Qt::AlignCenter,
          dialog->size(),
          _app->desktop()->availableGeometry()
      )
  );
  dialog->show();
}


void GUI::Window::slotActualizeSubwindowView( ImageWindow* window, bool set) {
  Q_UNUSED(set);

  connect( window, SIGNAL(sigUpdateConnectedViews(Canvas*)),
           this, SLOT(slotUpdateConnectedViews(Canvas*)) );
  connect( this, SIGNAL(sigUpdateConnectedViews(Canvas*)),
           window, SLOT(slotSynchronizeConnectedViews(Canvas*)) );
}

void GUI::Window::slotUpdateConnectedViews( Canvas* buf ) {
  emit sigUpdateConnectedViews(buf);
}

void GUI::Window::slotUpdateCoords(QPoint p) {
  emit sigDistributeCoord(p);
}

void GUI::Window::slotUpdateMarkers(Marker m) {
  emit sigDistributeMarker(m);
}

void GUI::Window::slotUpdateProperties(Canvas::property_t m) {
  emit sigDistributeProperty(m);
}
