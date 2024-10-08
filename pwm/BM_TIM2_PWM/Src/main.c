#include <stdint.h>
#include "stm32f1xx.h"

volatile uint16_t adc_data[3];

int main(void)
{
    // Configuração dos pinos para PWM
    {
        volatile unsigned long *CR;
        unsigned short tPIN = 0;
        unsigned short offset = 0x00;

        // PB0 para PWM Canal 3
        RCC->APB2ENR |= 8;  // Habilita PORT B
        CR = (volatile unsigned long *) (&GPIOB->CRL + offset);
        *CR &= ~(0xF << (tPIN * 4));  // Reseta o pino alvo
        *CR |= ((0x3 << (tPIN * 4)) | (0x2 << (tPIN * 4 + 2)));  // Direção e opção

        // PA6 para PWM Canal 1
        tPIN = 6;
        offset = 0x00;
        RCC->APB2ENR |= 4;  // Habilita PORT A
        CR = (volatile unsigned long *) (&GPIOA->CRL + offset);
        *CR &= ~(0xF << (tPIN * 4));  // Reseta o pino alvo
        *CR |= ((0x3 << (tPIN * 4)) | (0x2 << (tPIN * 4 + 2)));  // Direção e opção

        // PA7 para PWM Canal 2
        tPIN = 7;
        *CR &= ~(0xF << (tPIN * 4));  // Reseta o pino alvo
        *CR |= ((0x3 << (tPIN * 4)) | (0x2 << (tPIN * 4 + 2)));  // Direção e opção
    }

    // Configuração dos pinos de entrada analógica para ADC
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  // Habilita o clock para GPIOA
        GPIOA->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);  // PA1 como entrada analógica
        GPIOA->CRL &= ~(GPIO_CRL_MODE2 | GPIO_CRL_CNF2);  // PA2 como entrada analógica
        GPIOA->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);  // PA3 como entrada analógica
    }

    // Configuração do Timer3
    {
        RCC->APB2ENR |= (1 << 0);  // Clock AFIO
        AFIO->MAPR &= ~(1 << 10);  // Reseta remapeamento dos pinos
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;  // Clock TIMER3
        TIM3->PSC = 0;
        TIM3->ARR = 100;
        TIM3->CCMR1 = 0x6060;  // Configura PWM no modo 1 para CH1 e CH2
        TIM3->CCMR2 = 0x0060;  // Configura PWM no modo 1 para CH3
        TIM3->CCER = 0x0111;   // Habilita os canais de saída
        TIM3->CR1 |= (1 << 0);  // Habilita o Timer
    }

    // Configuração do ADC
    {
        RCC->APB2ENR |= 1 << 9;  // Habilita o clock do ADC1
        ADC1->CR1 = (1 << 8);  // Habilita o modo SCAN
        ADC1->CR2 = (1 << 1);  // Habilita o modo de conversão contínua
        ADC1->CR2 |= (7 << 17);  // Seleção do evento externo apontado para SWSTART
        ADC1->CR2 &= ~(1 << 11);  // Alinhamento de dados à direita
        ADC1->SMPR2 |= (7 << 3) | (7 << 6) | (7 << 9);  // Tempo de amostragem de 239,5 ciclos
        ADC1->SQR1 |= (2 << 20);  // SQR1_L = 2 para 3 conversões
        ADC1->CR2 |= (1 << 8);  // Habilita DMA para ADC
        ADC1->SQR3 |= (1 << 0);  // Primeiro canal: PA1 (canal 1)
        ADC1->SQR3 |= (2 << 5);  // Segundo canal: PA2 (canal 2)
        ADC1->SQR3 |= (3 << 10);  // Terceiro canal: PA3 (canal 3)

        ADC1->CR2 |= 1 << 0;  // ADON = 1 habilita o ADC1
        uint32_t delay = 10000;
        while (delay--);

        ADC1->SR = 0;  // Limpa o registrador de status
        ADC1->CR2 |= (1 << 20);  // Habilita a conversão no evento externo
        ADC1->CR2 |= 1 << 22;  // Inicia a conversão
    }

    // Configuração do DMA
    {
        RCC->AHBENR |= RCC_AHBENR_DMA1EN;  // Habilita o acesso ao Clock do DMA1
        DMA1_Channel1->CCR = 0x000005A0;
        DMA1_Channel1->CNDTR = 3;
        DMA1_Channel1->CPAR = (uint32_t)(&ADC1->DR);
        DMA1_Channel1->CMAR = (uint32_t)(adc_data);
        DMA1_Channel1->CCR |= DMA_CCR_EN;
    }

    // Loop principal
    while (1)
    {
        // Ajusta o PWM para cada canal
        TIM3->CCR1 = adc_data[0] / 42;  // Ajusta o PWM para CH1
        TIM3->CCR2 = adc_data[1] / 42;  // Ajusta o PWM para CH2
        TIM3->CCR3 = adc_data[2] / 42;  // Ajusta o PWM para CH3
    }
}
