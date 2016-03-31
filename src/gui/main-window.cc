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
      conv = debate->conversation();
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
      conv->undo = new QAction(tr("&Undo"), this);
      conv->undo->setShortcut(QKeySequence::Undo);
      menu->addAction(conv->undo);
      conv->redo = new QAction(tr("&Redo"), this);
      conv->redo->setShortcut(QKeySequence::Redo);
      menu->addAction(conv->redo);
      menu->addSeparator();
      conv->cut = new QAction(tr("Cu&t"), this);
      conv->cut->setShortcut(QKeySequence::Cut);
      menu->addAction(conv->cut);
      conv->copy = new QAction(tr("&Copy"), this);
      conv->copy->setShortcut(QKeySequence::Copy);
      menu->addAction(conv->copy);
      conv->paste = new QAction(tr("&Paste"), this);
      conv->paste->setShortcut(QKeySequence::Paste);
      menu->addAction(conv->paste);
      menu->addSeparator();
      conv->insert_list = new QAction(tr("Insert &List"), this);
      conv->insert_list->setShortcut(Qt::CTRL + Qt::Key_L);
      menu->addAction(conv->insert_list);
      conv->insert_table = new QAction(tr("Insert &Table"), this);
      conv->insert_table->setShortcut(Qt::CTRL + Qt::Key_E);
      menu->addAction(conv->insert_table);

      menu = menuBar()->addMenu(tr("F&ormat"));

      // These actions belong to Conversation and connect to the current Question.
      conv->bold = new QAction(tr("&Bold"), this);
      conv->bold->setShortcut(Qt::CTRL + Qt::Key_B);
      QFont bold; bold.setBold(true); conv->bold->setFont(bold);
      conv->bold->setCheckable(true);
      menu->addAction(conv->bold);

      conv->italic = new QAction(tr("&Italic"), this);
      conv->italic->setShortcut(Qt::CTRL + Qt::Key_I);
      QFont italic; italic.setItalic(true); conv->italic->setFont(italic);
      conv->italic->setCheckable(true);
      menu->addAction(conv->italic);

      conv->underline = new QAction(tr("&Underline"), this);
      conv->underline->setShortcut(Qt::CTRL + Qt::Key_U);
      QFont underline; underline.setUnderline(true); conv->underline->setFont(underline);
      conv->underline->setCheckable(true);
      menu->addAction(conv->underline);

      QAction* font;
      font = new QAction(tr("&Font"), this);
      font->setShortcut(Qt::CTRL + Qt::Key_F);
      font->setCheckable(true);
      menu->addAction(font);

      menu->addSeparator();

      QActionGroup *grp = new QActionGroup(this);
      //connect(grp, SIGNAL(triggered(QAction*)), debate->conversation(), SLOT(align(QAction*)));

      conv->alignLeft = new QAction(tr("&Left"), grp);
      conv->alignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
      conv->alignLeft->setCheckable(true);

      conv->alignCenter = new QAction(tr("C&enter"), grp);
      conv->alignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
      conv->alignCenter->setCheckable(true);

      conv->alignRight = new QAction(tr("&Right"), grp);
      conv->alignRight->setShortcut(Qt::CTRL + Qt::Key_R);
      conv->alignRight->setCheckable(true);

      conv->alignJustify = new QAction(tr("&Justify"), grp);
      conv->alignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
      conv->alignJustify->setCheckable(true);

      menu->addActions(grp->actions());

      QAction* style;
      style = new QAction(tr("&Style"), this);
      style->setShortcut(Qt::CTRL + Qt::Key_S);
      style->setCheckable(true);
      menu->addAction(style);

      menu->addSeparator();

      auto actionTextColor = new QAction(tr("&Color..."), this);
      connect(actionTextColor, SIGNAL(triggered()), this, SLOT(textColor()));
      menu->addAction(actionTextColor);

      auto tb1 = new QToolBar(this);
      tb1->hide();
      tb1->setWindowTitle(tr("Font Actions"));
      addToolBar(tb1);

      conv->style = new QComboBox(tb1);
      tb1->addWidget(conv->style);
      conv->style->addItem("Standard");
      conv->style->addItem("Bullet List (Disc)");
      conv->style->addItem("Bullet List (Circle)");
      conv->style->addItem("Bullet List (Square)");
      conv->style->addItem("Ordered List (Decimal)");
      conv->style->addItem("Ordered List (Alpha lower)");
      conv->style->addItem("Ordered List (Alpha upper)");
      conv->style->addItem("Ordered List (Roman lower)");
      conv->style->addItem("Ordered List (Roman upper)");

      connect(style, SIGNAL(toggled(bool)), tb1, SLOT(setVisible(bool)));

      auto tb2 = new QToolBar(this);
      tb2->hide();
      tb2->setWindowTitle(tr("Style Actions"));
      addToolBar(tb2);

      conv->fontName = new QFontComboBox(tb2);
      tb2->addWidget(conv->fontName);

      conv->fontSize = new QComboBox(tb2);
      conv->fontSize->setObjectName("comboSize");
      tb2->addWidget(conv->fontSize);
      conv->fontSize->setEditable(true);

      QFontDatabase db;
      foreach(int size, db.standardSizes())
          conv->fontSize->addItem(QString::number(size));

      conv->fontSize->setCurrentIndex(conv->fontSize->findText(QString::number(QApplication::font()
                                                                     .pointSize())));

      connect(font, SIGNAL(toggled(bool)), tb2, SLOT(setVisible(bool)));


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
