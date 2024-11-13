/*
 * shell.c
 *
 *  Created on: Oct 1, 2023
 *      Author: nicolas
 */
#include "usart.h"
#include "mylibs/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PERCENTAGE 100
#define PWM_MAX_DUTY_CYCLE 4249

uint8_t prompt[]="user@Nucleo-STM32G474RET6>>";
uint8_t started[]=
		"\r\n*-----------------------------*"
		"\r\n| Welcome on Nucleo-STM32G474 |"
		"\r\n*-----------------------------*"
		"\r\n";
uint8_t newline[]="\r\n";
uint8_t backspace[]="\b \b";
uint8_t cmdNotFound[]="Command not found\r\n";
uint8_t brian[]="Brian is in the kitchen\r\n";
uint8_t uartRxReceived;
uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];

char	 	cmdBuffer[CMD_BUFFER_SIZE];
int 		idx_cmd;
char* 		argv[MAX_ARGS];
int		 	argc = 0;
char*		token;
int 		newCmdReady = 0;
extern TIM_HandleTypeDef htim1;
int i=0;
int i_old=50;


void setPWM(int percentage) {
    if (percentage > MAX_PERCENTAGE) {
        percentage = MAX_PERCENTAGE;
    } else if (percentage < 0) {
        percentage = 0;
    }
    int dutyCycle = (percentage * PWM_MAX_DUTY_CYCLE) / MAX_PERCENTAGE;
    TIM1->CCR1 = dutyCycle;
    TIM1->CCR2 = PWM_MAX_DUTY_CYCLE - dutyCycle;
}

void processCommand() {
    if (argc == 2 && strcmp(argv[0], "speed") == 0) {
        int percentage = atoi(argv[1]);  // Convertit l'argument en pourcentage
        setPWM(percentage);
    } else {
        HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
    }
}


void Shell_Init(void){
	memset(argv, 0, MAX_ARGS*sizeof(char*));
	memset(cmdBuffer, 0, CMD_BUFFER_SIZE*sizeof(char));
	memset(uartRxBuffer, 0, UART_RX_BUFFER_SIZE*sizeof(char));
	memset(uartTxBuffer, 0, UART_TX_BUFFER_SIZE*sizeof(char));

	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
	HAL_UART_Transmit(&huart2, started, strlen((char *)started), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, prompt, strlen((char *)prompt), HAL_MAX_DELAY);
}

void Shell_Loop(void){
	if(uartRxReceived){
		switch(uartRxBuffer[0]){
		case ASCII_CR: // Nouvelle ligne, instruction à traiter
			HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
			cmdBuffer[idx_cmd] = '\0';
			argc = 0;
			token = strtok(cmdBuffer, " ");
			while(token!=NULL){
				argv[argc++] = token;
				token = strtok(NULL, " ");
			}
			idx_cmd = 0;
			newCmdReady = 1;
			break;
		case ASCII_BACK: // Suppression du dernier caractère
			cmdBuffer[idx_cmd--] = '\0';
			HAL_UART_Transmit(&huart2, backspace, sizeof(backspace), HAL_MAX_DELAY);
			break;

		default: // Nouveau caractère
			cmdBuffer[idx_cmd++] = uartRxBuffer[0];
			HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
		}
		uartRxReceived = 0;
	}

	if(newCmdReady){
		if(strcmp(argv[0],"WhereisBrian?")==0){
			HAL_UART_Transmit(&huart2, brian, sizeof(brian), HAL_MAX_DELAY);
		}
		else if (strcmp(argv[0], "start") == 0) {
		     TIM1->CCR1 = PWM_MAX_DUTY_CYCLE / 2;
		     TIM1->CCR2 = PWM_MAX_DUTY_CYCLE / 2;

		     HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		     HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
		     HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
		     HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

		     int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE,
		                                              "PWM started at 50%% duty cycle\r\n");
		     HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
		}
		else if (strcmp(argv[0], "stop") == 0) {
		                 // Stoppe le PWM sur tous les canaux
		      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
		      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
		      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);

		      int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE,
		                                                   "PWM stopped\r\n");
		      HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
		}
		else if (strcmp(argv[0], "help") == 0) {
		            int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE,
		                                              "Available commands:\r\n"
		                                              " - start: Start PWM at 50%% duty cycle\r\n"
		                                              " - stop: Stop PWM generation\r\n"
		                                              " - speed <percentage>: Set motor speed as a percentage (0-100%)\r\n"
		                                              " - WhereisBrian?: Where is Brian ?\r\n");
		            HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
		}
		else if (argc == 2 && strcmp(argv[0], "speed") == 0) {
		    int percentage = atoi(argv[1]);  // Convertit l'argument en pourcentage
		    if (percentage >= 0 && percentage <= 100) {
		        static int i_old = 50;
		        int i;

		        for (i = i_old; i != percentage;) {
		            if (i > percentage) {
		                setPWM(i);
		                i--;
		                HAL_Delay(100);
		            } else if (i < percentage) {
		                setPWM(i);
		                i++;
		                HAL_Delay(100);
		            }
		        }

		        i_old = percentage;
		    } else {
		        int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE,
		                                          "Error: Percentage must be between 0 and 100\r\n");
		        HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
		    }
		}
		else{
			HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
		}
		HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
		newCmdReady = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        uartRxReceived = 1;
        HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
    }
}
