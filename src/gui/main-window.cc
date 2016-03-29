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
#include <QToolBar>
#include <QAction>
#include <QComboBox>
#include <QFontComboBox>
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
      // why pointer?
      debate = new Debate(this);
      tabs.addTab(debate, "Interpreter");
      auto s = debate->widget()->frameSize();
      s.rwidth() += debate->verticalScrollBar()->width();
      s.rheight() += debate->horizontalScrollBar()->width();
      resize(s);

      QMenu* menu;
      menu = menuBar()->addMenu(tr("&File"));
      QAction* action = new QAction(tr("New Worksheet"), this);
      action->setShortcut(tr("Ctrl+N"));
      menu->addAction(action);
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(new_file()));
      action = new QAction(tr("Open Worksheet"), this);
      action->setShortcut(tr("Ctrl+O"));
      menu->addAction(action);
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(open_file()));
      action = new QAction(tr("Save Worksheet"), this);
      action->setShortcut(tr("Ctrl+S"));
      menu->addAction(action);
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(save_file()));
      action = new QAction(tr("Quit"), this);
      action->setShortcut(tr("Ctrl+Q"));
      menu->addAction(action);
      connect(action, SIGNAL(triggered()), this, SLOT(close()));

      menu = menuBar()->addMenu(tr("E&xecute"));
      action = new QAction(tr("Comment Selection"), this);
      action->setShortcut(QKeySequence(Qt::Key_Return + Qt::CTRL));
      menu->addAction(action);
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(only_comment()));

      action = new QAction(tr("Execute Selection"), this);
      action->setShortcut(QKeySequence(Qt::Key_Return + Qt::SHIFT));
      menu->addAction(action);
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(send_query()));

      action = new QAction(tr("Execute All"), this);
      action->setShortcut(tr("Ctrl+A"));
      menu->addAction(action);
      // Fix this
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(send_query()));


      menu = menuBar()->addMenu(tr("&Edit"));
      action = new QAction(tr("Read Input"), this);
      action->setShortcut(tr("Ctrl+R"));
      menu->addAction(action);
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(read_file()));
      action = new QAction(tr("Write Input"), this);
      action->setShortcut(tr("Ctrl+W"));
      menu->addAction(action);
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(write_file()));
      menu->addSeparator();
      action = new QAction(tr("&Undo"), this);
      action->setShortcut(QKeySequence::Undo);
      menu->addAction(action);
      action = new QAction(tr("&Redo"), this);
      action->setShortcut(QKeySequence::Redo);
      menu->addAction(action);
      menu->addSeparator();
      action = new QAction(tr("Cu&t"), this);
      action->setShortcut(QKeySequence::Cut);
      menu->addAction(action);
      action = new QAction(tr("&Copy"), this);
      action->setShortcut(QKeySequence::Copy);
      menu->addAction(action);
      action = new QAction(tr("&Paste"), this);
      action->setShortcut(QKeySequence::Paste);
      menu->addAction(action);

      menu = menuBar()->addMenu(tr("F&ormat"));
      action = new QAction(tr("&Underline"), this);
      menu->addAction(action);
      action->setShortcut(QKeySequence());
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(underline()));

      action = new QAction(tr("&Bold"), this);
      action->setShortcut(Qt::CTRL + Qt::Key_B);
      QFont bold;
      bold.setBold(true);
      action->setFont(bold);
      action->setCheckable(true);
      menu->addAction(action);
      connect(action, SIGNAL(triggered()), debate->conversation(), SLOT(textBold()));

      action = new QAction(tr("&Italic"), this);
      action->setShortcut(Qt::CTRL + Qt::Key_I);
      QFont italic;
      italic.setItalic(true);
      action->setFont(italic);
      action->setCheckable(true);
      menu->addAction(action);
      connect(action, SIGNAL(triggered()), this, SLOT(textItalic()));

      auto actionTextUnderline = new QAction(tr("&Underline"), this);
      actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
      actionTextUnderline->setPriority(QAction::LowPriority);
      QFont underline;
      underline.setUnderline(true);
      actionTextUnderline->setFont(underline);
      actionTextUnderline->setCheckable(true);
      connect(actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
      menu->addAction(actionTextUnderline);

      menu->addSeparator();

      QActionGroup *grp = new QActionGroup(this);
      connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

      auto actionAlignLeft = new QAction(tr("&Left"), grp);
      auto actionAlignCenter = new QAction(tr("C&enter"), grp);
      auto actionAlignRight = new QAction(tr("&Right"), grp);
      auto actionAlignJustify = new QAction(tr("&Justify"), grp);
      actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
      actionAlignLeft->setCheckable(true);
      actionAlignLeft->setPriority(QAction::LowPriority);
      actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
      actionAlignCenter->setCheckable(true);
      actionAlignCenter->setPriority(QAction::LowPriority);
      actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
      actionAlignRight->setCheckable(true);
      actionAlignRight->setPriority(QAction::LowPriority);
      actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
      actionAlignJustify->setCheckable(true);
      actionAlignJustify->setPriority(QAction::LowPriority);
      menu->addActions(grp->actions());

      menu->addSeparator();

      auto actionTextColor = new QAction(tr("&Color..."), this);
      connect(actionTextColor, SIGNAL(triggered()), this, SLOT(textColor()));
      menu->addAction(actionTextColor);

      auto tb = new QToolBar(this);
      //tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
      tb->setWindowTitle(tr("Format Actions"));
      addToolBarBreak(Qt::TopToolBarArea);
      addToolBar(tb);

      auto comboStyle = new QComboBox(tb);
      tb->addWidget(comboStyle);
      comboStyle->addItem("Standard");
      comboStyle->addItem("Bullet List (Disc)");
      comboStyle->addItem("Bullet List (Circle)");
      comboStyle->addItem("Bullet List (Square)");
      comboStyle->addItem("Ordered List (Decimal)");
      comboStyle->addItem("Ordered List (Alpha lower)");
      comboStyle->addItem("Ordered List (Alpha upper)");
      comboStyle->addItem("Ordered List (Roman lower)");
      comboStyle->addItem("Ordered List (Roman upper)");
      connect(comboStyle, SIGNAL(activated(int)),
              this, SLOT(textStyle(int)));

      auto comboFont = new QFontComboBox(tb);
      tb->addWidget(comboFont);
      connect(comboFont, SIGNAL(activated(QString)),
              this, SLOT(textFamily(QString)));

      auto comboSize = new QComboBox(tb);
      comboSize->setObjectName("comboSize");
      tb->addWidget(comboSize);
      comboSize->setEditable(true);

      QFontDatabase db;
      foreach(int size, db.standardSizes())
          comboSize->addItem(QString::number(size));

      connect(comboSize, SIGNAL(activated(QString)),
              this, SLOT(textSize(QString)));
      comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font()
                                                                     .pointSize())));




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
