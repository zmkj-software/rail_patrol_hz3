
#pragma once
#include "serialport/serialport.h"
#include "basic/time_util.h"
#include "basic/byte_util.h"
#include "rail/tcms_recv_info.h"
#include "tcmsdata.h"
#include "ipc/ipc.h"
#include "log/log_util.h"
#include "boost/format.hpp"
#include <iostream>
#include <time.h>

class SerialPortTcms : public cpp_utl::SerialPort
{
public:
    bool initTcms(const std::string& port_in);
    bool start();
    void recvData(std::size_t size_recv);

private:
    void parseOneFrm(uint8_t* p_data);

    std::string port;
    uint64_t timestamp_last_recv;

    cpp_utl::SerialPortSelfCheck serialport;

    // biz
    rail::TCMSRecvInfoProducer tcms_recv_info_producer;
    cpp_utl::SharedMem<rail::TCMSRecvInfo> shm_tcms_recv_info;
    time_t timer;
};
