#include "qqserver.h"
#include "onlinelist.h"
#include "ui_qqserver.h"

QQServer::QQServer(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::QQServer) {
  ui->setupUi(this);

  quint16 listenPort = 9990;
  this->setWindowTitle(QString("服务端监听%1").arg(listenPort));

  udpSocket = new QUdpSocket(this);

  udpSocket->bind(QHostAddress::Any, listenPort);

  connect(udpSocket, &QUdpSocket::readyRead, this,
          &QQServer::onUdpSocketReadyRead);

  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName("Info.db");
  db.open();
  atModel = new SqlAccountModel(this, db);
  fdModel = new SqlFriendModel(this, db);
  gpModel = new SqlGroupModel(this, db);

  test(atModel, fdModel, gpModel);
}

QQServer::~QQServer() {
  delete ui;
  delete fdModel;
  delete atModel;
  delete gpModel;
}

void QQServer::onUdpSocketReadyRead() {

  char buf[4096] = {0};
  QHostAddress ip;
  quint16 port;

  qint64 len = udpSocket->readDatagram(buf, sizeof(buf), &ip, &port);

  if (len > 0) {

    QString content =
        QString("[%1,%2]：\n%3").arg(ip.toString()).arg(port).arg(buf);
    ui->textBrowser->append(content);
    qDebug() << "收到命令：" << content << '\n';

    parseCommand(buf, ip, port);
  } else {
    QString content = QString("收取失败");
    qDebug() << content << '\n';
  }
}

QJsonObject QQServer::getTargetIpPort(int targetId) {
  for (int i = 0; i < onlineUser.length(); i++) {
    if (targetId == onlineUser[i]->id) {
      QJsonObject targetObj;
      targetObj.insert("ip", onlineUser[i]->ip.toString());
      targetObj.insert("port", onlineUser[i]->port);
      return targetObj;
    }
  }
  QJsonObject targetObj;
  targetObj.insert("ip", "0");
  targetObj.insert("port", 0);
  return targetObj;
}

void QQServer::sendMessage(QString content, QString ip, QString port) {
  udpSocket->writeDatagram(content.toUtf8().data(), QHostAddress(ip),
                           port.toUInt());
}

void QQServer::sendMessage(QString content, QHostAddress ip, quint16 port) {
  udpSocket->writeDatagram(content.toUtf8().data(), ip, port);
}

void QQServer::sendMessage(QByteArray content, QHostAddress ip, quint16 port) {
  udpSocket->writeDatagram(content, ip, port);
}

void QQServer::sendMessage(QByteArray content, QString ip, QString port) {
  udpSocket->writeDatagram(content, QHostAddress(ip), port.toUInt());
}

void QQServer::parseCommand(QString jsonStr, QHostAddress ip, quint16 port) {

  QJsonParseError error;
  QByteArray jsonByteArray = jsonStr.toUtf8();
  QJsonObject obj = QJsonDocument::fromJson(jsonByteArray, &error).object();

  QString command = obj["command"].toString();

  if (command == "register") {
    registerRespond(obj, ip, port);
  } else if (command == "login") {
    loginRespond(obj, ip, port);
  } else if (command == "sendChatMessage") {
    sendChatMessageRespond(obj, ip, port);
  } else if (command == "search") {
    searchRespond(obj, ip, port);
  } else if (command == "add") {
    addRespond(obj, ip, port);
  } else if (command == "createGroup") {
    createGroupRespond(obj, ip, port);
  } else if (command == "delete") {
    deleteRespond(obj, ip, port);
  } else if (command == "friendRequest") {
    friendRespond(obj, ip, port);
  } else if (command == "groupRequest") {
    groupRespond(obj, ip, port);
  } else if (command == "messageRequest") {
    messageRespond(obj, ip, port);
  } else if (command == "personInfoRequest") {
    getPersonalInfoRespond(obj, ip, port);
  } else if (command == "groupInfoRequest") {
    getGroupInfoRespond(obj, ip, port);
  } else if (command == "changePInfoRequest") {
    changePInfo(obj, ip, port);
  } else if (command == "changeGInfoRequest") {
    changeGInfo(obj, ip, port);
  } else {

    sendMessage(QString("未知命令"), ip, port);
  }
}

void QQServer::registerRespond(QJsonObject obj, QHostAddress ip, quint16 port) {

  QString user = obj["user"].toString();
  QString password = obj["password"].toString();

  sendMessage(atModel->addUserAccount(user, password), ip, port);
}

void QQServer::loginRespond(QJsonObject obj, QHostAddress ip, quint16 port) {

  int id = obj["id"].toInt();
  QString password = obj["password"].toString();

  QByteArray respond = atModel->checkAccount(id, password);
  bool loginOk = QJsonDocument::fromJson(respond).object()["result"].toBool();
  if (loginOk) {
    onlineUser.append(new User(ip, port, id));

    User *last = onlineUser.last();
    qDebug() << "上线：" << last->id << last->ip << last->port;
  }

  sendMessage(respond, ip, port);
}

void QQServer::changePInfo(QJsonObject obj, QHostAddress, quint16) {

  int id = obj["id"].toInt();
  QString name = obj["name"].toString();
  QString icon = obj["icon"].toString();
  int gender = obj["gender"].toInt();
  QString area = obj["area"].toString();
  QString edu = obj["edu"].toString();
  QString saying = obj["saying"].toString();
  QString birth = obj["birth"].toString();

  atModel->updateUserInfo(id, name, gender, birth, area, edu, saying);
}

void QQServer::changeGInfo(QJsonObject obj, QHostAddress ip, quint16 port) {
  qDebug() << "changeGInfo";

  int id = obj["id"].toInt();
  QString name = obj["name"].toString();
  QString notice = obj["notice"].toString();
  QString summary = obj["summary"].toString();

  gpModel->updateGroupInfo(id, name, summary, notice);
}

void QQServer::sendChatMessageRespond(QJsonObject obj, QHostAddress ip,
                                      quint16 port) {

  int sendId = obj["sendId"].toInt();
  int targetId = obj["targetId"].toInt();
  QString content = obj["content"].toString();
  int time = obj["time"].toInt();
  int type = obj["type"].toInt();

  if (targetId / 100000 == 1) {

    QJsonObject targetObj = getTargetIpPort(targetId);
    QString targetIp = targetObj["ip"].toString();
    QString targetPort = QString("%1").arg(targetObj["port"].toInt());

    QJsonObject jsonObj;
    jsonObj.insert("command", "sendChatMessageBack");
    jsonObj.insert("groupId", 0);
    jsonObj.insert("sendId", sendId);
    jsonObj.insert("content", content);
    jsonObj.insert("time", time);
    QString diagram = QJsonDocument(jsonObj).toJson();
    sendMessage(diagram, targetIp, targetPort);

    fdModel->sendMessage(sendId, targetId, type, time, content);
  } else {

    Group *group = new Group(targetId, gpModel);

    for (int i = 0; i < group->memberList.length(); i++) {
      if (group->memberList[i] == sendId) {
        continue;
      }

      int memberId = group->memberList[i];

      QJsonObject memberObj = getTargetIpPort(memberId);
      QString memberIp = memberObj["ip"].toString();
      QString memberPort = QString("%1").arg(memberObj["port"].toInt());
      QJsonObject jsonObj;
      jsonObj.insert("command", "sendChatMessageBack");
      jsonObj.insert("groupId", targetId);
      jsonObj.insert("sendId", sendId);
      jsonObj.insert("content", content);
      jsonObj.insert("time", time);
      QString diagram = QJsonDocument(jsonObj).toJson();
      sendMessage(diagram, memberIp, memberPort);
    }

    gpModel->sendMessage(targetId, sendId, type, time, content);
  }
}

void QQServer::searchRespond(QJsonObject obj, QHostAddress ip, quint16 port) {

  int targetId = obj["targetId"].toInt();

  QJsonObject userObj =
      QJsonDocument::fromJson(atModel->userInfo(targetId)).object();
  QJsonObject groupObj =
      QJsonDocument::fromJson(gpModel->groupInfo(targetId, 100001)).object();

  QJsonObject respondObj;
  respondObj.insert("command", "searchBack");
  if (!userObj["result"].isNull()) {
    respondObj.insert("finded", true);
    respondObj.insert("isPerson", true);
    respondObj.insert("pName", userObj["result"].toObject()["name"].toString());
    respondObj.insert("headImg",
                      userObj["result"].toObject()["icon"].toString());

  } else if (!groupObj["result"].isNull()) {
    respondObj.insert("finded", true);
    respondObj.insert("isPerson", false);
    respondObj.insert("pName", userObj["result"].toObject()["name"].toString());
    respondObj.insert("headImg",
                      userObj["result"].toObject()["icon"].toString());
  } else {
    respondObj.insert("finded", false);
    respondObj.insert("isPerson", false);
    respondObj.insert("pName", "无结果");
    respondObj.insert("headImg", "无结果");
  }
  QString diagram = QJsonDocument(respondObj).toJson();
  sendMessage(diagram, ip, port);
}

void QQServer::addRespond(QJsonObject obj, QHostAddress ip, quint16 port) {

  int sendId = obj["sendId"].toInt();
  int targetId = obj["targetId"].toInt();
  if (targetId / 100000 == 1) {

    bool succeed = fdModel->addFriend(sendId, targetId);

    QJsonObject respondObj;
    respondObj.insert("command", "addBack");

    QJsonObject targetObj = getTargetIpPort(targetId);
    QJsonObject respondToTargetObj;
    QString targetIp = targetObj["ip"].toString();
    QString targetPort = QString("%1").arg(targetObj["port"].toInt());
    respondToTargetObj.insert("command", "addBack");
    if (succeed) {
      respondObj.insert("friendId", targetId);
      sendMessage(QJsonDocument(respondObj).toJson(), ip, port);
      respondToTargetObj.insert("friendId", sendId);
      sendMessage(QJsonDocument(respondToTargetObj).toJson(), targetIp,
                  targetPort);
    } else {
      respondObj.insert("friendId", 0);
      sendMessage(QJsonDocument(respondObj).toJson(), ip, port);
    }
  } else {
    QJsonObject respondObj;
    respondObj.insert("command", "addBack");
    if (true == gpModel->joinGroup(targetId, sendId, 0)) {
      respondObj.insert("friendId", targetId);
    } else {
      respondObj.insert("friendId", 0);
    }
    sendMessage(QJsonDocument(respondObj).toJson(), ip, port);
  }
}

void QQServer::createGroupRespond(QJsonObject obj, QHostAddress ip,
                                  quint16 port) {
  int ownerId = obj["id"].toInt();
  qDebug() << ownerId;
  int groupId = gpModel->createGroup(ownerId);
  qDebug() << groupId;
  QJsonObject respondObj;
  respondObj.insert("command", "createGroupBack");
  respondObj.insert("groupId", groupId);
  sendMessage(QJsonDocument(respondObj).toJson(), ip, port);
}

void QQServer::deleteRespond(QJsonObject obj, QHostAddress ip, quint16 port) {

  QJsonObject respondObj;
  respondObj.insert("command", "deleteBack");

  QString diagram = QJsonDocument(respondObj).toJson();
  sendMessage(diagram, ip, port);
}

void QQServer::friendRespond(QJsonObject obj, QHostAddress ip, quint16 port) {

  int id = obj["id"].toInt();

  sendMessage(fdModel->friendList(id), ip, port);
}

void QQServer::groupRespond(QJsonObject obj, QHostAddress ip, quint16 port) {

  int id = obj["id"].toInt();

  sendMessage(gpModel->groupList(id), ip, port);
}

void QQServer::messageRespond(QJsonObject obj, QHostAddress ip, quint16 port) {

  int targetId = obj["targetId"].toInt();
  int requestId = obj["requestId"].toInt();

  QByteArray diagram;
  if (targetId / 100000 == 1) {
    diagram = fdModel->messageList(requestId, targetId);
  } else {
    diagram = gpModel->messageList(targetId);
  }
  sendMessage(diagram, ip, port);
}

void QQServer::getPersonalInfoRespond(QJsonObject obj, QHostAddress ip,
                                      quint16 port) {

  int targetId = obj["targetId"].toInt();
  qDebug() << targetId;

  QJsonObject userObj =
      QJsonDocument::fromJson(atModel->userInfo(targetId)).object();
  qDebug() << userObj;

  QString diagram = QJsonDocument(userObj).toJson();
  sendMessage(diagram, ip, port);
}

void QQServer::getGroupInfoRespond(QJsonObject obj, QHostAddress ip,
                                   quint16 port) {

  int targetId = obj["targetId"].toInt();
  int clientId = obj["clientId"].toInt();

  QByteArray diagram = gpModel->groupInfo(targetId, clientId);
  sendMessage(diagram, ip, port);
}

void QQServer::test(SqlAccountModel *atModel, SqlFriendModel *fdModel,
                    SqlGroupModel *gpModel) {

  QJsonObject obj;
  obj.insert("comm", "1");
  qDebug() << obj["groupId"].toInt();
}
