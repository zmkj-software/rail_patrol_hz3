#include <QCoreApplication>
#include "serialporttcms.h"
#include <iostream>
#include "log/log_util.h"
INIT_LOG_OUTSIDE_MAIN_FUNC
int main(int argc, char *argv[])
{
    cpp_utl::initLog("./log", "tcms");
    QCoreApplication a(argc, argv);
    SerialPortTcms p;
    if (!p.initTcms("COM2"))
    {
         LOG_ERROR << "SerialPortTcms init failed";
        return -1;
    }
    return a.exec();
}
