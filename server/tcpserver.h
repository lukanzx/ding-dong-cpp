#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

class TcpServer : public QThread {
  Q_OBJECT
public:
  explicit TcpServer(QString filePath, int targetId, QString time,
                     QObject *parent = nullptr);

signals:

private:
  QTcpServer *tcpServer;
  QTcpSocket *tcpSocket;
};

#endif
