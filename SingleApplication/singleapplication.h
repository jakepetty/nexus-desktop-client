#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QApplication>
#include <QSharedMemory>
class QtSingleApplication : public QApplication
{
public:
    QtSingleApplication( int & argc, char * * argv );
    ~QtSingleApplication();

    bool lock();

private:
    QSharedMemory * _singular;
};

#endif // SINGLEAPPLICATION_H
