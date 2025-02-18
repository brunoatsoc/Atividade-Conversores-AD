// Bibliotecas que são usadas no programa
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
// Fim includes

#define VRX_PIN 26 // Pino Joystick
#define VRY_PIN 27 // Pino Joystick
#define JB_PIN 22 // Pino do botão do Joystick
#define LED_RED_PIN 13 // Pino do led vermelho
#define LED_GREEN_PIN 11 // Pino do led verde
#define LED_BLUE_PIN 12 // Pino do led azul
#define BUTTON_A_PIN 5 // Pino do botão A
#define I2C_SDA 14 // Pino do display
#define I2C_SCL 15 // Pino do display
#define endereco 0x3C // Enderenço I2C
#define I2C_PORT i2c1 // Definição da interface
#define LIMIT 200 // Limite do Joystick
#define CENTER 2048 // Centro do Joystick

// Definição das funções que foram criadas para o programa
void initialize_pwm(uint pin, uint wrap);
void initialize_led(uint pin);
void initialize_adc(uint pin);
void initialize_button(uint pin);
void initialize_i2c();
void initialize_display(ssd1306_t* ssd);
void gpio_irq_handler(uint gpio, uint32_t events);
int map_value(int value, int min_input, int max_input, int min_output, int max_output);
void draw_border(ssd1306_t* ssd);
// Fim declaração das funções

// Constantes usadas no programa
uint32_t volatile LAST_TIME = 0;
bool volatile EDGE = false;
bool volatile LED = true;
const uint WRAP = 4095;
// Fim constantes

// Função principal
int main() {
    stdio_init_all(); // inicializa a entrada e saída padrão

    initialize_pwm(LED_BLUE_PIN, WRAP); // Inicializa o led azul como PWM
    initialize_pwm(LED_RED_PIN, WRAP); // Inicializa o led vermelho como PWM

    initialize_adc(VRX_PIN); // Inicializa o Joystick como ADC
    initialize_adc(VRY_PIN); // Inicializa o Joystick como ADC

    initialize_button(JB_PIN); // Inicializa o botão do Joystick
    gpio_set_irq_enabled_with_callback(JB_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Configura a interrupção do botão do Joystick
    initialize_button(BUTTON_A_PIN); // Inicializa o botão A
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Configura a interrupção do botão A

    initialize_led(LED_GREEN_PIN); // Inicializa o led verde

    initialize_i2c(); // Inicializa o I2C

    // Confugura o display
    ssd1306_t ssd;
    initialize_display(&ssd);

    uint16_t adc_value_x, adc_value_y;
    uint red_pwm, blue_pwm;

    while(1) {
        adc_select_input(1);
        adc_value_x = adc_read(); // Pga o valor do adc para o eixo x
        adc_select_input(0);
        adc_value_y = adc_read(); // Pega o valor do adc para o eixo y

        // Seta o pwm como zero para cada iteração
        red_pwm = 0;
        blue_pwm = 0;

        // Lógica para a movimentação do joystick
        if(LED) {
            if (adc_value_y < CENTER - LIMIT) {
                // Pega a posição do joystick para mudar a intensidade do led azul
                blue_pwm = map_value(adc_value_y, 0, CENTER - LIMIT, WRAP, 0);
            } else if (adc_value_y > CENTER + LIMIT) {
                blue_pwm = map_value(adc_value_y, CENTER + LIMIT, WRAP, 0, WRAP);
            }

            if (adc_value_x < CENTER - LIMIT) {
                // Pega a posição do joystick para mudar a intensidade do led vermelho
                red_pwm = map_value(adc_value_x, 0, CENTER - LIMIT, WRAP, 0);
            } else if (adc_value_x > CENTER + LIMIT) { 
                red_pwm = map_value(adc_value_x, CENTER + LIMIT, WRAP, 0, WRAP);
            }
        }

        // Pega os valores de x e y do joystick
        uint x = map_value(adc_value_x, 0, 4095, 0, 119);
        uint y = map_value(adc_value_y, 0, 4095, 55, 0);

        // Valor maximo e minimo de x
        if (x == 118) {
            x = 116;
        } else if (x == 0) {
            x = 4;
        }

        // Vaçor maiximo minimo de y
        if (y == 55) {
            y = 52;
        } else if (y == 1) {
            y = 4;
        }

        // Limpa a tela do display
        ssd1306_fill(&ssd, false);

        // Imprime a borda nas laterais do display
        if (EDGE) {
            draw_border(&ssd); // Borda pontilhada
        } else {
            ssd1306_rect(&ssd, 0, 0, 128, 64, true, false); // Borda normal
        }

        // Desenha o quadrado na posição de x e y que foram calculados anteriormente
        ssd1306_rect(&ssd, y, x, 8, 8, true, true);
        ssd1306_send_data(&ssd);

        // Ajusta a intensidade dos leds vermelho e azul
        pwm_set_gpio_level(LED_BLUE_PIN, blue_pwm);
        pwm_set_gpio_level(LED_RED_PIN, red_pwm);

        sleep_ms(100);
    }

    return 0;
} // Fim main

// Função para inicializar o PWM
void initialize_pwm(uint pin, uint wrap) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice, wrap);
    pwm_set_enabled(slice, true);
} // Fim initialize_pwm

// Função para inicializar um led
void initialize_led(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
} // Fim initialize_led

// Função para incializar o ADC
void initialize_adc(uint pin) {
    adc_init();
    adc_gpio_init(pin);
} // Fim initialize_adc

// Função apra inicializar um botão
void initialize_button(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
} // Fim initialize_button

// Função para inicializar o I2C
void initialize_i2c() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
} // initialize_i2c

// Função para configurar o display
void initialize_display(ssd1306_t* ssd) {
    ssd1306_init(ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_send_data(ssd);
} // Fim initialize_display

// Função para trabalhar as interrupçãoes dos botões
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - LAST_TIME > 250) {
        LAST_TIME = current_time;

        if(gpio == JB_PIN) {
            gpio_put(LED_GREEN_PIN, !gpio_get(LED_GREEN_PIN));
            EDGE = !EDGE;
        }
        
        if(gpio == BUTTON_A_PIN) {
            LED = !LED;
        }
    }
} // Fim gpio_irq_handler

// Função para calcular a posição do joystick
int map_value(int value, int min_input, int max_input, int min_output, int max_output) {
    return (value - min_input) * (max_output - min_output) / (max_input - min_input) + min_output;
} // Fim map_value

// Função para desennhar a borda pontilhada na tela do display
void draw_border(ssd1306_t* ssd) {
    for (int i = 0; i <= 127; i += 4) {
        ssd1306_pixel(ssd, i, 0, true); // Borda do topo
        ssd1306_pixel(ssd, i, 63, true); // Borda inferior
    }
    for (int i = 0; i <= 63; i += 4) {
        ssd1306_pixel(ssd, 0, i, true); // Borda esquerda
        ssd1306_pixel(ssd, 127, i, true); // Borda direita
    }
    
    // Desenha um traço a mais na quina inferior direita do display
    ssd1306_pixel(ssd, 127, 63, true);
} // Fim draw_border
