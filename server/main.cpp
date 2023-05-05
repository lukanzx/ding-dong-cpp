#include "qqserver.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QQServer w;
  w.show();
  return a.exec();
}
