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

#ifndef OPENAXIOM_CONVERSATION_INCLUDED
#define OPENAXIOM_CONVERSATION_INCLUDED

#include <vector>
#include <queue>
#include <QFrame>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTemporaryFile>
#include <QTextEdit>
#include <QFont>
#include <QEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QRegExp>
#include "server.h"
#include "klfbackend.h"
#include "klfblockprocess.h"

namespace OpenAxiom {
   // A conversation is a set of exchanges.  An exchange is a question
   // followed by an answer.  A conversation that takes place in a
   // a certain frame is a debate.
   class Debate;
   class Conversation;
   class Exchange;
   class Question;
   class Answer;
   class OutputTextArea;

   class MainWindow;

   // --------------------
   // -- OutputTextArea --
   // --------------------
   // An output text area is a widget where we output text.
   // The texts are accumulated, as opposed to overwritten
   // including delayed subsitution of LaTeX generated images.

   class OutputTextArea : public QTextEdit {
      typedef QTextEdit Base;
   public:
      explicit OutputTextArea(QWidget*);
      // the metrics of this output area
      // QSize sizeHint() const;
      // Add a new paragraph to existing texts.  Paragraghs are
      // separated by the newline character.
      void align_left(const QString&);
      void align_right(const QString&, const QTextCharFormat);
      void add_image(OutputTextArea *where, int pos, const QImage& s);
      // Current cursor
      QTextCursor& get_cursor() { return cur; }
      QTextCursor& set_cursor(QTextCursor new_cur) { cur = new_cur; return cur; }
   protected:
      QTextCursor cur;
   };

   // ---------------
   // -- Question --
   // ---------------
   // A question is a multi-line query area.
   class Question : public QTextEdit {
       Q_OBJECT
   public:
      explicit Question(Exchange*);
      QSize sizeHint() const;
      QTemporaryFile* file() { return &tmp; }
      Exchange *exchange() { return exch; }
      // next(+1) or previous(-1) question
      void jump(int n);

   signals:
       void returnPressed();
       void dontEvaluate();

   private slots:
       void dirtyText();

   public slots:
       void showContextMenu(const QPoint &pt);

   protected:
      void enterEvent(QEvent*);
      void focusInEvent(QFocusEvent*);
      void keyPressEvent ( QKeyEvent * event );

   private:
      Exchange *exch;
      int cur_height;
      QTemporaryFile tmp;
   };

   // ------------
   // -- Answer --
   // ------------
   class Answer : public OutputTextArea {
      Q_OBJECT
   public:
      explicit Answer(Exchange*);
   };

   // --------------
   // -- Exchange --
   // --------------
   class Exchange : public QFrame {
      Q_OBJECT
   public:
      Exchange(Conversation*, int);

      // The widget holding the query area
      Question* question() { return &query; }
      const Question* question() const { return &query; }
      Server* server() const;

      // The widget holding the reply area.
      Answer* answer() { return &reply; }
      const Answer* answer() const { return &reply; }

      // Exchange number: children[i]->number()=i+1
      int number() const { return no; };
      // Exchange number: children[i]->set_number(i+1)
      int set_number(int n) { no = n; return no; };

      // Conversation
      Conversation * conversation() { return win; };

      // Reimplement position management.
      QSize sizeHint() const;

   protected:
      void resizeEvent(QResizeEvent*);
      Conversation* const win;

   private:
      int no;
      Question query;
      Answer reply;

   private slots:
      void send_query();
   };

   // Conversation banner, welcome greetings.
   class Banner : public OutputTextArea {
      typedef OutputTextArea Base;
   public:
      explicit Banner(Conversation*);
   };

   // -- Set of conversations that make up a session.
   // -- We remember and number each topic so that we
   // -- can go back in the conversation set and reevaluate
   // -- queries.
   class Conversation : public QWidget {
      Q_OBJECT;
   public:
      explicit Conversation(Debate*);
      ~Conversation();

      Debate* debate() const { return win; }

      // Holds if this conversation just started.
      bool fresh() const { return children.empty(); }

      // Number of exchanges in this conversation
      int length() const { return children.size(); }

      // Return the `i'-th conversation in this set, if any.
      Exchange* operator[](int) const;

      // Return the bottom left corner of the rectangle enclosing the
      // the set of exchanges in this conversation.
      QPoint bottom_left() const;
      
      // Existing topic
      Exchange* nth_topic(int n);

      // set current topic
      void set_topic(Exchange * exch);
      
      // Override QWidegt::sizeHint.  Return the cumulative sizes
      // of all conversations so far.
      QSize sizeHint() const;

      // Return a pointer to the current exchange, if any.
      Exchange* exchange() { return cur_ex; }

      // Start a new conversation topic.
      Exchange* new_topic();

      void process_reply(QByteArray data);
      void resize_me2(QSize sz);
      void adjustConversation(int n);

   public slots:
      // Return the topic following a given topic in this set of conversations
      Exchange* next();
      // delete one of the children
      void delete_topic();
      void insert_topic();
      // Target for dont_evaluate
      void comment();
      void add_image(const QImage& s, OutputTextArea *area, int pos);
      // worksheet files
      void open_file();
      void save_file();
      // input files
      void read_file();
      void write_file();
      void underline();

   protected:
      void resizeEvent(QResizeEvent*);
      //void paintEvent(QPaintEvent*);

   private slots:
      void read_reply();

   private:
      typedef std::vector<Exchange*> Children;
      Debate* const win;
      Banner greetings;
      Children children;
      Exchange* cur_ex;
      OutputTextArea* cur_out;
      QRegExp rx;
      QRegExp tx;
      QRegExp dd;

      KLFBackend::klfSettings settings;
      KLFBackend::klfInput input;
      KLFBackend::klfOutput output;
   };
}

#endif  // OPENAXIOM_CONVERSATION_INCLUDED


// Local Variables:
// mode: c++
// End:
