#include <stdint.h>
#include "stm32f1xx.h"

// Definição do clock da CPU e baud rate para comunicação UART
#define CPU_CLK 8000000
#define BaudRate 115200

// Variáveis globais para armazenamento de comandos e controle de estado
char ch = '\0';         // Armazena o caractere recebido via UART
char command[6];        // Buffer para armazenar o comando recebido

// Variáveis de controle de estado e ciclo PWM
uint8_t MAIN_SM = 0;    // Estado principal da máquina de estados
uint8_t duty_cycle;     // Ciclo de trabalho para os sinais PWM
uint8_t demoMode = 0;   // Flag para controle do modo demo

// Variáveis globais para controle das cores RGB
volatile uint16_t red = 0, green = 0, blue = 0;  // Valores PWM para as cores RGB
volatile uint8_t colorIndex = 0;                 // Índice para o ciclo de cores no modo demo
volatile uint8_t increasing = 1;                 // Flag para controle do aumento/diminuição de brilho
static uint16_t brightness = 0;                  // Valor do brilho atual

uint8_t *ptr;  // Ponteiro para dados a serem enviados via UART

// Declaração das funções usadas no código
void RCCEnable();
void GPIOConfig();
void USART1Config();
void Timer3Config();
void timer2Config();
void resetDemoMode();
void updateColorSequence();
void uart_write(char ch);

int main(void){
    // Inicialização de clocks, GPIOs, USART, e timers
    RCCEnable();
    GPIOConfig();
    USART1Config();
    Timer3Config();
    timer2Config();

    // Loop principal do programa
    while (1){
        if (MAIN_SM){ // Verifica se um novo comando foi recebido
            MAIN_SM = 0; // Reinicia o estado da máquina de estados
            if (demoMode){ // Verifica se está no modo demo
                if (command[0] == 'd' || command[0] == 'D'){
                    resetDemoMode(); // Reseta as configurações do modo demo
                    TIM2->CR1 &= ~TIM_CR1_CEN; // Desativa o timer 2
                    demoMode = !demoMode; // Alterna o modo demo
                }
            }
            else{ // Caso não esteja no modo demo, verifica o comando para ajustar o PWM
                switch (command[0])
                {
                case 'R': // Ajusta o PWM do canal vermelho
                case 'r':
                    duty_cycle = (command[1] - '0') * 10 + (command[2] - '0');
                    TIM3->CCR1 = duty_cycle; // Define o ciclo de trabalho no canal 1 (vermelho)
                    break;

                case 'G': // Ajusta o PWM do canal verde
                case 'g':
                    duty_cycle = (command[1] - '0') * 10 + (command[2] - '0');
                    TIM3->CCR2 = duty_cycle; // Define o ciclo de trabalho no canal 2 (verde)
                    break;

                case 'B': // Ajusta o PWM do canal azul
                case 'b':
                    duty_cycle = (command[1] - '0') * 10 + (command[2] - '0');
                    TIM3->CCR3 = duty_cycle; // Define o ciclo de trabalho no canal 3 (azul)
                    break;

                case 'd': // Ativa/desativa o modo demo
                case 'D':
                    demoMode = !demoMode;
                    TIM2->CR1 |= TIM_CR1_CEN; // Ativa o timer 2
                    break;
                }
            }
        }
    }
}

// Função para resetar as configurações do modo demo
void resetDemoMode(){
    red = 0;
    blue = 0;
    green = 0;
    colorIndex = 0;
    increasing = 1;
    brightness = 0;
    TIM3->CCR1 = 0; // Desativa o PWM do canal 1 (vermelho)
    TIM3->CCR2 = 0; // Desativa o PWM do canal 2 (verde)
    TIM3->CCR3 = 0; // Desativa o PWM do canal 3 (azul)
}

// Função para atualizar a sequência de cores no modo demo
void updateColorSequence(){
    // Controla o ciclo de aumento e diminuição de brilho (PWM)
    static uint8_t step = 1;

    if (increasing){ // Se o brilho está aumentando
        brightness += step;
        if (brightness >= 100) // Se o brilho atingir o máximo, inverte o ciclo
            increasing = 0;
    } else { // Se o brilho está diminuindo
        brightness -= step;
        if (brightness == 0){ // Quando atingir o mínimo, avança para a próxima cor
            increasing = 1;
            colorIndex = (colorIndex + 1) % 7; // Cicla entre 7 cores
        }
    }

    // Define as cores de acordo com o índice
    switch (colorIndex)
    {
    case 0: // Vermelho
        red = brightness;
        green = 0;
        blue = 0;
        break;
    case 1: // Verde
        red = 0;
        green = brightness;
        blue = 0;
        break;
    case 2: // Azul
        red = 0;
        green = 0;
        blue = brightness;
        break;
    case 3: // Amarelo
        red = brightness;
        green = brightness;
        blue = 0;
        break;
    case 4: // Ciano
        red = 0;
        green = brightness;
        blue = brightness;
        break;
    case 5: // Roxo
        red = brightness;
        green = 0;
        blue = brightness;
        break;
    case 6: // Branco
        red = brightness;
        green = brightness;
        blue = brightness;
        break;
    }

    // Atualiza os valores PWM
    TIM3->CCR1 = red;   // Atualiza o canal 1 com o valor de vermelho
    TIM3->CCR2 = green; // Atualiza o canal 2 com o valor de verde
    TIM3->CCR3 = blue;  // Atualiza o canal 3 com o valor de azul
}

// Função para enviar um caractere via UART
void uart_write(char ch){
    // Espera até que o registrador de transmissão esteja vazio
    while (!(USART1->SR & USART_SR_TXE))
    {
    }
    // Envia o caractere pelo registrador de dados
    USART1->DR = (ch & 0xFF);
}

// Configuração da USART1
void USART1Config(){
    // Habilita o clock da USART1
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    // Habilita o receptor (RE) e transmissor (TE)
    USART1->CR1 |= USART_CR1_RE | USART_CR1_TE;
    // Configura a taxa de baud rate
    USART1->BRR = ((CPU_CLK + (BaudRate / 2U)) / BaudRate);
    // Habilita a interrupção RXNE para receber dados
    USART1->CR1 |= USART_CR1_RXNEIE;
    // Habilita a interrupção da USART1 no NVIC
    NVIC_EnableIRQ(USART1_IRQn);
    // Habilita a USART1
    USART1->CR1 |= USART_CR1_UE;
}

// Habilitação dos clocks necessários para GPIO e USART
void RCCEnable(){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_USART1EN;
}

// Configuração dos pinos GPIO para UART e PWM
void GPIOConfig(){
    // Configura PA9 como TX (transmissão UART)
    GPIOA->CRH &= 0xFFFFFF0F;
    GPIOA->CRH |= 0x000000B0;
    // Configura PA10 como RX (recepção UART)
    GPIOA->CRH &= 0xFFFFF0FF;
    GPIOA->CRH |= 0x00000400;

    // Configura PA6 (PWM), PA7 (PWM) e PB0 (PWM)
    GPIOA->CRL &= 0x00FFFFFF;
    GPIOA->CRL |= 0xBB000000;
    GPIOB->CRL &= 0xFFFFFFF0;
    GPIOB->CRL |= 0x0000000B;
}

// Configuração do Timer 2 para controle no modo demo
void timer2Config(){
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;  // Habilita o clock do TIM2

    TIM2->PSC = 8000 - 1;  // Prescaler para diminuir a frequência
    TIM2->ARR = 20 - 1;    // Tempo de auto-reload de 20ms

    TIM2->DIER |= TIM_DIER_UIE;  // Habilita a interrupção de atualização do TIM2
    NVIC_EnableIRQ(TIM2_IRQn);   // Ativa a interrupção no NVIC
}

// Interrupção do Timer 2 para controle do modo demo
void TIM2_IRQHandler(void){
    if (TIM2->SR & TIM_SR_UIF){
        TIM2->SR &= ~TIM_SR_UIF; // Limpa a flag de interrupção
        updateColorSequence();   // Atualiza a sequência de cores no modo demo
    }
}

// Interrupção da USART1 para tratar a recepção de dados
int i = 0;
void USART1_IRQHandler(void){
    if ((USART1->SR & USART_SR_RXNE)){ // Verifica se um caractere foi recebido
        ch = USART1->DR; // Lê o caractere recebido
        uart_write(ch);  // Envia de volta o caractere recebido (eco)
        if (ch != '\r'){ // Se não for um caractere de nova linha, armazena o comando
            command[i++] = ch;
        } else { // Se for uma nova linha, processa o comando
            command[i] = '\0'; // Finaliza o comando com NULL
            MAIN_SM = 1; // Define o estado para processar o comando
            i = 0; // Reinicia o índice do comando
        }
    }
    if ((USART1->SR & USART_SR_TXE)){ // Se estiver pronto para enviar
        if (*ptr){
            USART1->DR = (*ptr & 0xFF); // Envia o próximo caractere
            ptr++;
        } else {
            USART1->CR1 &= ~USART_CR1_TXEIE; // Desabilita a interrupção de transmissão
        }
    }
}

// Configuração do Timer 3 para gerar PWM para as cores RGB
void Timer3Config(){
    RCC->APB2ENR |= (1 << 0); // Habilita o clock AFIO
    AFIO->MAPR &= (0x00 << 10); // Configuração dos pinos (A6, A7 e B0)

    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // Habilita o clock do Timer 3

    TIM3->PSC = 0; // Prescaler para definir a frequência do PWM
    TIM3->ARR = 100; // Resolução do ciclo de trabalho (0-100%)

    // Configuração dos canais de captura/compare para gerar PWM
    TIM3->CCMR1 = 0x6060; // Configuração para os canais 1 e 2 (A6 e A7)
    TIM3->CCMR2 = 0x0060; // Configuração para o canal 3 (B0)
    TIM3->CCER = 0x0111;  // Habilita os canais 1, 2, e 3
    TIM3->CR1 |= (1 << 0); // Ativa o Timer 3
}
