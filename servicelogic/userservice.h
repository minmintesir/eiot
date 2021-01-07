#ifndef USERSERVICE_H
#define USERSERVICE_H
//
#include <QObject>

struct User_Message_  {
  uint userid;
  ::std::string buildingnumber;
  ::std::string roomnumber;
  ::std::string panelsn;
  ::std::string adapteraddr;
  ::std::string heatmeteraddr;
  ::std::string controlvalveaddr;
};

class UserService : public QObject
{
    Q_OBJECT
public:
    explicit UserService(QObject *parent = nullptr);
    User_Message_ User_Message;
    //QList <User_Message_> User_Message_list;

signals:

public slots:
};

#endif // USERSERVICE_H
