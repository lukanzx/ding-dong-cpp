#ifndef QQSERVER_H
#define QQSERVER_H

#include "onlinelist.h"
#include "sqlaccountmodel.h"
#include "sqlfriendmodel.h"
#include "sqlgroupmodel.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QMainWindow>
#include <QUdpSocket>
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui {
class QQServer;
}
QT_END_NAMESPACE

class User {
public:
  User(QHostAddress ip, quint16 port, int id) {
    this->id = id;
    this->ip = ip;
    this->port = port;
  }
  ~User();

  int id;
  QHostAddress ip;
  quint16 port;
};

class Group {
public:
  Group(int gId, SqlGroupModel *gpModel) {
    QJsonObject obj =
        QJsonDocument::fromJson(gpModel->memberList(gId)).object();
    QJsonArray list = obj["list"].toArray();
    for (int i = 0; i < list.size(); i++) {
      memberList.append(list[i].toObject()["id"].toInt());
    }
    for (int i = 0; i < memberList.length(); i++) {
      qDebug() << memberList[i];
    }
  }

  ~Group() { memberList.clear(); }

  QList<int> memberList;
};

class QQServer : public QMainWindow {
  Q_OBJECT

public:
  QQServer(QWidget *parent = nullptr);
  ~QQServer();

private slots:

  void onUdpSocketReadyRead();

private:
  Ui::QQServer *ui;

  QUdpSocket *udpSocket = NULL;

  QList<User *> onlineUser;
  QList<Group *> groupList;
  QJsonObject getTargetIpPort(int targetId);

  SqlAccountModel *atModel;
  SqlFriendModel *fdModel;
  SqlGroupModel *gpModel;

  void parseCommand(QString jsonStr, QHostAddress ip, quint16 port);

  void sendMessage(QString content, QString ip, QString port);
  void sendMessage(QString content, QHostAddress ip, quint16 port);
  void sendMessage(QByteArray content, QHostAddress ip, quint16 port);
  void sendMessage(QByteArray content, QString ip, QString port);

  void registerRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void loginRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void sendChatMessageRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void searchRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void addRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void createGroupRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void deleteRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void friendRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void groupRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void messageRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void getPersonalInfoRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void getGroupInfoRespond(QJsonObject obj, QHostAddress ip, quint16 port);
  void changePInfo(QJsonObject obj, QHostAddress ip, quint16 port);
  void changeGInfo(QJsonObject obj, QHostAddress ip, quint16 port);

  void test(SqlAccountModel *atModel, SqlFriendModel *fdModel,
            SqlGroupModel *gpModel);
};
#endif
