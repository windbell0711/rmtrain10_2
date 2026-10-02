#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "../Inc/remote.h"
#include "callback.h"

#include <stdio.h>
#include <stdint.h>

Remote rc(&huart3);

extern uint8_t rx_msg[10];

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart == &huart1)
//     {
//         // if (rx_msg[0] == 'R')
//         // {
//         //     HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
//         // }
//         // else if (rx_msg[0] == 'M')
//         // {
//         //     HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);
//         // }
//
//         HAL_UART_Transmit(&huart1, rx_msg, 10, 500);
//
//         HAL_UART_Receive_DMA(&huart1, rx_msg, 10);
//     }
// }


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart3)
    {
        rc.rxMsgCallback();
        rc.handle();
        1+1;
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == &huart3)
    {
        if (HAL_UARTEx_GetRxEventType(huart) == HAL_UART_RXEVENT_IDLE || HAL_UARTEx_GetRxEventType(huart) == HAL_UART_RXEVENT_TC)
        {
            uint8_t get_msg[] = "RxEventCallback ";
            HAL_UART_Transmit(&huart1, get_msg, 16, 500);
            rc.rxMsgCallback();
            rc.handle();

            uint8_t notify_msg[10];
            snprintf((char *)notify_msg, sizeof(notify_msg), "%u", rc.channel_.l_row);
            HAL_UART_Transmit(&huart1, notify_msg, 10, 500);
        }
    }
    if (huart == &huart1)
    {
        if (HAL_UARTEx_GetRxEventType(huart) == HAL_UART_RXEVENT_IDLE || HAL_UARTEx_GetRxEventType(huart) == HAL_UART_RXEVENT_TC)
        {
            HAL_UART_Transmit(&huart1, rx_msg, Size, 100);
            HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_msg, 10);
        }
    }
}

extern "C" {
void robotInit()
{
    rc.init();
}
}
