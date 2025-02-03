#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// Definição dos pinos GPIO
#define LED_AZUL 11
#define LED_VERMELHO 12
#define LED_VERDE 13
#define BOTÃO 05

// Estados dos LEDs
typedef enum {
    LIGADO,
    DESLIGADO
} estado_led_t;

// Variáveis para controlar o estado dos LEDs e do botão
estado_led_t led_azul_estado = DESLIGADO;
estado_led_t led_vermelho_estado = DESLIGADO;
estado_led_t led_verde_estado = DESLIGADO;
bool botao_pressionado = false;
int led_atual = 0; // 0: azul, 1: vermelho, 2: verde

// Função para atualizar os LEDs
void atualizar_leds() {
    gpio_put(LED_AZUL, led_azul_estado == LIGADO);
    gpio_put(LED_VERMELHO, led_vermelho_estado == LIGADO);
    gpio_put(LED_VERDE, led_verde_estado == LIGADO);
}

// Função de call-back do temporizador
void turn_off_callback(struct alarm_pool *p, void *data) {
    switch (led_atual) {
        case 0:
            led_azul_estado = DESLIGADO;
            break;
        case 1:
            led_vermelho_estado = DESLIGADO;
            break;
        case 2:
            led_verde_estado = DESLIGADO;
            break;
    }

    atualizar_leds();

    led_atual++;
    if (led_atual > 2) {
        led_atual = 0;
        botao_pressionado = false; // Permite novo acionamento do botão
    } else {
        // Agenda o próximo desligamento após 3 segundos
        add_alarm_in_ms(3000, turn_off_callback, NULL, NULL);
    }
}

int main() {
    stdio_init_all();

    // Inicialização dos pinos GPIO
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(BOTÃO);
    gpio_set_dir(BOTÃO, GPIO_IN);
    gpio_pull_up(BOTÃO);

    // Loop principal
    while (true) {
        // Verifica se o botão foi pressionado e se não está em estado de temporização
        if (gpio_get(BOTÃO) == 0 && !botao_pressionado) {
            botao_pressionado = true;
            led_atual = 0; // Reinicia o contador de LEDs

            // Liga os LEDs
            led_azul_estado = LIGADO;
            led_vermelho_estado = LIGADO;
            led_verde_estado = LIGADO;
            atualizar_leds();

            // Agenda o primeiro desligamento (LED azul) após 3 segundos
            add_alarm_in_ms(3000, turn_off_callback, NULL, NULL);
        }

        // Imprime informações a cada segundo (opcional)
        sleep_ms(1000);
        printf("Informação: Tempo = %d segundos\n", time_us_64() / 1000000);
    }

    return 0;
}