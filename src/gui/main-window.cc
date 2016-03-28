// Copyright (C) 2011-2013, Gabriel Dos Reis.
// All rights reserved.
// Written by Gabriel Dos Reis.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     - Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     - Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in
//       the documentation and/or other materials provided with the
//       distribution.
//
//     - Neither the name of OpenAxiom. nor the names of its contributors
//       may be used to endorse or promote products derived from this
//       software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <QMenuBar>
#include <QAction>
#include <QApplication>
#include <QTimer>
#include <QThread>
#include <QMessageBox>
#include <QScrollBar>

#include "debate.h"
#include "main-window.h"

#include "latexthread.h"

namespace OpenAxiom {
   void
   MainWindow::display_error(QString s) {
      QMessageBox::critical(this, tr("System error"), s);
   }

   void
   MainWindow::read_databases() {
   }


   static void connect_server_io(MainWindow* win, Debate* debate) {
      QObject::connect(win->server(), SIGNAL(readyReadStandardError()),
                       win, SLOT(display_error()));
      QObject::connect( win->server(), SIGNAL(readyReadStandardOutput()),
                       debate->conversation(), SLOT(read_reply()));
   }

   MainWindow::MainWindow(int argc, char* argv[])
         : srv(argc, argv), tabs(this) {

      setCentralWidget(&tabs);
      setWindowTitle("OpenAxiom");
      debate = new Debate(this);
      tabs.addTab(debate, "Interpreter");
      auto s = debate->widget()->frameSize();
      s.rwidth() += debate->verticalScrollBar()->width();
      s.rheight() += debate->horizontalScrollBar()->width();
      resize(s);
      QMenu* file = menuBar()->addMenu(tr("&File"));

      QAction* action = new QAction(tr("New Worksheet"), this);
      file->addAction(action);
      action->setShortcut(tr("Ctrl+N"));
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(new_file()));

      action = new QAction(tr("Open Worksheet"), this);
      file->addAction(action);
      action->setShortcut(tr("Ctrl+O"));
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(open_file()));

      action = new QAction(tr("Save Worksheet"), this);
      file->addAction(action);
      action->setShortcut(tr("Ctrl+S"));
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(save_file()));

      action = new QAction(tr("Read Input"), this);
      file->addAction(action);
      action->setShortcut(tr("Ctrl+R"));
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(read_file()));

      action = new QAction(tr("Write Input"), this);
      file->addAction(action);
      action->setShortcut(tr("Ctrl+W"));
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(write_file()));

      action = new QAction(tr("Quit"), this);
      file->addAction(action);
      action->setShortcut(tr("Ctrl+Q"));
      connect(action, SIGNAL(triggered()), this, SLOT(close()));

      QMenu* edit = menuBar()->addMenu(tr("&Edit"));
      // Fix this
      action = new QAction(tr("&Underline"), this);
      edit->addAction(action);
      action->setShortcut(QKeySequence());
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(underline()));

      latexthread = new LatexThread();
      connect(latexthread, SIGNAL(error(QString)), this, SLOT(display_error(QString)));
      connect(latexthread, SIGNAL(got_image(QImage, OutputTextArea*, int)), debate->conversation(), SLOT(add_image(QImage, OutputTextArea*, int)));
      latexthread->start();

      // When invoked in a --role=server mode, OpenAxiom would
      // wait to be pinged before displaying a prompt.  This is
      // an unfortunate result of a rather awkward hack.
      server()->launch();
      // Why this?
      //read_databases();
      connect_server_io(this, debate);
      server()->input(")read init.input )quiet");
   }

   MainWindow::~MainWindow() {
   }

   MainWindow *newMW;

   void MainWindow::done(int s, QProcess::ExitStatus) {
      // For the time being, shut done the whole application
      // if the interpreter quits.  FIXME.
      QApplication::exit(s);
   }

   void MainWindow::display_error() {
      auto s = server()->readAllStandardError();
      QMessageBox::critical(this, tr("System error"), QString(s));
   }
}
