#include "singleapplication.h"


QtSingleApplication::QtSingleApplication(int &argc, char **argv) : QApplication(argc, argv, true)
{
    _singular = new QSharedMemory("SharedMemoryForOnlyOneInstanceOfYourBeautifulApplication", this);
}

QtSingleApplication::~QtSingleApplication()
{
    if(_singular->isAttached())
        _singular->detach();
}

bool QtSingleApplication::lock()
{
    if(_singular->attach(QSharedMemory::ReadOnly)) {
        _singular->detach();
        return false;
    }

    if(_singular->create(1))
        return true;

    return false;
}
