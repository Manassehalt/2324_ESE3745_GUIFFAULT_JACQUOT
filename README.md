# 2324_ESE3745_GUIFFAULT_JACQUOT


## 6.1. Génération de 4 PWM

Générer quatre PWM sur les bras de pont U et V pour controler le hacheur à partir du timer déjà attribué sur ces pins.

<ins> **Cahier des charges :** </ins>

- [x] Fréquence de la PWM : 20kHz

La frequence de base  de la clock est 170MHz

Pour cela on utilise le timer 1 et on regle ARR=8499 PRSC=0


- [x] Mettre en place un Temps mort

Temps mort minimum d'apres la datasheet:

turn off delay time 39ns

fall time 35ns

> [!CAUTION]
>On va donc prendre un temps mort de 100ns pour avoir une bonne marge de securité.
> 
>Le dead Time est une **nécessité** pour éviter de detruire les transistors durant la commutation 

- [x] Résolution minimum : 10bits.

Pour les tests, fixer le rapport cyclique à 60%.
alpha =0.6
Une fois les PWM générées, les afficher sur un oscilloscope et les faire vérifier par votre professeur.
> [!IMPORTANT]
> 
>!!!Activer le PWM et le PWMN!!! :
> 
>```C
>HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
>HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); la complementaire
>```


##  Code shell.c : 

 ```C
 /* shell.c*/

#include "usart.h"
#include "mylibs/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PERCENTAGE 100
#define PWM_MAX_DUTY_CYCLE 8499

 ```
 ```C
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
```
### Commandes du shell
```C
	if(newCmdReady){
		if(strcmp(argv[0],"WhereisBrian?")==0){
			HAL_UART_Transmit(&huart2, brian, sizeof(brian), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"help")==0){
			int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "Print all available functions here\r\n");
			HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
		}
		else if(argc == 2 && strcmp(argv[0], "speed") == 0) {
	        int percentage = atoi(argv[1]);  // Convertit l'argument en pourcentage
	        setPWM(percentage);
		}
		else{
			HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
		}
		HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
		newCmdReady = 0;
	}
}
```

C'est dans cette partie du code que l'on rajoute de nouvelles commandes avec la suite de else If.
> [!TIP]
> Pour faire des commandes en deux mots comme speed xxx on compare simultanément la commande en terme de nombre de mots de commande (ici 2)
>et on compare le premier terme avec argv[0] 

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        uartRxReceived = 1;
        HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
    }
}
```
<p align="left"> <img src="tek00000.png" width="85%" height="auto" /> </p>
<ins>Image du PWM avec son complémentaire avec alpha = 60%, F=20kHz et DT = 0( uniquement quand pas de puissance )<ins/>





![image](https://github.com/user-attachments/assets/0cd49918-679e-4c6d-aa4c-c3140a1233a6)


![image](https://github.com/user-attachments/assets/38719636-1896-470f-a62b-476840618f06)
