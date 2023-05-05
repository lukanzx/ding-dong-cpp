#include "qqclient.h"
#include <QQmlContext>
#include <QVariant>

QQClient::QQClient(QQmlApplicationEngine *engine, QObject *parent) {

  this->engine = engine;
  this->root = engine->rootObjects().first();

  quint16 listenPort = 9991;
  qDebug() << QString("客户端监听%1").arg(listenPort);

  udpSocket = new QUdpSocket(this);

  udpSocket->bind(QHostAddress::Any, listenPort);

  connect(udpSocket, &QUdpSocket::readyRead, this,
          &QQClient::on_udpSocket_readyRead);

  QObject::connect(root, SIGNAL(loginSignal(int, QString)), this,
                   SLOT(login(int, QString)));
  QObject *registerWindow = root->findChild<QObject *>("registerWindow");
  QObject::connect(registerWindow, SIGNAL(registerSignal(QString, QString)),
                   this, SLOT(registerAccount(QString, QString)));
  QObject *chatScreen = root->findChild<QObject *>("mainWindow")
                            ->findChild<QObject *>("chatWindow1")
                            ->findChild<QObject *>("chatWindow2")
                            ->findChild<QObject *>("chatScreen");
  QObject::connect(chatScreen, SIGNAL(sendMessageSignal(int, QString, int)),
                   this, SLOT(sendChatMessage(int, QString, int)));
  QObject *addFriendWindow = root->findChild<QObject *>("mainWindow")
                                 ->findChild<QObject *>("chatWindow1")
                                 ->findChild<QObject *>("contactScreen")
                                 ->findChild<QObject *>("addFriendWindow");
  QObject::connect(addFriendWindow, SIGNAL(searchSignal(int)), this,
                   SLOT(search(int)));
  QObject::connect(addFriendWindow, SIGNAL(addSignal(int)), this,
                   SLOT(add(int)));
  QObject *contactScreen = root->findChild<QObject *>("mainWindow")
                               ->findChild<QObject *>("chatWindow1")
                               ->findChild<QObject *>("contactScreen");
  QObject::connect(contactScreen, SIGNAL(createGroupSignal(QVariant)), this,
                   SLOT(createGroup(QVariant)));
  QObject::connect(contactScreen, SIGNAL(requestContactSignal()), this,
                   SLOT(refreshContactFriend()));
  QObject::connect(contactScreen, SIGNAL(requestGroupSignal()), this,
                   SLOT(refreshContactGroup()));

  QObject *historyMessageScreen =
      root->findChild<QObject *>("mainWindow")
          ->findChild<QObject *>("chatWindow1")
          ->findChild<QObject *>("chatWindow2")
          ->findChild<QObject *>("historyMessageScreen");
  QObject::connect(historyMessageScreen, SIGNAL(refreshChatListSignal(int)),
                   SLOT(messageRequest(int)));

  QObject::connect(contactScreen, SIGNAL(infSignal(int, bool)), this,
                   SLOT(infoRequest(int, bool)));

  QObject::connect(chatScreen, SIGNAL(getPInfSignal(int, bool)), this,
                   SLOT(infoRequest(int, bool)));
}

QQClient::~QQClient() {}

void QQClient::on_udpSocket_readyRead() {

  char buf[4096] = {0};
  QHostAddress ip;
  quint16 port;
  QJsonParseError parseErr;
  QJsonObject recObj;
  QString result;

  qint64 len = udpSocket->readDatagram(buf, sizeof(buf), &ip, &port);

  if (len > 0) {

    QString content =
        QString("[%1,%2]：%3").arg(ip.toString()).arg(port).arg(buf);
    qDebug() << content << '\n';

    parseCommand(buf, ip, port);
  } else {
    QString content = QString("收取失败");
    qDebug() << content << '\n';
  }
}

void QQClient::sendMessage(QString content, QString ip, QString port) {
  udpSocket->writeDatagram(content.toUtf8().data(), QHostAddress(ip),
                           port.toUInt());
}

void QQClient::sendMessage(QString content, QHostAddress ip, quint16 port) {
  udpSocket->writeDatagram(content.toUtf8().data(), ip, port);
}

void QQClient::sendMessage(QByteArray content, QHostAddress ip, quint16 port) {
  udpSocket->writeDatagram(content, ip, port);
}

void QQClient::sendMessage(QByteArray content, QString ip, QString port) {
  udpSocket->writeDatagram(content, QHostAddress(ip), port.toUInt());
}

void QQClient::parseCommand(QString jsonStr, QHostAddress ip, quint16 port) {

  QJsonParseError error;
  QByteArray jsonByteArray = jsonStr.toUtf8();
  QJsonObject obj = QJsonDocument::fromJson(jsonByteArray, &error).object();

  QString command = obj["command"].toString();

  if (command == "registerBack") {
    registerBack(obj);
  } else if (command == "loginBack") {
    loginBack(obj);
  } else if (command == "sendChatMessageBack") {
    sendChatMessageBack(obj);
  } else if (command == "searchBack") {
    searchBack(obj);
  } else if (command == "addBack") {
    addBack(obj);
  } else if (command == "createGroupBack") {
    createGroupBack(obj);
  } else if (command == "deleteBack") {
    deleteBack(obj);
  } else if (command == "friendBack") {
    friendBack(obj);
  } else if (command == "groupBack") {
    groupBack(obj);
  } else if (command == "messageBack") {
    messageBack(obj);
  } else if (command == "personInfoBack") {
    personInfoBack(obj);
  } else if (command == "groupInfoBack") {
    groupInfoBack(obj);
  } else {
    qDebug() << "未知命令";
  }
}

void QQClient::registerAccount(QString user, QString password) {

  QJsonObject jsonObj;
  jsonObj.insert("command", "register");
  jsonObj.insert("user", user);
  jsonObj.insert("password", password);
  QString content = QJsonDocument(jsonObj).toJson();

  sendMessage(content, this->hostIp, this->hostPort);
}

void QQClient::login(int id, QString password) {

  QJsonObject jsonObj;
  jsonObj.insert("command", "login");
  jsonObj.insert("id", id);
  jsonObj.insert("password", password);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);

  clientId = id;
}

void QQClient::sendChatMessage(int targetId, QString content, int time) {

  QJsonObject jsonObj;
  jsonObj.insert("command", "sendChatMessage");
  jsonObj.insert("sendId", clientId);
  jsonObj.insert("targetId", targetId);
  jsonObj.insert("content", content);
  jsonObj.insert("time", time);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);
}

void QQClient::search(int targetId) {

  QJsonObject jsonObj;
  jsonObj.insert("command", "search");
  jsonObj.insert("targetId", targetId);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);
}

void QQClient::add(int targetId) {

  QJsonObject jsonObj;
  jsonObj.insert("command", "add");
  jsonObj.insert("sendId", clientId);
  jsonObj.insert("targetId", targetId);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);
}

void QQClient::createGroup(QVariant var) {
  qDebug() << "createGroup";

  QJsonObject obj;
  obj.insert("command", "createGroup");
  obj.insert("id", clientId);
  QString diagram = QJsonDocument(obj).toJson();
  sendMessage(diagram, this->hostIp, this->hostPort);
}

void QQClient::deleteRequest(int targetId) {

  QJsonObject jsonObj;
  jsonObj.insert("command", "delete");
  jsonObj.insert("targetId", targetId);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);
}

void QQClient::friendRequest(int id) {

  QJsonObject jsonObj;
  jsonObj.insert("command", "friendRequest");
  jsonObj.insert("id", id);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);
}

void QQClient::messageRequest(int id) {

  QJsonObject jsonObj;
  jsonObj.insert("command", "messageRequest");
  jsonObj.insert("requestId", clientId);
  jsonObj.insert("targetId", id);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);
}

void QQClient::groupRequest(int id) {

  QJsonObject jsonObj;
  jsonObj.insert("command", "groupRequest");
  jsonObj.insert("id", id);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);
}
void QQClient::infoRequest(int id, bool isGroup) {
  qDebug() << "infoRequest()";
  if (isGroup == true) {
    groupInfoRequest(id);
  } else {
    personInfoRequest(id);
  }
}

void QQClient::personInfoRequest(int id) {

  qDebug() << "personInfoRequest()";
  QJsonObject jsonObj;
  jsonObj.insert("command", "personInfoRequest");
  jsonObj.insert("targetId", id);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);
}

void QQClient::groupInfoRequest(int id) {

  qDebug() << "groupInfoRequest()";
  QJsonObject jsonObj;
  jsonObj.insert("command", "groupInfoRequest");
  jsonObj.insert("clientId", clientId);
  jsonObj.insert("targetId", id);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);
}

void QQClient::changePInfoRequest(QString name, QString saying, int id,
                                  QString personalHead, int sexn, QString area,
                                  QString edul) {

  qDebug() << "changePInfoRequest()";
  QJsonObject jsonObj;
  jsonObj.insert("command", "changePInfoRequest");
  jsonObj.insert("id", id);
  jsonObj.insert("name", name);
  jsonObj.insert("icon", personalHead);
  jsonObj.insert("gender", sexn);
  jsonObj.insert("area", area);
  jsonObj.insert("edu", edul);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);
}

void QQClient::changeGInfoRequest(QString groupName, QString groupNotice,
                                  QString groupSummary, int groupId) {

  qDebug() << "changeGInfoRequest()";
  QJsonObject jsonObj;
  jsonObj.insert("command", "changeGInfoRequest");
  jsonObj.insert("id", groupId);
  jsonObj.insert("name", groupName);
  jsonObj.insert("notice", groupNotice);
  jsonObj.insert("summary", groupSummary);
  QString diagram = QJsonDocument(jsonObj).toJson();

  sendMessage(diagram, this->hostIp, this->hostPort);
}

void QQClient::test(int i) { qDebug() << "test()"; }

void QQClient::registerBack(QJsonObject obj) {

  int id = obj["id"].toInt();
  qDebug() << "register";

  QVariant res;
  QObject *registerWindow = root->findChild<QObject *>("registerWindow");
  QMetaObject::invokeMethod(registerWindow, "registerBack",
                            Q_RETURN_ARG(QVariant, res), Q_ARG(QVariant, id));
}

void QQClient::loginBack(QJsonObject obj) {

  bool isSuccess = obj["result"].toBool();
  qDebug() << "loginBack::data" << obj;

  if (!isSuccess) {
    clientId = 0;
  } else {
    friendRequest(clientId);
    groupRequest(clientId);
    messageRequest(clientId);

    /*
    messageRequest(100002);
    messageRequest(600001);
    */
  }

  QVariant res;
  QMetaObject::invokeMethod(root, "loginBack", Q_RETURN_ARG(QVariant, res),
                            Q_ARG(QVariant, isSuccess));
}

void QQClient::sendChatMessageBack(QJsonObject obj) {

  int sendId = obj["sendId"].toInt();
  QString content = obj["content"].toString();
  int time = obj["time"].toInt();

  QJsonObject jsonObj;
  jsonObj.insert("uid", sendId);
  jsonObj.insert("groupId", obj["groupId"].toInt());

  for (int i = 0; i < friendList.length(); i++) {
    if (friendList[i]->id == sendId) {
      jsonObj.insert("name", friendList[i]->name);
      jsonObj.insert("avatar", friendList[i]->icon);
    }
  }
  jsonObj.insert("time", time);
  jsonObj.insert("message", content);

  jsonObj.insert("type", 0);

  QVariant res;
  QObject *chatScreen = root->findChild<QObject *>("mainWindow")
                            ->findChild<QObject *>("chatWindow1")
                            ->findChild<QObject *>("chatWindow2")
                            ->findChild<QObject *>("chatScreen");
  QMetaObject::invokeMethod(chatScreen, "appendData",
                            Q_RETURN_ARG(QVariant, res),
                            Q_ARG(QVariant, jsonObj));
}

void QQClient::searchBack(QJsonObject obj) {

  QVariant res;
  QObject *addFriendWindow = root->findChild<QObject *>("mainWindow")
                                 ->findChild<QObject *>("chatWindow1")
                                 ->findChild<QObject *>("contactScreen")
                                 ->findChild<QObject *>("addFriendWindow");
  QMetaObject::invokeMethod(addFriendWindow, "searchBack",
                            Q_RETURN_ARG(QVariant, res), Q_ARG(QVariant, obj));
}

void QQClient::addBack(QJsonObject obj) {

  int friendId = obj["friendId"].toInt();

  QVariant res;
  QObject *addFriendWindow = root->findChild<QObject *>("mainWindow")
                                 ->findChild<QObject *>("chatWindow1")
                                 ->findChild<QObject *>("contactScreen")
                                 ->findChild<QObject *>("addFriendWindow");
  QMetaObject::invokeMethod(addFriendWindow, "addBack",
                            Q_RETURN_ARG(QVariant, res),
                            Q_ARG(QVariant, friendId));
}

void QQClient::createGroupBack(QJsonObject obj) {

  int groupId = obj["groupId"].toInt();

  QVariant res;
  QObject *contactScreen = root->findChild<QObject *>("mainWindow")
                               ->findChild<QObject *>("chatWindow1")
                               ->findChild<QObject *>("contactScreen");
  qDebug() << "creageGroupBack";
  QMetaObject::invokeMethod(contactScreen, "createGroupIsSuccess",
                            Q_RETURN_ARG(QVariant, res),
                            Q_ARG(QVariant, groupId));
}

void QQClient::deleteBack(QJsonObject obj) {
  qDebug() << "deleteBack()" << obj;
}

void QQClient::friendBack(QJsonObject obj) {

  QJsonArray friendJsonList = obj["list"].toArray();

  friendList.clear();

  QJsonObject clientObj = friendJsonList[0].toObject();

  QQmlContext *context = engine->rootContext();
  ClientInfo *info = new ClientInfo(clientObj);
  clientName = clientObj["name"].toString();
  clientAvatar = clientObj["icon"].toString();
  context->setContextProperty("ClientInfo", info);

  for (int i = 1; i < friendJsonList.size(); i++) {

    QJsonObject user = friendJsonList[i].toObject();

    this->friendList.append(new User(
        user["id"].toInt(), user["name"].toString(), user["icon"].toString()));
  }
}

void QQClient::messageBack(QJsonObject obj) {

  int targetId = obj["targetId"].toInt();
  QJsonArray packageList;
  if (targetId / 100000 == 1) {
    QJsonArray messageList = obj["messagelist"].toArray();
    for (int i = 0; i < messageList.size(); i++) {
      QJsonObject item = messageList[i].toObject();

      int sendId = item["sid"].toInt();
      QString content = item["message"].toString();
      int time = item["datetime"].toInt();

      QJsonObject jsonObj;
      jsonObj.insert("uid", sendId);
      jsonObj.insert("groupId", 0);

      if (sendId == clientId) {
        jsonObj.insert("name", clientName);
        jsonObj.insert("avatar", clientAvatar);

      } else {
        for (int i = 0; i < friendList.length(); i++) {
          qDebug() << friendList[i]->id;
          if (friendList[i]->id == sendId) {

            jsonObj.insert("name", friendList[i]->name);
            jsonObj.insert("avatar", friendList[i]->icon);
          }
        }
      }

      jsonObj.insert("time", time);
      jsonObj.insert("message", content);
      jsonObj.insert("type", 0);
      packageList.append(jsonObj);
    }
  } else {
    QJsonArray messageList = obj["grouplist"].toArray();
    for (int i = 0; i < messageList.size(); i++) {
      QJsonObject item = messageList[i].toObject();

      qDebug() << item["datetime"].toInt() << item["message"].toString()
               << item["mid"].toInt();

      int sendId = item["mid"].toInt();
      QString content = item["message"].toString();
      int time = item["datetime"].toInt();

      QJsonObject jsonObj;
      jsonObj.insert("uid", sendId);
      jsonObj.insert("groupId", targetId);

      if (sendId == clientId) {
        jsonObj.insert("name", clientName);
        jsonObj.insert("avatar", clientAvatar);

      } else {
        for (int i = 0; i < friendList.length(); i++) {
          qDebug() << friendList[i]->id;
          if (friendList[i]->id == sendId) {

            jsonObj.insert("name", friendList[i]->name);
            jsonObj.insert("avatar", friendList[i]->icon);
          }
        }
      }
      jsonObj.insert("time", time);
      jsonObj.insert("message", content);
      jsonObj.insert("type", 0);
      packageList.append(jsonObj);
    }
  }

  QObject *chatScreen = root->findChild<QObject *>("mainWindow")
                            ->findChild<QObject *>("chatWindow1")
                            ->findChild<QObject *>("chatWindow2")
                            ->findChild<QObject *>("chatScreen");
  QVariant res;
  QMetaObject::invokeMethod(chatScreen, "setMessages",
                            Q_RETURN_ARG(QVariant, res),
                            Q_ARG(QVariant, packageList));
}

void QQClient::groupBack(QJsonObject obj) {

  QJsonArray groupJsonList = obj["groupList"].toArray();

  groupList.clear();

  for (int i = 0; i < groupJsonList.size(); i++) {

    QJsonObject group = groupJsonList[i].toObject();

    this->groupList.append(new Group(
        group["id"].toInt(), group["name"].toString(), group["icon"].toString(),
        group["intro"].toString(), group["notice"].toString()));
  }
  for (int i = 0; i < groupList.length(); i++) {
    qDebug() << groupList[i]->name;
  }
}
void QQClient::personInfoBack(QJsonObject obj) {

  qDebug() << obj;
  QVariant res;
  QObject *personInfoWindow = root->findChild<QObject *>("mainWindow")
                                  ->findChild<QObject *>("chatWindow1")
                                  ->findChild<QObject *>("contactScreen");
  QMetaObject::invokeMethod(personInfoWindow, "personalInfBack",
                            Q_RETURN_ARG(QVariant, res),
                            Q_ARG(QVariant, obj["result"].toObject()));
}
void QQClient::groupInfoBack(QJsonObject obj) {

  QVariant res;
  QObject *groupInfoWindow = root->findChild<QObject *>("mainWindow")
                                 ->findChild<QObject *>("chatWindow1")
                                 ->findChild<QObject *>("contactScreen");
  QMetaObject::invokeMethod(groupInfoWindow, "groupInfBack",
                            Q_RETURN_ARG(QVariant, res),
                            Q_ARG(QVariant, obj["result"].toObject()));
}

void QQClient::refreshContactFriend() {

  friendRequest(clientId);

  QJsonArray jsonArray;
  for (int i = 0; i < friendList.length(); i++) {
    QJsonObject friendObj;
    friendObj.insert("userId", friendList[i]->id);
    friendObj.insert("userName", friendList[i]->name);
    friendObj.insert("avatar", friendList[i]->icon);
    jsonArray.append(friendObj);
  }

  QVariant res;
  QObject *contactScreen = root->findChild<QObject *>("mainWindow")
                               ->findChild<QObject *>("chatWindow1")
                               ->findChild<QObject *>("contactScreen");
  QMetaObject::invokeMethod(contactScreen, "setContacts",
                            Q_RETURN_ARG(QVariant, res),
                            Q_ARG(QVariant, jsonArray));
}

void QQClient::refreshContactGroup() {

  groupRequest(clientId);

  QJsonArray jsonArray;
  for (int i = 0; i < groupList.length(); i++) {
    QJsonObject groupObj;
    groupObj.insert("userId", groupList[i]->id);
    groupObj.insert("userName", groupList[i]->name);
    groupObj.insert("avatar", groupList[i]->icon);
    jsonArray.append(groupObj);
  }

  QVariant res;
  QObject *contactScreen = root->findChild<QObject *>("mainWindow")
                               ->findChild<QObject *>("chatWindow1")
                               ->findChild<QObject *>("contactScreen");
  QMetaObject::invokeMethod(contactScreen, "setGroups",
                            Q_RETURN_ARG(QVariant, res),
                            Q_ARG(QVariant, jsonArray));
}
