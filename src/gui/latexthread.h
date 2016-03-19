#ifndef LATEXTHREAD_H
#define LATEXTHREAD_H

#include <QObject>
#include <QImage>
#include <QWaitCondition>
#include <QThread>
#include "klfbackend.h"
#include "klfblockprocess.h"
#include "conversation.h"

namespace OpenAxiom {

class LatexThread : public QThread {
    Q_OBJECT

public:
    LatexThread();
    ~LatexThread();
    void run();

public slots:
    void process(QString buf,  OutputTextArea *area);
    void wait();

signals:
    void got_image(QImage s, OutputTextArea *area, int pos);
    void error(const std::string& err);

private:
    // add your variables here
    KLFBackend::klfSettings settings;
    KLFBackend::klfInput input;
    KLFBackend::klfOutput output;
    QMutex mutex;
    QWaitCondition worker;
    QString texbuf;
    OutputTextArea* texarea;
    int texpos;
};

}

#endif // LATEXTHREAD_H
