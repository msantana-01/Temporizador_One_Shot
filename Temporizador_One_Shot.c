#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Definição dos pinos
#define PINO_LED_AZUL 12
#define PINO_LED_VERMELHO 13
#define PINO_LED_VERDE 11
#define PINO_BOTAO 5

// Estados dos LEDs
typedef enum {
    TODOS_LEDS_LIGADOS,
    DOIS_LEDS_LIGADOS,
    UM_LED_LIGADO,
    TODOS_LEDS_DESLIGADOS
} EstadoLEDs;

// Variáveis globais
EstadoLEDs estado_atual = TODOS_LEDS_DESLIGADOS;
bool botao_pressionado = false;
bool temporizador_rodando = false;

// Função para debounce do botão
bool debounce_botao() {
    static uint32_t ultimo_tempo = 0;
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (tempo_atual - ultimo_tempo < 50) { // 50ms para debounce
        return false;
    }

    ultimo_tempo = tempo_atual;
    return true;
}

// Função para ligar/desligar os LEDs
void controlar_leds(bool azul, bool vermelho, bool verde) {
    gpio_put(PINO_LED_AZUL, azul);
    gpio_put(PINO_LED_VERMELHO, vermelho);
    gpio_put(PINO_LED_VERDE, verde);
}

// Callback para desligar os LEDs
int64_t callback_desligar_leds(alarm_id_t id, void *user_data) {
    switch (estado_atual) {
        case TODOS_LEDS_LIGADOS:
            controlar_leds(false, true, true); // Dois LEDs ligados
            estado_atual = DOIS_LEDS_LIGADOS;
            break;
        case DOIS_LEDS_LIGADOS:
            controlar_leds(false, false, true); // Um LED ligado
            estado_atual = UM_LED_LIGADO;
            break;
        case UM_LED_LIGADO:
            controlar_leds(false, false, false); // Todos os LEDs desligados
            estado_atual = TODOS_LEDS_DESLIGADOS;
            temporizador_rodando = false; // Finaliza a temporização
            break;
        default:
            break;
    }

    if (estado_atual != TODOS_LEDS_DESLIGADOS) {
        add_alarm_in_ms(3000, callback_desligar_leds, NULL, false); // Novo alarme de 3 segundos
    }

    return 0;
}

// Função principal
int main() {
    //stdio_init_all();

    // Configuração dos pinos dos LEDs como saída
    gpio_init(PINO_LED_AZUL);
    gpio_init(PINO_LED_VERMELHO);
    gpio_init(PINO_LED_VERDE);
    gpio_set_dir(PINO_LED_AZUL, GPIO_OUT);
    gpio_set_dir(PINO_LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(PINO_LED_VERDE, GPIO_OUT);

    // Configuração do pino do botão como entrada
    gpio_init(PINO_BOTAO);
    gpio_set_dir(PINO_BOTAO, GPIO_IN);
    gpio_pull_up(PINO_BOTAO); // Ativa o resistor de pull-up

    while (true) {
        if (!gpio_get(PINO_BOTAO) && debounce_botao() && !temporizador_rodando) {
            botao_pressionado = true;
        }

        if (botao_pressionado && estado_atual == TODOS_LEDS_DESLIGADOS) {
            controlar_leds(true, true, true); // Liga todos os LEDs
            estado_atual = TODOS_LEDS_LIGADOS;
            temporizador_rodando = true;
            add_alarm_in_ms(3000, callback_desligar_leds, NULL, false); // Inicia o temporizador
            botao_pressionado = false;
        }

        sleep_ms(10); // Pequeno delay para evitar leituras excessivas
    }

    return 0;
}