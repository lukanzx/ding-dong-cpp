#ifndef QQCLIENT_H
#define QQCLIENT_H
#include <QDateTime>
#include <QHostAddress>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QUdpSocket>
#include <iostream>

class ClientInfo : public QObject {
  Q_OBJECT
  Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString avatar READ avatar WRITE setAvatar NOTIFY avatarChanged)

public:
  ClientInfo() {}
  ClientInfo(QJsonObject infoObj) { parse(infoObj); }

  int mId = 0;
  QString mName = "默认昵称";
  QString mAvatar = "https://www.com8.cn/wp-content/1246.jpg";

  int id() { return mId; };
  void setId(int id) {
    if (id != mId) {
      mId = id;
      emit idChanged();
    }
  }

  QString name() { return mName; }
  void setName(QString name) {
    if (name != mName) {
      mName = name;
      emit nameChanged();
    }
  }

  QString avatar() { return mAvatar; }
  void setAvatar(QString avatar) {
    if (avatar != mAvatar) {
      mAvatar = avatar;
      emit avatarChanged();
    }
  }

  void parse(QJsonObject infoObj) {
    mId = infoObj["id"].toInt();
    mName = infoObj["name"].toString();
    mAvatar = infoObj["icon"].toString();
    qDebug() << "ClientInfo" << mId << mName << mAvatar;
  }

signals:
  void idChanged();
  void nameChanged();
  void avatarChanged();
};

class User {
public:
  User(int id, QString name, QString icon) {
    this->id = id;
    this->name = name;
    this->icon = icon;
    qDebug() << "构建User:" << id << name;
  }

  int id;
  QString name;
  QString icon;
};

class Group {
public:
  Group(int id, QString name, QString icon, QString intro, QString notice) {
    this->id = id;
    this->name = name;
    this->icon = icon;
    this->intro = intro;
    this->notice = notice;
    qDebug() << "构建Group:" << id << name;
  }
  int id;
  QString name;
  QString icon;
  QString intro;
  QString notice;
};

class QQClient : public QObject {
  Q_OBJECT
public:
  QQClient(QQmlApplicationEngine *engine, QObject *parent = nullptr);
  ~QQClient();
  Q_INVOKABLE void test(int i);
  Q_INVOKABLE void changePInfoRequest(QString, QString, int, QString, int,
                                      QString, QString);
  Q_INVOKABLE void changeGInfoRequest(QString groupName, QString groupNotice,
                                      QString groupSummary, int groupId);

private slots:

  void registerAccount(QString user, QString password);
  void login(int id, QString password);
  void sendChatMessage(int targetId, QString content, int time);
  void search(int targetId);
  void add(int targetId);
  void createGroup(QVariant memberList);
  void deleteRequest(int targetId);
  void friendRequest(int id);
  void messageRequest(int id);
  void groupRequest(int id);
  void infoRequest(int id, bool isGroup);
  void personInfoRequest(int id);
  void groupInfoRequest(int id);

  void refreshContactFriend();
  void refreshContactGroup();

private:
  QUdpSocket *udpSocket;
  QHostAddress hostIp = QHostAddress("81.68.194.103");
  quint16 hostPort = 8080;

  int clientId = 0;
  ClientInfo *clientInfo;
  QString clientName;
  QString clientAvatar;
  QList<User *> friendList;
  QList<Group *> groupList;

  QQmlApplicationEngine *engine = NULL;
  QObject *root = NULL;

  void on_udpSocket_readyRead();
  void sendMessage(QString content, QString ip, QString port);
  void sendMessage(QString content, QHostAddress ip, quint16 port);
  void sendMessage(QByteArray content, QHostAddress ip, quint16 port);
  void sendMessage(QByteArray content, QString ip, QString port);

  void parseCommand(QString jsonStr, QHostAddress ip, quint16 port);

  void registerBack(QJsonObject obj);
  void loginBack(QJsonObject obj);
  void sendChatMessageBack(QJsonObject obj);
  void searchBack(QJsonObject obj);
  void addBack(QJsonObject obj);
  void createGroupBack(QJsonObject obj);
  void deleteBack(QJsonObject obj);
  void friendBack(QJsonObject obj);
  void messageBack(QJsonObject obj);
  void groupBack(QJsonObject obj);
  void personInfoBack(QJsonObject obj);
  void groupInfoBack(QJsonObject obj);
};

#endif
