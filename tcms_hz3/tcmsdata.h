#pragma once
#include <string>
#include "log/log_util.h"
#pragma execution_character_set("utf-8")

struct FrmRecvReadStatus
{
    const static uint32_t LEN = 1 + 1 + 2 + (1 + 32);
    uint8_t head;                   // 0x01
    uint8_t cmd;                    // 0x03: read reply
    uint16_t crc;
    // body
    uint8_t num_reg;                //should be 0x10, 16, 16 reg, 32 bytes
    // following is data

    // 32 bytes start
    uint16_t heart;
    uint16_t mix1;
    /*
    bool mix1_tc1_on;
    bool mix1_tc2_on;
    bool mix1_signal_close_door;
    */
    uint16_t velocity;// 1=0.1km/h
    uint16_t id_station_start;
    uint16_t id_station_end;
    uint16_t id_station_next;
    uint16_t id_station_current;
    uint16_t voltage;//1=1V
    uint16_t current;//1=1A
    // 32 bytes end

    bool fromBytes(uint8_t *p_data);
};
