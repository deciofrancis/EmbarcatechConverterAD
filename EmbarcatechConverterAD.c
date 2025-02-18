#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/time.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

// Definições de pinos e constantes
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define DISPLAY_ADDR 0x3c
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13
#define BUTTON_A 5
#define BUTTON_JOY 22
#define VRX_PIN 27
#define VRY_PIN 26

// Variáveis globais para controle de estado
static volatile bool led_green_state = false; // Estado do LED verde
static volatile bool leds_enabled = true;     // Habilita/desabilita os LEDs
static volatile bool draw_extra_rect = false; // Controla se desenha retângulos extras no display

static volatile uint32_t last_interrupt_time = 0; // Último tempo de interrupção para debounce
static ssd1306_t display;                         // Estrutura do display OLED
static volatile bool display_color = true;        // Cor do display (true = branco, false = preto)

// Constantes para PWM e joystick
const float PWM_CLKDIV = 6.1;           // Divisor de clock para PWM
const uint16_t PWM_WRAP = 2048;         // Valor máximo do PWM
const uint16_t JOYSTICK_DEADZONE = 100; // Zona morta do joystick para evitar leituras sensíveis

// Protótipo da função de interrupção
static void button_irq_handler(uint gpio, uint32_t events);

// Função para inicializar todos os periféricos
void initialize_peripherals()
{
    // Inicializa o LED verde como saída
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    // Inicializa o botão A como entrada com pull-up
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    // Inicializa o botão do joystick como entrada com pull-up
    gpio_init(BUTTON_JOY);
    gpio_set_dir(BUTTON_JOY, GPIO_IN);
    gpio_pull_up(BUTTON_JOY);

    // Inicializa o I2C para comunicação com o display OLED
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED
    ssd1306_init(&display, WIDTH, HEIGHT, false, DISPLAY_ADDR, I2C_PORT);
    ssd1306_config(&display);
    ssd1306_send_data(&display);
    ssd1306_fill(&display, false); // Limpa o display
    ssd1306_send_data(&display);

    // Configura o PWM para o LED vermelho
    gpio_set_function(LED_RED, GPIO_FUNC_PWM);
    uint slice_red = pwm_gpio_to_slice_num(LED_RED);
    pwm_set_clkdiv(slice_red, PWM_CLKDIV);
    pwm_set_wrap(slice_red, PWM_WRAP);
    pwm_set_enabled(slice_red, true);

    // Configura o PWM para o LED azul
    gpio_set_function(LED_BLUE, GPIO_FUNC_PWM);
    uint slice_blue = pwm_gpio_to_slice_num(LED_BLUE);
    pwm_set_clkdiv(slice_blue, PWM_CLKDIV);
    pwm_set_wrap(slice_blue, PWM_WRAP);
    pwm_set_enabled(slice_blue, true);

    // Inicializa o ADC para leitura do joystick
    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);
}

// Função principal
int main()
{
    // Inicializa todas as funcionalidades padrão
    stdio_init_all();
    initialize_peripherals();

    // Configura as interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_JOY, GPIO_IRQ_EDGE_FALL, true, &button_irq_handler);

    // Variáveis para controle do PWM dos LEDs
    int pwm_red = 0;
    int pwm_blue = 0;
    const uint16_t center_value = 2048; // Valor central do joystick

    // Loop principal
    while (true)
    {
        // Lê o valor do eixo X do joystick
        adc_select_input(1);
        uint16_t x_value = adc_read();

        // Lê o valor do eixo Y do joystick
        adc_select_input(0);
        uint16_t y_value = adc_read();

        // Se os LEDs estiverem habilitados, ajusta o PWM com base no joystick
        if (leds_enabled)
        {
            int x_deviation = abs((int)x_value - center_value);
            int y_deviation = abs((int)y_value - center_value);

            if (x_deviation > JOYSTICK_DEADZONE)
            {
                pwm_red = x_deviation;
            }
            else
            {
                pwm_red = 0;
            }

            if (y_deviation > JOYSTICK_DEADZONE)
            {
                pwm_blue = y_deviation;
            }
            else
            {
                pwm_blue = 0;
            }
        }
        else
        {
            pwm_red = 0;
            pwm_blue = 0;
        }

        // Aplica os valores de PWM aos LEDs
        pwm_set_gpio_level(LED_RED, pwm_red);
        pwm_set_gpio_level(LED_BLUE, pwm_blue);

        // Desenha no display OLED
        ssd1306_fill(&display, !display_color);
        ssd1306_rect(&display, 3, 3, 122, 58, display_color, !display_color);

        // Se necessário, desenha retângulos extras
        if (draw_extra_rect)
        {
            ssd1306_rect(&display, 4, 4, 120, 56, display_color, !display_color);
            ssd1306_rect(&display, 5, 5, 118, 54, display_color, !display_color);
        }

        // Calcula a posição do cursor no display com base no joystick
        int pos_x = ceil(x_value / 38.0);
        int pos_y = ceil(y_value / 88.0);

        // Desenha um pequeno quadrado na posição calculada
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                ssd1306_pixel(&display, pos_x + 5 + j, 48 - pos_y + 5 + i, display_color);
            }
        }
        ssd1306_send_data(&display);
        sleep_ms(10); // Pequeno delay
    }
    return 0;
}

// Função de interrupção para os botões
static void button_irq_handler(uint gpio, uint32_t events)
{
    // Verifica se os botões foram pressionados
    bool button_a_pressed = !gpio_get(BUTTON_A);
    bool joystick_button_pressed = !gpio_get(BUTTON_JOY);
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Debounce: evita múltiplas interrupções em um curto período
    if (current_time - last_interrupt_time > 300000)
    {
        if (button_a_pressed)
        {
            last_interrupt_time = current_time;
            leds_enabled = !leds_enabled; // Alterna o estado dos LEDs
        }
        else if (joystick_button_pressed)
        {
            last_interrupt_time = current_time;
            draw_extra_rect = !draw_extra_rect;   // Alterna o desenho de retângulos extras
            led_green_state = !led_green_state;   // Alterna o estado do LED verde
            gpio_put(LED_GREEN, led_green_state); // Atualiza o LED verde
        }
    }
}