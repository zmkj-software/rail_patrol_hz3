#include "tcmsdata.h"
#include <array>
#include <iostream>

uint16_t littleBigTrans(uint16_t val_little)
{
    uint16_t val_big;
    char bytes_little[2];

    memcpy(bytes_little, &val_little, 2);

    char bytes_big[2];
    bytes_big[0] = bytes_little[1];
    bytes_big[1] = bytes_little[0];

    memcpy(&val_big, bytes_big, 2);
    return val_big;
}

//FrmRecvReadStatus-------------------------------------
bool FrmRecvReadStatus::fromBytes(uint8_t* p_udata)
{
    // check len
    char* p_data = (char*)p_udata;
    uint32_t len = sizeof (p_data);
    if (len < FrmRecvReadStatus::LEN)
    {
        return false;
    }

    // find the frm head
    uint32_t i_head = -1;
    for (uint32_t i = 0; i < len; i++)
    {
        if (p_data[i] == (char)(0x01) &&
            i + 1 < len && p_data[i+1] == (char)(0x03) &&
            i + 2 < len && p_data[i+2] == (char)(0x10))
        {
            i_head = i;
            p_udata = p_udata + i;
            break;
        }
    }
    if (i_head == -1)
    {
        // clear all
        LOG_ERROR << "frm head 0x01 not found";
        free(p_udata);
        p_udata = NULL;
        return false;
    }

    p_data = (char *)p_udata;
    if ((uint32_t)sizeof (p_udata) < FrmRecvReadStatus::LEN)
    {
        return false;
    }

    //correct and complete telegram
    char* p_body = p_data + 3;

    memcpy(&heart,              p_body,       2);heart = littleBigTrans(heart);
    memcpy(&mix1,               p_body+2,     2);mix1 = littleBigTrans(mix1);
    memcpy(&velocity,           p_body+4,     2);velocity = littleBigTrans(velocity);
    memcpy(&id_station_start,   p_body+6,     2);id_station_start = littleBigTrans(id_station_start);
    memcpy(&id_station_end,     p_body+8,     2);id_station_end = littleBigTrans(id_station_end);
    memcpy(&id_station_next,    p_body+10,    2);id_station_next = littleBigTrans(id_station_next);
    memcpy(&id_station_current, p_body+12,    2);id_station_current = littleBigTrans(id_station_current);
    memcpy(&voltage,            p_body+14,    2);voltage = littleBigTrans(voltage);
    memcpy(&current,            p_body+16,    2);current = littleBigTrans(current);

    p_udata = p_udata + FrmRecvReadStatus::LEN;
    return true;
}
