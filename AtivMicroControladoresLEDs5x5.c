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

double LEDsAzul[25]={
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
void hsv_to_rgb(float h, float s, float v, float *r, float *g, float *b) ;
void arco_iris_dinamico_iterativo(PIO pio, uint sm, float *hue_base);
void ligar_leds_verde(PIO pio, uint sm);
void ligar_leds_branco(PIO pio, uint sm);
void animacao_quadrado(PIO pio, uint sm) ;
void piscar_X(PIO pio, uint sm);


void handle_key_press(char key) {
    static float hue_base = 0.0; // Base para arco-íris

    switch (key) {
        case '0':
            //animação 1
            double desenho[25] = {0};
            desenho[12] = 1.0; // Apenas o LED central
            desenho_pio(desenho, pio, sm, 1.0, 1.0, 1.0); // Cor branca
            sleep_ms(1000);
            break;
        case '1':
            //animação 2
            break;
        case '2':
            //animação 3
            break;
        case '3':
            //animação 4
            animacao_quadrado(pio, sm);
            break;            
        case '4':
            //animação 5
            piscar_X(pio, sm);
            break;         
        case '5':
            //animação 6
            break;
        case '6':
            printf("Iniciando arco-íris dinâmico...\n");
            arco_iris_dinamico_iterativo(pio, sm, &hue_base);
            break;
        case '7':
            //animação 7
            break;
        case '8':
            //animação 8
            break;
        case '9':
            //animação 9
            break;

        case 'A':
           //Desligar todos os LEDs
            break;
        case 'B':
            //Ligar todos os LEDs na cor azul, 100% de intensidade
            desenho_pio(LEDsAzul, pio, sm, 0.0, 0.0, 1.0);
            break;
        case 'C':
            //Ligar todos os LEDs na cor vermelha, 80% de intensidade
            break;
        case 'D':
            //Ligar todos os LEDs na cor verde, 50% de intensidade
            ligar_leds_verde(pio, sm);
            break;
        case '#':
            //Ligar todos os LEDs na cor branca, 20% de intensidade
            ligar_leds_branco(pio, sm);
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

void hsv_to_rgb(float h, float s, float v, float *r, float *g, float *b) {
    int i = (int)(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
        case 0: *r = v; *g = t; *b = p; break;
        case 1: *r = q; *g = v; *b = p; break;
        case 2: *r = p; *g = v; *b = t; break;
        case 3: *r = p; *g = q; *b = v; break;
        case 4: *r = t; *g = p; *b = v; break;
        case 5: *r = v; *g = p; *b = q; break;
    }
}

void arco_iris_dinamico_iterativo(PIO pio, uint sm, float *hue_base) {
    float step = 1.0 / NUM_PIXELS; // Diferença de matiz entre os LEDs


    for (int i = 0; i < NUM_PIXELS; i++) {
        int posicao_fisica = mapa_leds[i]; // Obter posição física do LED
        float hue = fmod(*hue_base + i * step, 1.0); // Matiz do LED (0.0 a 1.0)
        
        // Converter HSV para RGB
        float r, g, b;
        hsv_to_rgb(hue, 1.0, 1.0, &r, &g, &b);

        // Definir cor do LED
        uint32_t cor = matrix_rgb(r, g, b);
        pio_sm_put_blocking(pio, sm, cor);
    }

    *hue_base = fmod(*hue_base + 0.01, 1.0); // Incrementar matiz base

}

void ligar_leds_verde(PIO pio, uint sm) {
    printf("Ligando todos os LEDs na cor verde (50%% de intensidade)...\n");
    desenho_pio(ledsLIGADOS, pio, sm, 0.0, 0.5, 0.0);
}

void ligar_leds_branco(PIO pio, uint sm) {
    printf("Ligando todos os LEDs na cor branca (20%% de intensidade)...\n");
    desenho_pio(ledsLIGADOS, pio, sm, 0.2, 0.2, 0.2);
}

void animacao_quadrado(PIO pio, uint sm) {
    double desenho[25] = {0};
    printf("Executando animação de quadrado crescendo...\n");

    // Camadas do quadrado crescendo
    int camadas[4][25] = {
        {12},                                  // Ponto central (primeiro frame)
        {6, 8, 12, 16, 18},                   // Segundo frame com "X"
        {0, 4, 20, 24, 2, 10, 14, 22, 6, 8, 16, 18, 12}, // Quadrado maior com "X"
        {0, 1, 2, 3, 4, 5, 9, 10, 14, 15, 19, 20, 21, 22, 23, 24} // Quadrado completo
    };

    // Cores para cada camada (mudança de cor por frame)
    float cores[4][3] = {
        {1.0, 0.0, 0.0}, // Vermelho
        {0.0, 1.0, 0.0}, // Verde
        {0.0, 0.0, 1.0}, // Azul
        {1.0, 1.0, 0.0}  // Amarelo
    };

    // Executa cada frame
    for (int frame = 0; frame < 4; frame++) {
        // Desliga todos os LEDs antes de configurar o próximo frame
        for (int i = 0; i < NUM_PIXELS; i++) {
            pio_sm_put_blocking(pio, sm, 0x000000); // Envia o comando para desligar
        }

        sleep_ms(50); // Adiciona um pequeno delay para sincronização de hardware

        // Zera o array 'desenho'
        memset(desenho, 0, sizeof(desenho));

        // Configura os LEDs da camada atual
        for (int i = 0; i < 25; i++) {
            int idx = camadas[frame][i];
            if (idx >= 0 && idx < 25) {
                desenho[idx] = 1.0;
            }
        }

        // Atualiza a matriz com a cor correspondente
        desenho_pio(desenho, pio, sm, cores[frame][0], cores[frame][1], cores[frame][2]);

        sleep_ms(200); // Tempo entre os frames
    }
}

void piscar_X(PIO pio, uint sm) {
    double desenho[25] = {0};

    // Definir o "X" nos LEDs
    desenho[0] = desenho[4] = desenho[6] = desenho[8] = desenho[12] = desenho[16] = desenho[18] = desenho[20] = desenho[24] = 1.0;

    printf("Transição do X com 10 cores diferentes...\n");

    float cores[10][3] = {
        {1.0, 0.0, 0.0}, // Vermelho
        {0.0, 1.0, 0.0}, // Verde
        {0.0, 0.0, 1.0}, // Azul
        {1.0, 1.0, 0.0}, // Amarelo
        {1.0, 0.0, 1.0}, // Magenta
        {0.0, 1.0, 1.0}, // Ciano
        {1.0, 0.5, 0.0}, // Laranja
        {0.5, 0.0, 0.5}, // Roxo
        {0.5, 0.5, 0.5}, // Cinza
        {1.0, 1.0, 1.0}  // Branco
    };

    // Itera pelas cores, criando transições suaves
    for (int frame = 0; frame < 10; frame++) {
        float *cor_atual = cores[frame % 10]; // Cor inicial
        float *cor_proxima = cores[(frame + 1) % 10]; // Próxima cor

        // Transição suave entre as cores
        for (float t = 0.0; t <= 1.0; t += 0.1) { // 10 passos para a transição
            float r = cor_atual[0] * (1.0 - t) + cor_proxima[0] * t;
            float g = cor_atual[1] * (1.0 - t) + cor_proxima[1] * t;
            float b = cor_atual[2] * (1.0 - t) + cor_proxima[2] * t;

            // Atualiza o desenho com as cores interpoladas
            desenho_pio(desenho, pio, sm, r, g, b);
            sleep_ms(50); // Pequeno delay para suavizar a transição
        }
    }

    // Apaga o "X" ao final
    for (int i = 0; i < 25; i++) {
        desenho[i] = 0.0;
    }
    desenho_pio(desenho, pio, sm, 0.0, 0.0, 0.0); // Desliga os LEDs
}
