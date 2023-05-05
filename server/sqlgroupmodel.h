#ifndef SQLGROUPMODEL_H
#define SQLGROUPMODEL_H

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QtSql/QSqlTableModel>

class SqlGroupModel : public QSqlTableModel {
  Q_OBJECT
public:
  SqlGroupModel(QObject *parent, QSqlDatabase db);
  ~SqlGroupModel();

  int createGroup(const int &masterID);

  QByteArray memberList(const int &gID);

  void updateGroupInfo(const int &id, const QString &name, const QString &intro,
                       const QString &notice);

  void delGroup(const int &gID);

  bool joinGroup(const int &gID, const int &mID, const int &rank);

  void quitGroup(const int &gID, const int &mID);

  void sendMessage(const int &gID, const int &mID, const int &type,
                   const int &datetime, const QString &message);

  QByteArray groupList(const int &mID);

  QByteArray messageList(const int &gID);

  QByteArray groupInfo(const int &gID, const int &mID);
};

#endif
