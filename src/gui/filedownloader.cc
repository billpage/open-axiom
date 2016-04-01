#include <QDebug>
#include "filedownloader.h"

FileDownloader::FileDownloader(QUrl imageUrl, QObject *parent) :
 QObject(parent)
{
 connect(
  &m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
  this, SLOT (fileDownloaded(QNetworkReply*))
  );
  m_error = false;
  m_errorString = "";

 QNetworkRequest request(imageUrl);
 m_WebCtrl.get(request);
}

FileDownloader::~FileDownloader() { }

void FileDownloader::fileDownloaded(QNetworkReply* pReply) {
    if (pReply->error()) {
        m_error = true;
        m_errorString = pReply->errorString();
        emit error(pReply->errorString());
    }
    m_DownloadedData = pReply->readAll();
    m_MimeType = pReply->header(QNetworkRequest::ContentTypeHeader);
    pReply->deleteLater();
    emit downloaded();
}

bool FileDownloader::error() const {
    return m_error;
}

QString FileDownloader::errorString() const {
    return m_errorString;
}

QVariant FileDownloader::mimeType() const {
    return m_MimeType;
}
QByteArray FileDownloader::downloadedData() const {
 return m_DownloadedData;
}
