# 2324_ESE3745_GUIFFAULT_JACQUOT


6.1. Génération de 4 PWM

Générer quatre PWM sur les bras de pont U et V pour controler le hacheur à partir du timer déjà attribué sur ces pins.

Cahier des charges :

Fréquence de la PWM : 20kHz
La frequence de base 170MHz
Pour cela on utilise le timer 1 et on regle ARR=8499 PRSC=0

Temps mort minimum : à voir selon la datasheet des transistors (faire valider la valeur)
d'apres la datasheet:

turn off delay time 39ns

fall time 35ns

On va donc prendre un temps mort de 100ns pour avoir une bonne marge de securité
Résolution minimum : 10bits.

Pour les tests, fixer le rapport cyclique à 60%.
alpha =0.6
Une fois les PWM générées, les afficher sur un oscilloscope et les faire vérifier par votre professeur.

!!!Activer le PWMN!!! :

	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); la complementaire
