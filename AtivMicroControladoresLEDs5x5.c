#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "pio_matrix.pio.h"

#define ROWS 4
#define COLS 4
uint8_t row_pins[ROWS] = {2, 3, 4, 5};  
uint8_t col_pins[COLS] = {19, 18, 17, 16};  


#define BUZZER_PIN 21
#define NUM_PIXELS 25
#define OUT_PIN 7

static PIO pio = pio0;
static uint sm;
static uint offset;

const char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Mapeamento da posição lógica para a posição física (em zig-zag)
int mapa_leds[25] = {
    24, 23, 22, 21, 20,  // Linha 1: direita -> esquerda
    15, 16, 17, 18, 19,  // Linha 2: esquerda -> direita
    14, 13, 12, 11, 10,  // Linha 3: direita -> esquerda
     5,  6,  7,  8,  9,  // Linha 4: esquerda -> direita
     4,  3,  2,  1,  0   // Linha 5: direita -> esquerda
};

//leds 5x5 ligados
double ledsLIGADOS[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

void init_gpio(void);
char scan_keypad(void);
uint32_t matrix_rgb(double r, double g, double b);
void desenho_pio(double *desenho, PIO pio, uint sm, double r, double g, double b);

void handle_key_press(char key) {
    switch (key) {
        case '0':
            //animação 1
            break;
        case '1':
            //animação 2
            break;
        case '2':
            //animação 3
            break;
        case '3':
            //animação 4
            break;            
        case '4':
            //animação 5
            break;         
        case '5':
            //animação 6
            break;
        case '6':
            //animação 7
            break;
        case 'A':
           //Desligar todos os LEDs
            break;
        case 'B':
            //Ligar todos os LEDs na cor azul, 100% de intensidade
            break;
        case 'C':
            //Ligar todos os LEDs na cor vermelha, 80% de intensidade
            break;
        case 'D':
            //Ligar todos os LEDs na cor verde, 50% de intensidade
            break;
        case '#':
            //Ligar todos os LEDs na cor branca, 20% de intensidade
            break;
        case '*':
            //reboot
            printf("Reiniciando...\n");
            reset_usb_boot(0, 0);
            break;
        default:
            printf("Tecla não atribuída: %c\n", key);
            break;
    }
}
// Função principal
int main() {
    sm = pio_claim_unused_sm(pio, true);
    offset = pio_add_program(pio, &pio_matrix_program);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    stdio_init_all();
    init_gpio();
    printf("Iniciando...\n");

    char last_key = 0;

    // Loop principal
    while (true) {
        char key = scan_keypad();
        if (key != 0 && key != last_key) {
            printf("Tecla pressionada: %c\n", key);
            handle_key_press(key);
            last_key = key;
        }
        sleep_ms(100); // Aguarda 100 ms
    }
}

void init_gpio() {
   
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], 1);
    }

    for (int i = 0; i < COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_up(col_pins[i]);
    }

    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0); 
}

char scan_keypad() {
    for (int row = 0; row < ROWS; row++) {
        gpio_put(row_pins[row], 0); 
        for (int col = 0; col < COLS; col++) {
            if (gpio_get(col_pins[col]) == 0) { 
                gpio_put(row_pins[row], 1);
                return keys[row][col]; 
            }
        }
        gpio_put(row_pins[row], 1); 
    }
    return 0; 
}

// Função para definir a cor dos LEDs
uint32_t matrix_rgb(double r, double g, double b) {
    return ((uint8_t)(g * 255) << 24) | ((uint8_t)(r * 255) << 16) | ((uint8_t)(b * 255) << 8);
}

// Função para exibir o padrão na matriz de LEDs
void desenho_pio(double *desenho, PIO pio, uint sm, double r, double g, double b) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        int posicao_fisica = mapa_leds[i]; // Aplica o mapeamento
        uint32_t valor_led = matrix_rgb(desenho[posicao_fisica] * r, desenho[posicao_fisica] * g, desenho[posicao_fisica] * b);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}
