#ifndef SQLACCOUNTMODEL_H
#define SQLACCOUNTMODEL_H

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QtSql/QSqlTableModel>

class SqlAccountModel : public QSqlTableModel {
  Q_OBJECT
public:
  SqlAccountModel(QObject *parent, QSqlDatabase db);
  ~SqlAccountModel();

  QByteArray addUserAccount(const QString &userName,
                            const QString &userPassword);

  QByteArray checkAccount(const int &userID, const QString &userPassword);

  QByteArray userInfo(const int &userID);

  void updateIcon(const int &userID, const QString &iconURL);

  void updateUserInfo(const int &userID, const QString &name, const int &gender,
                      const QString &birth, const QString &area,
                      const QString &education, const QString &signature);

  QByteArray messageList(const int &ID);
};
#endif
