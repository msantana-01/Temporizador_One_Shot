# Controle de LEDs com Raspberry Pi Pico W

Este projeto implementa um sistema de controle de LEDs utilizando um botão no Raspberry Pi Pico W. Ao pressionar o botão, todos os LEDs acendem e, após um tempo, vão apagando gradativamente até que todos se apaguem. O botão só altera o estado dos LEDs quando o último LED é desligado.

## Funcionamento

1. O botão é pressionado e, após o debounce, ativa todos os LEDs.
2. Um temporizador (`add_alarm_in_ms()`) é iniciado para desligar os LEDs em estágios.
3. Após 3 segundos, um LED se apaga, e assim sucessivamente até que todos estejam apagados.
4. O ciclo só pode ser reiniciado após todos os LEDs estarem apagados.

## Principais funções utilizadas

### `add_alarm_in_ms()`

Esta função da biblioteca `hardware/timer.h` permite agendar a execução de uma função após um determinado tempo em milissegundos. No código, ela é usada para criar um temporizador que controla o desligamento gradual dos LEDs.

Exemplo de uso no código:

```c
add_alarm_in_ms(3000, callback_desligar_leds, NULL, false);
```

Isso agenda a execução da função `callback_desligar_leds()` após 3 segundos.

### `callback_desligar_leds()`

Essa função é chamada pelo temporizador e reduz a quantidade de LEDs acesos a cada 3 segundos até que todos sejam desligados.

### `debounce_botao()`

O botão pode gerar múltiplos sinais elétricos ao ser pressionado, resultando em leituras incorretas. Para evitar isso, a função `debounce_botao()` verifica se já passou um tempo mínimo desde a última ativação antes de considerar o botão pressionado.

```c
bool debounce_botao() {
    static uint32_t ultimo_tempo = 0;
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (tempo_atual - ultimo_tempo < 50) { // 50ms para debounce
        return false;
    }

    ultimo_tempo = tempo_atual;
    return true;
}
```

## Demonstração no Wokwi

<https://wokwi.com/projects/421628509625375745>

## Demonstração na Bitdoglab

https://youtu.be/5Nkw3mb48y8
