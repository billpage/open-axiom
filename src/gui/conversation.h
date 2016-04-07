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
#include <QComboBox>
#include <QFontComboBox>
#include "filedownloader.h"
#include "server.h"
#include "exchange.h"
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

   // -- Set of conversations that make up a session.
   // -- We remember and number each topic so that we
   // -- can go back in the conversation set and reevaluate
   // -- queries.
   class Conversation : public QWidget {
      Q_OBJECT;
   public:
      explicit Conversation(Debate*);
      ~Conversation();

      // Number of exchanges in this conversation
      int length() const { return children.size(); }
      // Return the `i'-th conversation in this set, if any.
      Exchange* operator[](int) const;
      // Return a pointer to the current exchange, if any.
      Exchange* exchange() { return cur_ex; }

      // Return the bottom left corner of the rectangle enclosing the
      // the set of exchanges in this conversation.
      QPoint bottom_left() const;
      
      // Existing topic
      Exchange* nth_topic(int n);
      // Start a new conversation topic.
      Exchange* new_topic();
      // set current topic
      void set_topic(Exchange * exch);
      
      // Override QWidegt::sizeHint.  Return the cumulative sizes
      // of all conversations so far.
      QSize sizeHint() const;

      void process_reply(QByteArray data);

      void resize_me2(QSize sz);
      void adjustConversation(int n);
      void ensure_visibility(Exchange* e);

      QAction *undo, *redo;
      QAction *bold, *italic, *underline;
      QAction *cut, *copy, *paste;
      QAction *insert_list, *insert_table;
      QAction* alignLeft;
      QAction* alignCenter;
      QAction* alignRight;
      QAction* alignJustify;
      QComboBox* fontSize;
      QComboBox* style;
      QFontComboBox* fontName;
      QTemporaryFile tmpSPAD;
      bool maybeSave();
      //void setCurrentFileName(const QString &fileN);

   public slots:
      // Return the topic following a given topic in this set of conversations
      Exchange* next();
      // delete one of the children
      void delete_topic();
      void insert_topic();
      void add_image(const QImage& s, OutputTextArea *area, int pos);
      // worksheet files
      void open_file();
      void save_file();
      // input files
      void read_file();
      void write_file();
      void download_html();

   private slots:
      void read_reply();
      void send_query();
      void execute_one();
      void execute_all();
      void only_comment();
      void load_html();

   protected:
      void resizeEvent(QResizeEvent*);

   private:
      typedef std::vector<Exchange*> Children;
      Debate* const debate;
      Banner greetings;
      Children children;
      Exchange* cur_ex;
      OutputTextArea* cur_out;
      QRegExp rx;
      QRegExp tx;
      QRegExp dd;

      FileDownloader* fetch;
      QString currentFileName;
   };
}

#endif  // OPENAXIOM_CONVERSATION_INCLUDED


// Local Variables:
// mode: c++
// End:
