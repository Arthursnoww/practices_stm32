#include <stdint.h>

#if !defined(_SOFT_FP) && defined(_ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void delay(uint32_t count) {
    for (uint32_t i = 0; i < count; i++);
}

void apaga_leds(uint32_t *pGPIOC_ODR, uint32_t delay_count) {
    for (int i = 0; i < 5; i++) {
    	*pGPIOC_ODR &= ~0x0000E000; // apaga os LEDs
    }
}

void blink_led(uint32_t *pGPIO_ODR, uint32_t pin, uint32_t delay_count) {
    for (int i = 0; i < 3; i++) {
        *pGPIO_ODR &= ~pin; // Acende o LED
        delay(delay_count);
        *pGPIO_ODR |= pin;  // Apaga o LED
        delay(delay_count);
    }
}

int main(void)
{
	// Define os ponteiros para os Registradores RCC_APB2ENR, GPIOA_CRL, GPIOC_CRH, GPIOC_ODR e GPIOA_IDR

	// Endereço base do RCC-0x4002 1000 + offset do registrador RCC_APB2ENR 0x18
	uint32_t *pRCC_APB2ENR = (uint32_t *)0x40021018;

	// GPIOA_CRL trata da parte low e os pinos 1, 2 e 3 entram nessa parte
	// Endereço do registrador base da porta A-0x4001 0800 + offset do registrador GPIOA_CRL 0x00
	uint32_t *pGPIOA_CRL = (uint32_t *)0x40010800;
	//uint32_t *pGPIOA_ODR = (uint32_t *)0x4001080C; // Adiciona pGPIOA_ODR
	uint32_t *pGPIOA_IDR = (uint32_t *)0x40010808; // GPIOA_IDR é o registrador de entrada

	// GPIOC_CRH trata da parte high e os pinos 13, 14 e 15 entram nessa parte
	// Endereço do registrador base da porta C-0x4001 1000 + offset do registrador GPIOC_CRH 0x04
	uint32_t *pGPIOC_CRH = (uint32_t *)0x40011004;
	uint32_t *pGPIOC_ODR = (uint32_t *)0x4001100C;
	uint32_t *pGPIOA_ODR = (uint32_t *)0x4001080C;

	// 1. Habilita o clock das Portas A e C
	*pRCC_APB2ENR |= 0x14;

	// Configuração dos pinos 13, 14 e 15 como saída:
	// Pino 13:
	*pGPIOC_CRH &= 0xFF0FFFFF;
	*pGPIOC_CRH |= 0x00200000;

	// Pino 14:
	*pGPIOC_CRH &= 0xF0FFFFFF;
	*pGPIOC_CRH |= 0x02000000;

	// Pino 15:
	*pGPIOC_CRH &= 0x0FFFFFFF;
	*pGPIOC_CRH |= 0x20000000;

	// Configuração dos pinos A1, A2 e A3 como entrada:
	// Pino A1:
	*pGPIOA_CRL &= 0xFFFFF0FF;
	*pGPIOA_CRL |= 0x00000400; // Entrada pull-up/pull-down

	// Pino A2:
	*pGPIOA_CRL &= 0xFFFF0FFF;
	*pGPIOA_CRL |= 0x00004000; // Entrada pull-up/pull-down

	// Pino A3:
	*pGPIOA_CRL &= 0xFFF0FFFF;
	*pGPIOA_CRL |= 0x00040000; // Entrada pull-up/pull-down

	// Habilita pull-up nos pinos A1, A2 e A3 (opcional)
	*pGPIOA_ODR |= 0x0000000E;

	for(;;)
	{
		// Verifica o estado dos botões e controla os LEDs
		// Botão no pino A1 -> LED no pino 13
		if ((*pGPIOA_IDR & 0x00000002) == 0) // Verifica se o botão está pressionado
		{
			//blink_led(pGPIOC_ODR, 0x00002000, 50000); // Pisca o LED no pino 13
			*pGPIOC_ODR |= 0x00002000;
		}
		*pGPIOC_ODR &= ~0x00002000;
		//apaga_leds(pGPIOC_ODR, 50000);
		// Botão no pino A2 -> LED no pino 14
		if ((*pGPIOA_IDR & 0x00000004) == 0) // Verifica se o botão está pressionado
		{
			//blink_led(pGPIOC_ODR, 0x00004000, 50000); // Pisca o LED no pino 14
			*pGPIOC_ODR |= 0x00004000;
		}
		*pGPIOC_ODR &= ~0x00004000;
		//apaga_leds(pGPIOC_ODR, 50000);
		// Botão no pino A3 -> LED no pino 15
		if ((*pGPIOA_IDR & 0x00000008) == 0) // Verifica se o botão está pressionado
		{
			//blink_led(pGPIOC_ODR, 0x00008000, 50000); // Pisca o LED no pino 15
			*pGPIOC_ODR |= 0x00008000;
		}
		*pGPIOC_ODR &= ~0x00008000;
		//apaga_leds(pGPIOC_ODR, 50000);
	}
}
