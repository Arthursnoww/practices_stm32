#include <stdint.h>

#if !defined(_SOFT_FP) && defined(_ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#include "stm32f1xx.h"


void EXTI0_IRQHandler(void){
    // Toggle LED connected to PC13
    GPIOC->ODR ^= (1 << 13);
    // Clear pending interrupt flag
    EXTI->PR = (1 << 0);
}

void EXTI1_IRQHandler(void){
    GPIOC->ODR ^= (1 << 14);
    EXTI->PR = (1 << 1);
}

void EXTI15_10_IRQHandler(void){
    if (EXTI->PR & (1 << 10)) {
        GPIOC->ODR ^= (1 << 15);
        EXTI->PR = (1 << 10);
    }
}

int main(void){
    // habilitando clock
    RCC->APB2ENR |= (1 << 3) | (1 << 4);

    // Configurando PB0, PB1, PB10 como entrada e pull-up
    GPIOB->CRL &= 0xFFFFFF00;
    GPIOB->CRL |= 0x00000088;
    GPIOB->CRH &= 0xFFFFF0FF;
    GPIOB->CRH |= 0x00000800;
    GPIOB->ODR |= 0x00000403;

    // Configurando PC13, PC14, PC15 como saida
    GPIOC->CRH &= 0x000FFFFF;
    GPIOC->CRH |= 0x22200000;

    // Reset PC13, PC14, PC15
    GPIOC->ODR &= ~(0xE000);

    // Enable AFIO clock
    RCC->APB2ENR |= (1 << 0);

    // Configure EXTI lines for PB0, PB1, PB10
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PB | AFIO_EXTICR1_EXTI1_PB;
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI10_PB;

    // Enable falling edge trigger for EXTI0, EXTI1, EXTI10
    EXTI->FTSR |= (1 << 0) | (1 << 1) | (1 << 10);

    // Unmask EXTI0, EXTI1, EXTI10
    EXTI->IMR |= (1 << 0) | (1 << 1) | (1 << 10);

    // Set priority and enable EXTI0, EXTI1, EXTI15_10 interrupts in NVIC
    NVIC_SetPriority(EXTI0_IRQn, 0x10);
    NVIC_SetPriority(EXTI1_IRQn, 0x20);
    NVIC_SetPriority(EXTI15_10_IRQn, 0x30);
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI15_10_IRQn);

    __enable_irq();

    while(1){
    }
}
