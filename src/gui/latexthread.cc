#include "latexthread.h"
#include "conversation.h"
#include <QMessageBox>

namespace OpenAxiom {
LatexThread::LatexThread() {
    if(!KLFBackend::detectSettings(&settings))
        QMessageBox::critical(0, "error", "unable to find LaTeX in default directories.");
    input.mathmode = "\\[ ... \\]";
    input.dpi = 150;
    input.preamble = QString("\\usepackage{amssymb,amsmath,mathrsfs}\
                              \\newcommand{\\zag}[2]{\\begin{array}{c}\\multicolumn{1}{c|}{#1}\\\\\\hline \
                              \\multicolumn{1}{|c}{#2}\\end{array}}");
    // Not sure why the following is necessary when running outside of Qt Creator.
    settings.execenv = {"TEXINPUTS=/usr/share/texmf//:"};
    settings.epstopdfexec = ""; // no pdf
    settings.outlineFonts = false;
}

LatexThread::~LatexThread() {
    // free resources
}

void LatexThread::run() {
    forever {
        mutex.lock();
        //qDebug("Latex waiting");
        worker.wait(&mutex);
        //qDebug("Latex busy");
        input.latex = texbuf;
        output = KLFBackend::getLatexFormula(input, settings);
        if (output.status != 0) {
            emit error(output.errorstr);
        }
        else {
            //qDebug() << "Image ready" << texpos;
            QImage img = output.result.convertToFormat(QImage::Format_ARGB32,Qt::DiffuseAlphaDither);
            for (int i = 0; i < img.height(); i++)
                for (int j = 0; j < img.width(); j++)
                    if(img.pixel(j, i) == qRgba(255,255,255,255))
                        img.setPixel(j, i, Qt::transparent);
            emit got_image(img,texarea,texpos);
        }
        mutex.unlock();
    }
}

void LatexThread::process(QString buf, OutputTextArea* area) {
    //qDebug() << "Work waiting" << area->get_cursor().position();
    mutex.lock();
    //qDebug() << "Work ready" << area->get_cursor().position();
    texbuf = buf;
    texarea = area;
    texpos = area->get_cursor().position();
    worker.wakeOne();
    mutex.unlock();
}

void LatexThread::wait() {
    //qDebug() << "Finish waiting";
    mutex.lock();
    mutex.unlock();
}
}
