#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject
{
 Q_OBJECT
 public:
  explicit FileDownloader(QUrl imageUrl, QObject *parent = 0);
  virtual ~FileDownloader();
  QByteArray downloadedData() const;
  QVariant mimeType() const;
  bool error() const;
  QString errorString() const;

 signals:
  void downloaded();
  void error(QString);

 private slots:
  void fileDownloaded(QNetworkReply* pReply);

 private:
  QNetworkAccessManager m_WebCtrl;
  QByteArray m_DownloadedData;
  bool m_error;
  QString m_errorString;
  QVariant m_MimeType;
};

#endif // FILEDOWNLOADER_H
