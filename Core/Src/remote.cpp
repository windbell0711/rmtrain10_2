/**
******************************************************************************
 * @file    remote.cpp/h
 * @brief   Remote control. 遥控器
 ******************************************************************************
 * Copyright (c) 2026 Team JiaoLong-SJTU
 * All rights reserved.
 ******************************************************************************
 */

#include "../Inc/remote.h"

constexpr uint16_t REMOTE_CONNECT_TIMEOUT = 500u; 
// Constructor 构造函数
Remote::Remote(UART_HandleTypeDef *huart): huart_(huart), connect_(REMOTE_CONNECT_TIMEOUT){
    switch_.l = RCSwitchState_e::DOWN;
    switch_.r = RCSwitchState_e::DOWN;
}

// Start UART(SBUS) receive. 打开UART接收
void Remote::init() {
    // Your code here
    constexpr uint8_t initMsg[] = "Remote";
    HAL_UART_Transmit(&huart1, initMsg, 6, 5000);
    reset();
    HAL_UARTEx_ReceiveToIdle_DMA(huart_, rx_data_, 18);
}

// Reset RC data. 重置遥控器数据
void Remote::reset() {
    // Your code here
    channel_.l_row = 1024;
    channel_.l_col = 1024;
    channel_.r_row = 1024;
    channel_.r_col = 1024;
    channel_.dial_wheel = 0;  // TODO
    switch_.l = RCSwitchState_e::DOWN;
    switch_.r = RCSwitchState_e::DOWN;
}

// Check for uart correspondence. 检查串口是否匹配
bool Remote::rxMsgCheck(UART_HandleTypeDef *huart) const {
    // Your code here 
    if (huart == huart_)
    {
        return true;
    }
    return false;
}

// Update connect status, restart UART(SBUS) receive.
// 更新连接状态，重新打开UART(SBUS)接收
// void Remote::rxMsgCallback(uint8_t* rx_data_){
void Remote::rxMsgCallback(){
    // Your code here
    connect_.refresh();
    HAL_UARTEx_ReceiveToIdle_DMA(huart_, rx_data_, 18);
}

// Unpack data. 数据解包
void Remote::handle() {
    // Your code here
    uint16_t r_row = static_cast<uint16_t>(rx_data_[0]) | ((static_cast<uint16_t>(rx_data_[1]) & 0b111) << 8);
    uint16_t r_col = ((static_cast<uint16_t>(rx_data_[1]) & 0b11111000) >> 3)
                   | ((static_cast<uint16_t>(rx_data_[2]) & 0b00111111) << 5);
    uint16_t l_col = ((static_cast<uint16_t>(rx_data_[2]) & 0b11000000) >> 6)
                   | ((static_cast<uint16_t>(rx_data_[3]) & 0b11111111) << 2)
                   | ((static_cast<uint16_t>(rx_data_[4]) & 0b00000001) << 10);
    uint16_t l_row = ((static_cast<uint16_t>(rx_data_[4]) & 0b11111110) >> 1)
                   | ((static_cast<uint16_t>(rx_data_[5]) & 0b00001111) << 7);
    if (!(364 <= l_row && l_row <= 1684 &&
        364 <= l_col && l_col <= 1684 &&
        364 <= r_row && r_row <= 1684 &&
        364 <= r_col && r_col <= 1684))
    {
        return;
    }
    channel_.l_row = l_row;
    channel_.l_col = l_col;
    channel_.r_row = r_row;
    channel_.r_col = r_col;
    switch_.l = static_cast<RCSwitchState_e>((rx_data_[5] & 0b00110000) >> 4);
    switch_.r = static_cast<RCSwitchState_e>((rx_data_[5] & 0b11000000) >> 6);
}
