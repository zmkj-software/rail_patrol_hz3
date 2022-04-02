#include "serialporttcms.h"
#include "tcms_station_lut.h"

bool SerialPortTcms::initTcms(const std::string &port_in)
{
    port = port_in;
    timestamp_last_recv = 0;
    if (!tcms_recv_info_producer.init(LUT_ID_STATION_SRC2LOCAL))
    {
        LOG_ERROR << "shm tcms_recv_info_producer init failed";
        return false;
    }
    if (!shm_tcms_recv_info.init("tcms_recv_info", true))
    {
        LOG_ERROR << "shm tcms_recv_info init failed";
        return false;
    }

    start();
    {
        uint8_t bytes[8];
        bytes[0] = 0x01;
        bytes[1] = 0x06;
        bytes[2] = 0x06;
        bytes[3] = 0x42;

        bytes[4] = 0x00;// 0xe2
        bytes[5] = 0xe2;

        bytes[6] = 0xA9;// crc
        bytes[7] = 0x1F;
        uint32_t size_send = sizeof(bytes);
        serialport.sendData(bytes,size_send);
   }

    // config port num
    {
        uint8_t bytes[8];
        bytes[0] = 0x01;
        bytes[1] = 0x06;
        bytes[2] = 0x06;
        bytes[3] = 0x43;

        bytes[4] = 0x00;// 2 port
        bytes[5] = 0x02;

        bytes[6] = 0xF9;// crc
        bytes[7] = 0x57;
        uint32_t size_send = sizeof(bytes);
        serialport.sendData(bytes,size_send);
    }
    // config port info
    {
        uint8_t bytes[13];
        bytes[0] = 0x01;
        bytes[1] = 0x10;
        bytes[2] = 0x06;// reg addr
        bytes[3] = 0x45;

        bytes[4] = 0x00;// reg num
        bytes[5] = 0x02;

        bytes[6] = 0x04;// data len: 4Bytes, 0x04, 2B/port, 2*2B

        bytes[7] = 0x40;// port recv
        bytes[8] = 0xE9;
        // tcms--->pc: 0xE9, 32Bytes, not src
        //combine2Bytes(0x00E9, 0x01, 0);// 0x10 E9

        bytes[9] = 0xcE;// port send
        bytes[10] = 0x20;
        // pc--->tcms: 0xE20, 32Bytes, src
        // combine2Bytes(0x0E20, 0x01, 1);// 0x9E 20

        bytes[11] = 0x8d;// crc
        bytes[12] = 0x8C;
        uint32_t size_send = sizeof(bytes);
        serialport.sendData(bytes,size_send);
    }

    // send start signal
    {
        uint8_t bytes[8];
        bytes[0] = 0x01;
        bytes[1] = 0x06;
        bytes[2] = 0x06;
        bytes[3] = 0x40;
        bytes[4] = 0x00;
        bytes[5] = 0x01;
        bytes[6] = 0x49;
        bytes[7] = 0x56;

        uint32_t size_send = sizeof(bytes);
        serialport.sendData(bytes,size_send);
    }
    LOG_INFO << "init done:" << port_in;

    return true;

}

bool SerialPortTcms::start()
{
//    timer.start(80);
    timestamp_last_recv = cpp_utl::getTimestamp();
    cpp_utl::ParamSerialPort param_in;
    param_in.port = port;
    param_in.baud_rate = 9600;
    param_in.timeinterval_check_ms = 10*1000;
    param_in.timeinterval_send_ms = 1000;

    if (serialport.init(param_in))
    {
        LOG_ERROR << "SerialPort::init failed";
        return false;
    }
    LOG_INFO << "SerialPort::init done";
    return true;
}

void SerialPortTcms::recvData(size_t size_recv)
{
    timestamp_last_recv = cpp_utl::getTimestamp();
    constexpr uint32_t LEN_FRM = 9;
    uint8_t* p_data = nullptr;
    uint32_t size_data = 0;
    buffer.getDataToConsume(size_recv, p_data, size_data);
    while (size_data >= LEN_FRM)
    {
        // find head
        uint32_t cnt_byte_passed = 0;
        uint8_t* p_frm_head = nullptr;
        for (uint32_t i = 0; i < size_data-1; i++)
        {
            if (p_data[i] == 0xff && p_data[i+1] == 0xa5)
            {
                p_frm_head = p_data + i;
                break;
            }
            else cnt_byte_passed++;
        }
        size_data -= cnt_byte_passed;
        buffer.setConsumed(cnt_byte_passed);

        if (size_data >= LEN_FRM)
        {
            parseOneFrm(p_frm_head);
            size_data -= LEN_FRM;
            buffer.setConsumed(LEN_FRM);
            p_data += LEN_FRM;
        }
    }
}

void SerialPortTcms::parseOneFrm(uint8_t *p_data)
{
    static FrmRecvReadStatus frm_recv_status;
    while (frm_recv_status.fromBytes(p_data))
    {
        float velocity_this = frm_recv_status.velocity*0.1;
        if (frm_recv_status.velocity < 1) velocity_this = 0.0;
        tcms_recv_info_producer.update(
                    timestamp_last_recv,
                    frm_recv_status.id_station_end,
                    frm_recv_status.id_station_next,
                    velocity_this,
                    rail::EnumSleepSignal::NoSleep);
        shm_tcms_recv_info.write(tcms_recv_info_producer.getData());
    }
}
