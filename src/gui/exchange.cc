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

#include <string>
#include <sstream>
#include <iostream>

#include <QMessageBox>
#include <QTextFrame>
#include <QTextBlockFormat>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>
#include <QApplication>
#include <QKeyEvent>
#include <QBuffer>
#include <QFileDialog>
#include <QMenu>
#include "latexthread.h"

namespace OpenAxiom {

   // Return a resonable margin for this frame.
   static int our_margin(const QFrame* f) {
      return 2 + f->frameWidth() + 8;
   }

   // --------------------
   // -- OutputTextArea --
   // --------------------
   OutputTextArea::OutputTextArea(QWidget* p)
         : Base(p), cur(document()) {
      get_cursor().movePosition(QTextCursor::End);
      setReadOnly(true);          // this is a output only area.
      //setLineWrapMode(NoWrap);    // for the time being, mess with nothing.
      setAcceptRichText(true);
      setFont(p->font());
      setViewportMargins(0, 0, 0, 0);
      setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
      // We do not want to see scroll bars.  Usually disallowing vertical
      // scroll bars and allocating enough horizontal space is sufficient
      // to ensure that we don't see any horizontal scrollbar.
      setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      setLineWidth(1);
   }

   void OutputTextArea::align_left(const QString& s) {
      if (not document()->isEmpty()) {
          QTextBlockFormat blockFmt;
          blockFmt.setAlignment(Qt::AlignLeft);
          get_cursor().insertBlock(blockFmt);
      }
      get_cursor().insertText(s);
      resize(parentWidget()->width() - 2*our_margin(this),document()->size().height());
   }

   void OutputTextArea::align_right(const QString& s,const QTextCharFormat f) {
      QTextBlockFormat blockFmt;
      blockFmt.setAlignment(Qt::AlignRight);
      get_cursor().insertBlock(blockFmt);
      get_cursor().insertText(s,f);
      resize(parentWidget()->width() - 2*our_margin(this),document()->size().height());
   }

   // called from LatexThread signal
   void OutputTextArea::add_image(OutputTextArea *where, int pos, const QImage& s) {
      where->get_cursor().setPosition(pos);
      where->get_cursor().deletePreviousChar(); // marker
      where->get_cursor().insertImage(s);
      where->resize(parentWidget()->width() - 2*our_margin(where),where->document()->size().height());
   }

   // --------------
   // -- Question --
   // --------------
   Question::Question(Exchange* e) : QTextEdit(e) {
      setStyleSheet("* { border-style: solid hidden hidden solid; border-width: 1px; border-color:gray;}");
      setFont(e->font());
      setAcceptRichText(true);
      setViewportMargins(0, 0, 0, 0);
      setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
      setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      setLineWidth(1);
      cur_height = 0;
      exch = e;
      // open-axiom commands
      tmp.setFileTemplate("/tmp/axiomXXXXXX.input");
      if (not tmp.open()) QMessageBox::critical(0, "error", tmp.errorString());
      tmp.setAutoRemove(true);
      connect(this, SIGNAL(textChanged()), this, SLOT(dirtyText()));
      setContextMenuPolicy(Qt::CustomContextMenu);
      connect(this,SIGNAL(customContextMenuRequested(const QPoint&)),
              this,SLOT(showContextMenu(const QPoint &)));
   }

   void Question::showContextMenu(const QPoint &pt)
   {
       QMenu *menu = this->createStandardContextMenu();
       // Don' inherit colors
       menu->setStyleSheet("* { background-color: white; } QMenu::item:selected { background-color: rgb(40,130,220);  }");
       QAction *action;

       action = new QAction("Execute",this);
       action->setShortcut(QKeySequence(Qt::Key_Return + Qt::SHIFT));
       menu->addAction(action);
       connect(action,SIGNAL(triggered()),exchange(),SLOT(send_query()));

       action = new QAction("Comment",this);
       action->setShortcut(QKeySequence(Qt::Key_Return + Qt::CTRL));
       menu->addAction(action);
       connect(action,SIGNAL(triggered()),exchange(),SLOT(comment()));

       action = new QAction("Delete",this);
       action->setShortcut(QKeySequence(Qt::Key_Delete + Qt::ALT));
       menu->addAction(action);
       connect(action,SIGNAL(triggered()),exchange()->conversation(),SLOT(delete_topic()));

       action = new QAction("Insert",this);
       action->setShortcut(QKeySequence(Qt::Key_Insert + Qt::ALT));
       menu->addAction(action);
       connect(action,SIGNAL(triggered()),exchange()->conversation(),SLOT(insert_topic()));

       menu->exec(this->mapToGlobal(pt));
       delete menu;
   }

   // Amount of pixel spacing between the query and reply areas.
   const int spacing = 2;
   // Place the reply widget right below the frame containing
   // the query widget; make both of the same width, of course.
   void Exchange::prepare_reply_widget() {
       conversation()->adjustConversation(number());

      const QPoint pt = question()->geometry().bottomLeft();
      const int m = our_margin(answer());
      answer()->setGeometry(pt.x(), pt.y() + spacing,
             question()->width() - 2 * m, answer()->height());
   }

   void Question::dirtyText() {
       exchange()->setStyleSheet("* { background-color: rgb(255,240,240); }");
       exchange()->question()->setStyleSheet(
                   "* { border-style: solid hidden hidden solid; border-width: 1px; border-color:lightgray;}");
       if (document()->size().height()==cur_height) return;
       cur_height = document()->size().height();
       adjustSize(); updateGeometry();
       parentWidget()->adjustSize();
       exchange()->prepare_reply_widget();
   }

   QSize Question::sizeHint() const {
      return QSize(width(), document()->size().height());
   }

   void Question::focusInEvent(QFocusEvent* e) {
      exch->conversation()->set_topic(exch);
      QTextEdit::focusInEvent(e);
   }

   void Question::enterEvent(QEvent* e) {
      QTextEdit::enterEvent(e);
   }

   void Question::jump(int n) {
       Question* ques = exch->conversation()->nth_topic(exch->number()+n)->question();
       ques->setFocus();
       if (n>0) ques->moveCursor(QTextCursor::Start);
       if (n<0) ques->moveCursor(QTextCursor::End);
   }

   void Question::keyPressEvent ( QKeyEvent* event ) {
     if(event->key() == Qt::Key_Return) {
         if (event->modifiers()&Qt::ShiftModifier) {
             emit returnPressed();
         } else if (event->modifiers()&Qt::ControlModifier) {
             emit dontEvaluate();
         } else {
             QTextEdit::keyPressEvent( new QKeyEvent(QEvent::KeyPress, Qt::Key_Return,0) );
         }
         event->accept();
     }
     else if ((event->key() == Qt::Key_Up and event->modifiers()&Qt::AltModifier) or
              (event->key() == Qt::Key_Up and textCursor().atStart()) or
              (event->key() == Qt::Key_Left and textCursor().atStart()) )
         jump(-1);
     else if ((event->key() == Qt::Key_Down and event->modifiers()&Qt::AltModifier) or
              (event->key() == Qt::Key_Down and textCursor().atEnd()) or
              (event->key() == Qt::Key_Right and textCursor().atEnd()) )
         jump(+1);
     else if ((event->key() == Qt::Key_Delete and event->modifiers()&Qt::AltModifier))
         exchange()->conversation()->delete_topic();
     else if ((event->key() == Qt::Key_Insert and event->modifiers()&Qt::AltModifier))
         exchange()->conversation()->insert_topic();
     else QTextEdit::keyPressEvent( event );
   }

   // ------------
   // -- Answer --
   // ------------
   Answer::Answer(Exchange* e) : OutputTextArea(e) {
      setStyleSheet("* { border-style: hidden solid solid hidden; border-width: 1px; border-color:gray;}");
      hide();
   }

   // --------------
   // -- Exchange --
   // --------------
   // The layout within an exchange is as follows:
   //   -- input area (an editor) with its own decoration accounted for.
   //   -- an optional spacing
   //   -- an output area with its own decoration accounted for.
   QSize Exchange::sizeHint() const {
      const int m = our_margin(this);
      QSize sz = QSize(parentWidget()->width() - 2 * m, 2 * m+question()->height());
      if (not answer()->isHidden())
         sz.rheight() += answer()->document()->size().height() + spacing;
      return sz;
   }

   Exchange::Exchange(Conversation* conv, int n)
         : QFrame(conv), win(conv), no(n), query(this), reply(this) {
      setLineWidth(1);
      setFont(conv->font());
      move(conv->bottom_left());
      connect(&query, SIGNAL(returnPressed()), conv, SLOT(send_query()  ));
      connect(&query, SIGNAL(dontEvaluate() ), conv, SLOT(only_comment()));
   }

   void Exchange::resizeEvent(QResizeEvent* e) {
      QFrame::resizeEvent(e);
      const int w = width() - 2 * our_margin(this);
      question()->resize(w, question()->height());
      answer()->resize(w, answer()->height());
   }

   // ------------
   // -- Banner --
   // ------------
   Banner::Banner(Conversation* conv) :  Base(conv) {
      //setFrameStyle(StyledPanel | Raised);
      setFrameStyle(StyledPanel);
      setBackgroundRole(QPalette::Base);
   }
}
