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

#include <cmath>
#include <string>
#include <sstream>
#include <iostream>

#include <QMessageBox>
#include <QUuid>
#include <QUrl>
#include <QTextFrame>
#include <QTextBlockFormat>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>
#include <QApplication>
#include <QKeyEvent>
#include <QBuffer>
#include <QFileDialog>
#include <QMenu>
#include "conversation.h"
#include "debate.h"
#include "latexthread.h"

namespace OpenAxiom {
   // Largest width and line spacing, in pixel, of the font metrics
   // associated with `w'.
   static QSize font_units(const QWidget* w) {
      const QFontMetrics fm = w->fontMetrics();
      const auto h = fm.lineSpacing();
      if (auto w = fm.maxWidth())
         return { w, h };
      return { fm.width('W'), h };
   }

   // Return true if the QString `s' is morally an empty string.
   // QT makes a difference between a null string and an empty string.
   // That distinction is largely pedantic and without difference
   // for most of our practical purposes.
   static bool
   empty_string(const QString& s) {
      return s.isNull() or s.isEmpty();
   }
   
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
   static void
   prepare_reply_widget(Conversation* conv, Exchange* e) {
       conv->adjustConversation(e->number());

      Answer* a = e->answer();
      Question* q = e->question();
      const QPoint pt = e->question()->geometry().bottomLeft();
      const int m = our_margin(a);
      a->setGeometry(pt.x(), pt.y() + spacing,
             q->width() - 2 * m, a->height());
   }

   void Question::dirtyText() {
       exchange()->setStyleSheet("* { background-color: rgb(255,240,240); }");
       exchange()->question()->setStyleSheet(
                   "* { border-style: solid hidden hidden solid; border-width: 1px; border-color:lightgray;}");
       if (document()->size().height()==cur_height) return;
       cur_height = document()->size().height();
       adjustSize(); updateGeometry();
       parentWidget()->adjustSize();
       prepare_reply_widget(exchange()->conversation(), exchange());
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

   // Return a monospace font
   static QFont monospace_font() {
      QFont f("Monaco", 11);
      f.setStyleHint(QFont::TypeWriter);
      return f;
   }

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

   void Conversation::send_query() {
      QString input = exchange()->question()->toPlainText();
      if (empty_string(input))
         return;
      exchange()->question()->file()->resize(0);
      exchange()->question()->file()->write(input.toAscii());
      exchange()->question()->file()->flush();
      input = ")read " + exchange()->question()->file()->fileName() + " )quiet";
      exchange()->question()->clearFocus();
      prepare_reply_widget(this, cur_ex);
      debate->server()->input(input);
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   }

   static void ensure_visibility(Debate* debate, Exchange* e) {
      const int y = e->y() + e->height();
      QScrollBar* vbar = debate->verticalScrollBar();
      const int value = vbar->value();
      int new_value = y - vbar->pageStep();
      if (y < value)
         vbar->setValue(std::max(new_value, 0));
      else if (new_value > value)
         vbar->setValue(std::min(new_value, vbar->maximum()));
      e->question()->setFocus(Qt::OtherFocusReason);
   }

   void Conversation::only_comment() {
       cur_out->clear();
       cur_out->hide();
       cur_out->resize(
                   parentWidget()->width() - 2*our_margin(cur_out),
                   cur_out->document()->size().height());
       cur_out->resize(
                   cur_out->document()->size().width(),
                   cur_out->document()->size().height());
       cur_ex->setStyleSheet("* { background-color: transparent; }");
       exchange()->question()->setStyleSheet(
                   "* { border-style: none; border-width: 1px; border-color:lightgray; }");
       if (length() == 0) {
           ensure_visibility(debate, new_topic());
       }
       else {
           cur_ex->adjustSize();
           ensure_visibility(debate, next());
           QApplication::restoreOverrideCursor();
       }
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

   // ------------------
   // -- Conversation --
   // -------------------
   
   // Default number of characters per question line.
   const int columns = 80;
   const int lines = 40;
   // output tex
   auto tex = false;
   QString buf = "";
   QString texbuf = "";
   QString prompt = "";
   QString type = "";

   static QSize
   minimum_preferred_size(const Conversation* conv) {
      const QSize s = font_units(conv);
      return QSize(columns * s.width(), lines * s.height());
   }

   // Set a minimum preferred widget size, so no layout manager
   // messes with it.  Indicate we can make use of more space.

   Conversation::Conversation(Debate* d)
         : QWidget(d),
           debate(d),
           greetings(this),
           cur_ex(),
           cur_out(&greetings),
           rx("\\(\\d+\\)\\s->"),
           tx("\\sType: "),
           dd("^\\$\\$") {
      setFont(monospace_font());
      setBackgroundRole(QPalette::Base);
      greetings.setFont(font());
      setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
   }

   Conversation::~Conversation() {
      for (int i = children.size() -1 ; i >= 0; --i)
         delete children[i];
   }

   QPoint Conversation::bottom_left() const {
      if (length() == 0)
         return greetings.geometry().bottomLeft();
      return children.back()->geometry().bottomLeft();
   }

   static QSize round_up_height(const QSize& sz, int height) {
      if (height < 1)
         height = 1;
      const int n = (sz.height() + height) / height;
      return QSize(sz.width(), n * height);
   }
   
   QSize Conversation::sizeHint() const {
      const int n = length();
      if (n == 0)
         return minimum_preferred_size(this);
      const int view_height = debate->viewport()->height();
      QSize sz = greetings.size();
      for (int i = 0; i < n; ++i)
         sz.rheight() += children[i]->height();
      return round_up_height(sz, view_height);
   }

   void Conversation::resizeEvent(QResizeEvent* e) {
      QWidget::resizeEvent(e);
      setMinimumSize(size());
      const QSize sz = size();
      if (e->oldSize() == sz)
         return;
   }

   void Conversation::resize_me2(QSize sz) {
       greetings.resize(sz.width(), greetings.height());
       for (int i = 0; i < length(); ++i) {
          Exchange* e = children[i];
          e->resize(sz.width(), e->height());
       }
   }

   // set current topic
   void Conversation::set_topic(Exchange * exch) {
       cur_ex = exch;
       cur_out = cur_ex->answer();
   }

   Exchange* Conversation::new_topic() {
      Exchange* w = new Exchange(this, length() + 1);
      w->show();
      w->answer()->clear();
      children.push_back(w);
      adjustSize();
      updateGeometry();
      cur_out = w->answer();
      return cur_ex = w;
   }

   void Conversation::delete_topic() {
       // Can't delete last topic
       if (exchange()->number()==length()) return;
       auto n = exchange()->number()-1;  // from 0..length()-1
       delete children[n]; // de-allocate Exchange
       children.erase(children.begin()+n); // remove from list
       // renumber
       for(auto i=n;i<length();i++) children[i]->set_number(i+1);
       adjustConversation(n);
       cur_ex = children[n];
       cur_ex->question()->setFocus(Qt::OtherFocusReason);
   }

   void Conversation::insert_topic() {
       auto n = exchange()->number()-1;  // from 0..length()-1
       children.insert(children.begin()+n,new Exchange(this,n));
       // renumber
       for(auto i=n;i<length();i++) children[i]->set_number(i+1);
       adjustConversation(n);
       cur_ex = children[n];
       cur_ex->question()->setFocus(Qt::OtherFocusReason);
       cur_ex->show();
       cur_out = cur_ex->answer();
       cur_out->clear();
       adjustSize();
       updateGeometry();
   }

   Exchange* Conversation::nth_topic(int n) {
       if (n<1) return children[0];
       if (n>length()) return children[length()-1];
       return children[n-1];
   }

   // reposition all the older children
   void Conversation::adjustConversation(int n) {
       for (auto i=n;i<length();i++) {
           QPoint pt;
           if (i>0) pt = children[i-1]->geometry().bottomLeft();
           else     pt = greetings.geometry().bottomLeft();
           children[i]->setGeometry(pt.x(), pt.y(),
                                    children[i]->width(), children[i]->height());
       };
       adjustSize();
   }

   Exchange* Conversation::next() {
       Exchange* w = cur_ex;
       if (w == 0 or w->number() == length()) {
           cur_ex = new_topic();
       } else {
           cur_ex = children[w->number()];
           adjustConversation(w->number());
       }
       cur_ex->question()->setFocus(Qt::OtherFocusReason);
       return cur_ex;
   }

   static QTextCharFormat
   get_type_format(OutputTextArea* area) {
      auto format = area->get_cursor().charFormat();
      format.setFontWeight(QFont::Bold);
      format.setToolTip("domain of result");
      return format;
   }

   void Conversation::add_image(const QImage& s, OutputTextArea* area, int pos) {
       cur_out->add_image(area,pos,s);
   }

   void Conversation::read_reply() {
       auto data = debate->server()->readAll();
       buf.append(QString::fromLocal8Bit(data));
       if (rx.indexIn(buf) == -1) return;
       // Found prompt, start processing
       QStringList strs = buf.split('\n');
       buf = "";
       cur_out->clear();
       cur_out->resize(parentWidget()->width() - 2*our_margin(cur_out),cur_out->document()->size().height());
       for (auto& s : strs) {
           if (rx.indexIn(s) != -1) {
               prompt = s;
               continue;
           }
           auto dpos = dd.indexIn(s);
           if (dpos != -1) {
               if (tex) {
                   // Although single-threaded, getLatexFormula will yield to other QT signals
                   cur_out->align_left(".");
                   debate->latex()->process(texbuf,cur_out);
                   texbuf = "";
                   tex = false;
               }
               else {
                   tex = true;
               }
               continue;
           }
           auto tpos = tx.indexIn(s);
           if (tpos != -1) {
               type = s.mid(tpos+tx.matchedLength());
               cur_out->align_right(type, get_type_format(cur_out));
               continue;
           } else
               if (tex)
                   texbuf += s + "\n";
               else
                   cur_out->align_left(s);
           cur_out->update();
       }
       debate->latex()->wait();

       if (not empty_string(type)) {
           type = "";
       }
       // Make sure we're done
       QApplication::processEvents();
       cur_out->show();
       cur_out->resize(cur_out->document()->size().width(),cur_out->document()->size().height());
       if (length() == 0) {
           if (not empty_string(prompt)) {
               prompt = "";
               ensure_visibility(debate, new_topic());
           }
       }
       else {
           exchange()->setStyleSheet("* { background-color: transparent; }");
           exchange()->question()->setStyleSheet(
                       "* { border-style: solid hidden hidden solid; border-width: 1px; border-color:lightgray;}");
           exchange()->adjustSize();
           if (not empty_string(prompt)) {
               prompt = "";
               ensure_visibility(debate, next());
               QApplication::restoreOverrideCursor();
           }
           else {
               ensure_visibility(debate, exchange());
           }
       }
   }

   QString render_html(QTextDocument *doc) {
       QString html_data = doc->toHtml();
       // just the body
       QRegExp body("<body .*>.*(<.*)</body>");
       if (body.indexIn(doc->toHtml())==-1) QMessageBox::critical(0, "error", "no body in the library");
       html_data = body.cap(1);
       // embed images
       QTextBlock b = doc->begin();
       while (b.isValid()) {
           for (QTextBlock::iterator i = b.begin(); !i.atEnd(); ++i) {
               QTextCharFormat format = i.fragment().charFormat();
               bool isImage = format.isImageFormat();
               if (isImage) {
                   auto rn = format.toImageFormat().name();
                   auto im = doc->resource(QTextDocument::ImageResource, rn).value<QImage>();
                   QByteArray byteArray;
                   QBuffer buffer(&byteArray);
                   im.save(&buffer, "PNG"); // writes the image in PNG format inside the buffer
                   QString ims = QString::fromLatin1(byteArray.toBase64().data());
                   html_data.replace("<img src=\""+rn+"\" />",
                                     "<img src=\"data:image/png;base64,"+ims+"\" />");
               }
           }
           b = b.next();
       }
       return html_data;
   }

   void Conversation::open_file() {
       QString fn = QFileDialog::getOpenFileName(this,tr("Open Worksheet"),"",tr("open-axiom (*.oa);;html (*.html)"));
       if (fn.isEmpty()) return;
       QFile f( fn );
       if (f.open(QFile::ReadOnly)) {
           QString buf = f.readAll();
           QRegExp ques("<div class=\"(question|comment)\">\\n(<p .*>.*</p>)\\n</div>");
           ques.setMinimal(true);
           QRegExp ans("<div class=\"answer\">\\n(<p .*>.*</p>)\\n</div>");
           ans.setMinimal(true);
           int pos = 0;
           while ((pos = ques.indexIn(buf,pos)) != -1) {
               exchange()->question()->setHtml(ques.cap(2));
               pos += ques.matchedLength();
               if (ques.cap(1)=="question") {
               if ((pos = ans.indexIn(buf,pos)) != -1) {
                   cur_out->setHtml(ans.cap(1));
                   pos += ans.matchedLength();
                   cur_out->show();
                   cur_out->resize(cur_out->document()->size().width(),cur_out->document()->size().height());
               }
               exchange()->setStyleSheet("* { background-color: transparent; }");
               exchange()->question()->setStyleSheet(
                           "* { border-style: solid hidden hidden solid; border-width: 1px; border-color:lightgray;}");
               exchange()->adjustSize();
               ensure_visibility(debate, next());
               } else {
                   only_comment();
               }
           }
           f.close();
       } else
           QMessageBox::information(0, "error", f.errorString());
   }

   void Conversation::save_file() {
       QString fn = QFileDialog::getSaveFileName(this,tr("Save Worksheet"),"untitled.oa",tr("open-axiom (*.oa);;html (*.html)"));
       if (fn.isEmpty()) return;
       QFile f( fn );
       if (f.open(QFile::WriteOnly | QFile::Truncate)) {
           QTextDocument *doc;
           QTextStream out(&f);
           out << "<!DOCTYPE html>\n"
                  "<html>\n"
                  "<head><meta name=\"qrichtext\" content=\"1\" />\n"
                  "<style type=\"text/css\">\n"
                  "p, li { white-space: pre-wrap; }"
                  ".question { border-style: solid hidden hidden solid; border-width: thin;"
                              "padding:2px; padding-left:1em; margin-bottom:1em; }\n"
                  ".answer { border-style: hidden solid solid hidden; border-width: thin;"
                              "padding:2px; margin-bottom:1em; }\n"
                  ".comment { border-style: none; padding:2px; margin-bottom:1em; }\n"
                  ".banner { border-style: none; padding:2px; margin-bottom:1em;"
                            "font-family: monospace; text-align:center; }\n"
                  "</style>\n"
                  "</head>\n"
                  "<body style=\" font-family:'Monaco,monospace'; font-size:11pt; font-weight:400; font-style:normal;\">\n";
           out << "<div class=\"banner\">\n";
           doc = greetings.document();
           out << render_html(doc) << "\n";
           out << "</div>\n";
           for (auto i=0;i<length();i++) {
               if (children[i]->answer()->isHidden() ) {
                   out << "<div class=\"comment\">\n";
                   doc = children[i]->question()->document();
                   out << render_html(doc) << "\n";
                   out << "</div>\n";
               } else {
                   out << "<div class=\"question\">\n";
                   doc = children[i]->question()->document();
                   out << render_html(doc) << "\n";
                   out << "</div>\n";
                   out << "<div class=\"answer\">\n";
                   doc = children[i]->answer()->document();
                   out << render_html(doc) << "\n";
                   out << "</div>\n";
               }
           };
           out << "</body></html>\n";
           f.close();
       } else
           QMessageBox::information(0, "error", f.errorString());
   }

   void Conversation::read_file() {
       QString fn = QFileDialog::getOpenFileName(this,tr("Read Input"),"",tr("input (*.input)"));
       if (fn.isEmpty()) return;
       QFile f( fn );
       if (f.open(QFile::ReadOnly)) {
           exchange()->question()->setPlainText(f.readAll());
           f.close();
       } else
           QMessageBox::information(0, "error", f.errorString());
   }

   void Conversation::write_file() {
       QString fn = QFileDialog::getSaveFileName(this,tr("Write Input"),"untitled.input",tr("input (*.input)"));
       if (fn.isEmpty()) return;
       QFile f( fn );
       if (f.open(QFile::WriteOnly | QFile::Truncate)) {
           f.write(exchange()->question()->toPlainText().toAscii());
           f.close();
       } else
           QMessageBox::information(0, "error", f.errorString());
   }

   void Conversation::underline() {
       QTextCharFormat format;
       format.setFontUnderline(true);
       QTextCursor cursor = exchange()->question()->textCursor();
       if (!cursor.hasSelection())
          cursor.select(QTextCursor::WordUnderCursor);
       cursor.mergeCharFormat(format);
       exchange()->question()->mergeCurrentCharFormat(format);
   }
}
