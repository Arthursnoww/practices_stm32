#include "stm32f1xx.h"

uint8_t color_index = 0;
uint8_t blink_count = 0;
uint8_t led_state = 0;


// Tratador da interrupção do TIM2
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)           // Verifica se a interrupção de atualização ocorreu
    {
        TIM2->SR &= ~TIM_SR_UIF;         // Limpa a flag de interrupção

        if (led_state == 0)
        {
            Set_LED_Color(color_index);  // Liga o LED com a cor atual
            led_state = 1;
        }
        else
        {
            Set_LED_Color(0);            // Desliga o LED
            led_state = 0;

            blink_count++;
            if (blink_count >= 2)        // Se piscou duas vezes
            {
                blink_count = 0;
                color_index = (color_index + 1) % 8; // Alterna para a próxima cor
            }
        }
    }
}


// Função para acionar as cores do LED RGB
void Set_LED_Color(uint8_t color)
{
    GPIOC->ODR |= (1 << 13) | (1 << 14) | (1 << 15); // Desativa todos os LEDs

    switch(color)
    {
        case 0:  // Apagado
            GPIOC->ODR &= ~(1 << 13);
            GPIOC->ODR &= ~(1 << 14);
            GPIOC->ODR &= ~(1 << 15);
            break;
        case 1:  // Vermelho
            GPIOC->ODR |=  (1 << 13);
            GPIOC->ODR &= ~(1 << 14);
            GPIOC->ODR &= ~(1 << 15);
            break;
        case 2:  // Verde
            GPIOC->ODR &= ~(1 << 13);
            GPIOC->ODR |=  (1 << 14);
            GPIOC->ODR &= ~(1 << 15);
            break;
        case 3:  // Azul
            GPIOC->ODR &= ~(1 << 13);
            GPIOC->ODR &= ~(1 << 14);
            GPIOC->ODR |=  (1 << 15);
            break;
        case 4:  // Amarelo (Vermelho + Verde)
            GPIOC->ODR |=  (1 << 13);
            GPIOC->ODR |=  (1 << 14);
            GPIOC->ODR &= ~(1 << 15);
            break;
        case 5:  // Ciano (Verde + Azul)
            GPIOC->ODR &= ~(1 << 13);
            GPIOC->ODR |=  (1 << 14);
            GPIOC->ODR |=  (1 << 15);
            break;
        case 6:  // Roxo (Vermelho + Azul)
            GPIOC->ODR |=  (1 << 13);
            GPIOC->ODR &= ~(1 << 14);
            GPIOC->ODR |=  (1 << 15);
            break;
        case 7:  // Branco (Vermelho + Verde + Azul)
            GPIOC->ODR |=  (1 << 13);
            GPIOC->ODR |=  (1 << 14);
            GPIOC->ODR |=  (1 << 15);
            break;
    }
}

int main(void)
{
	RCC->APB2ENR |= (1 << 4);  // Habilita o clock para o GPIOC
	GPIOC->CRH &= 0x000FFFFF;  // Configura os pinos PC13, PC14 e PC15 como saída push-pull de 2 MHz
	GPIOC->CRH |= 0x22200000;

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Habilita o clock para o TIM2
	TIM2->PSC = 7999;                   // Prescaler para dividir o clock de 8 MHz para 1 kHz (1 ms)
	TIM2->ARR = 500 - 1;             	// Carrega o valor de comparação para o período desejado (500ms)
	TIM2->DIER |= TIM_DIER_UIE;         // Habilita a interrupção de atualização
	TIM2->CR1 |= TIM_CR1_CEN;           // Ativa o Timer
	NVIC_EnableIRQ(TIM2_IRQn);          // Habilita a interrupção do TIM2 no NVIC


    while(1)
    {
    }
}
