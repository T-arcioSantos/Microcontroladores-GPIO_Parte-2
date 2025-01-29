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
uint8_t row_pins[ROWS] = {16, 17, 18, 19};  
uint8_t col_pins[COLS] = {20, 4, 9, 8};  
  
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

double ledsDesligados[25] = {0.0};

void init_gpio(void);
char scan_keypad(void);
uint32_t matrix_rgb(double r, double g, double b);
void desenho_pio(double *desenho, PIO pio, uint sm, double r, double g, double b);
void activate_buzzer(uint32_t frequency, uint32_t duration_ms);
void handle_key_press(char key);

void hsv_to_rgb(float h, float s, float v, float *r, float *g, float *b) ;
void arco_iris_dinamico_iterativo(PIO pio, uint sm, float *hue_base);
void ligar_leds_verde(PIO pio, uint sm);
void ligar_leds_branco(PIO pio, uint sm);
void animacao_quadrado(PIO pio, uint sm) ;
void escrever_nordeste(PIO pio, uint sm);
void xadrez(PIO pio, uint sm);
void animacao1();
void animacao3(PIO pio, uint sm); // Feito por Cintia
void animacao10(PIO pio, uint sm);

void onda_de_cores(PIO pio, uint sm);
void explosao_radial(PIO pio, uint sm, float *hue_base);
void ligar_leds_azul(PIO pio, uint sm);
void desligar_leds(PIO pio, uint sm);
void animacao6(PIO pio, uint sm);

int main() {
    sm = pio_claim_unused_sm(pio, true);
    offset = pio_add_program(pio, &pio_matrix_program);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    stdio_init_all();
    init_gpio();
    printf("Iniciando...\n");


    char last_key = 11;
    int key_released = 1;

    // Loop principal
    while (true) {
        char key = scan_keypad();

        if (key != 11 && key != last_key) {
            printf("Tecla pressionada: %c\n", key);
            handle_key_press(key);
            last_key = key;
            key_released = 0;
        }
        else {
            key_released = 1;
            last_key = 11;
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

void handle_key_press(char key) {
    static float hue_base = 0.0; // Base para arco-íris
    switch (key) {
        case '0':
            //animação 1
            printf("Iniciando contagem regressiva e coração...\n");
            animacao1(); 
            break;
        case '1':
           printf("Onda de Cores \n");
            onda_de_cores(pio, sm);
            printf("Onda de Cores finalizada.\n");
            break;
        case '2':
            //animação 3
            printf("Iniciando linha coluna colorida...\n");
            animacao3(pio, sm);
            printf("Linha coluna colorida finalizada.\n");
            break;
        case '3':
            //animação 4
            printf("Iniciando quadrado crescendo...\n");
            animacao_quadrado(pio, sm);
            printf("Quadrado crescendo finalizado.\n");
            break;            
        case '4':
            //animação 5
            printf("Escrevendo a frase 'I S2 NORDESTE'...\n");
            escrever_nordeste(pio, sm);
            printf("Frase 'I S2 NORDESTE' finalizada.\n");
            break;         
        case '5':
            //animação 6
            printf("Iniciando espiral...\n");
            animacao6(pio, sm);
            printf("Espiral finalizado.\n");
            break;
        case '6':
            //animação 7
            printf("Iniciando arco-íris dinâmico...\n");
            arco_iris_dinamico_iterativo(pio, sm, &hue_base);
            desenho_pio(ledsDesligados, pio, sm, 0.0, 0.0, 0.0);
            printf("Arco-íris dinâmico finalizado.\n");
            break;
        case '7':
            //animação 8
         printf("Iniciando xadrez...\n");
         xadrez(pio, sm);
            printf("Xadrez finalizado.\n");
            break;
        case '8':
            printf("Iniciando Explosão Radial...\n");
            explosao_radial(pio, sm, &hue_base);
            printf("Explosão Radial finalizada.\n");
            break;
        case '9':
            //animação 10
            printf("Iniciando movimento cardinal...\n");
            animacao10(pio, sm);
            printf("Movimento cardinal finalizado.\n");
            break;
        case 'A':
            //Desligar todos os LEDs
            desligar_leds(pio, sm);
            break;
        case 'B':
            //Ligar todos os LEDs na cor azul, 100% de intensidade
            ligar_leds_azul(pio, sm);
            break;
        case 'C':
            //Ligar todos os LEDs na cor vermelha, 80% de intensidade
             desenho_pio(ledsLIGADOS, pio, sm, 0.8, 0.0, 0.0); // LEDs vermelhos, 80%
             printf("Todos os LEDs ligados na cor vermelha com 80%% de intensidade\n");
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

char scan_keypad() {
    for (int row = 0; row < ROWS; row++) {
        gpio_put(row_pins[row], 0); // Ativa a linha atual (LOW)
       
        for (int col = 0; col < COLS; col++) {
            // printf("Varrendo coluna %d (GPIO%d)\n", col, col_pins[col]);
            if (gpio_get(col_pins[col]) == 0) { // Verifica se a coluna está LOW
               
                gpio_put(row_pins[row], 1); // Desativa a linha antes de retornar
                gpio_put(col_pins[col],1); // Desativa a coluna antes de retornar
                return keys[row][col]; // Retorna a tecla pressionada
            }
        }
        gpio_put(row_pins[row], 1); 
        // Desativa a linha após a varredura
    }
    return 11; // Retorna 11 se nenhuma tecla for pressionada
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

void activate_buzzer(uint32_t frequency, uint32_t duration_ms) {
    uint32_t period_us = 1000000 / frequency; // Calcula o período em microssegundos
    uint32_t half_period_us = period_us / 2; // Calcula metade do período para o ciclo ativo/inativo

    uint32_t start_time = time_us_64();
    while (time_us_64() - start_time < duration_ms * 1000) {
        gpio_put(BUZZER_PIN, 1); // Liga o buzzer
        busy_wait_us(half_period_us); // Espera metade do período
        gpio_put(BUZZER_PIN, 0); // Desliga o buzzer
        busy_wait_us(half_period_us); // Espera a outra metade do período
    }
}

// ANIMAÇÕES ////////////////////
// Números de 1 a 5
double numero1[25] = {
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 1.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 1.0, 1.0, 1.0, 0.0
};

double numero2[25] = {
    0.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 1.0, 1.0, 0.0,
    0.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double numero3[25] = {
    0.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 1.0, 1.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 1.0, 1.0, 1.0, 0.0
};

double numero4[25] = {
    0.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 1.0, 1.0, 0.0,
    0.0, 1.0, 0.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 1.0, 0.0
};

double numero5[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 0.0
};

double desenho6[25] = { 
    0.0, 1.0, 0.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0
};

// Array de ponteiros para os números
double* numeros[] = {numero5, numero4, numero3, numero2, numero1};

// -- animação 1 -- contagem regressiva e coração Tecla 0 -- //
void animacao1(){
   // Contagem regressiva usando loop
    for(int i = 0; i < 5; i++) {
        desenho_pio(numeros[i], pio, sm, 1.0, 0.0, 0.0);
        sleep_ms(1000);
    }
    desenho_pio(desenho6, pio, sm, 1.0, 0.0, 0.0);
    activate_buzzer(200, 300);
    sleep_ms(900);
    desenho_pio(ledsDesligados, pio, sm, 0.0, 0.0, 0.0);
};


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

// -- animação 2 -- Tecla 1 -- Onda de Cores //


// Frames da Onda de Cores (expansão e contração) - Animação 2 Tecla 1 
double onda_frame0[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double onda_frame1[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double onda_frame2[25] = {
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0
};

double onda_frame3[25] = {
    0.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 1.0, 1.0, 1.0, 0.0
};

double onda_frame4[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

// Frames de contração (cópias explícitas)
double onda_frame5[25];
double onda_frame6[25];
double onda_frame7[25];
double onda_frame8[25];
double onda_frame9[25] = {0};


// Array de frames
double* onda_frames[] = {
    onda_frame0, onda_frame1, onda_frame2,
    onda_frame3, onda_frame4, onda_frame5,
    onda_frame6, onda_frame7, onda_frame8, onda_frame9
};

// Inicializa frames de contração com memcpy
void init_onda_frames() {
    memcpy(onda_frame5, onda_frame3, sizeof(onda_frame3));
    memcpy(onda_frame6, onda_frame2, sizeof(onda_frame2));
    memcpy(onda_frame7, onda_frame1, sizeof(onda_frame1));
    memcpy(onda_frame8, onda_frame0, sizeof(onda_frame0));
}

float onda_cores[10][3] = {
    {0.5, 0.0, 0.5}, // Roxo
    {0.3, 0.0, 0.7}, 
    {0.1, 0.0, 0.9}, 
    {0.0, 0.5, 1.0}, 
    {0.0, 0.8, 1.0}, 
    {0.0, 0.5, 1.0}, 
    {0.1, 0.0, 0.9}, 
    {0.3, 0.0, 0.7}, 
    {0.5, 0.0, 0.5},
    {0.0, 0.0, 0.0}  // Preto
};

uint32_t onda_frequencias[10] = {
    200, 300, 400, 500, 600, 
    500, 400, 300, 200, 0
};

void onda_de_cores(PIO pio, uint sm) {
    init_onda_frames(); // Inicializa cópias dos frames
    
    printf("Iniciando Onda de Cores (Tecla 1)\n");
    int frame = 0;
    int direcao = 1;
    int num_repeticoes = 9;

    while (num_repeticoes-- > 0) {

        if (scan_keypad() != '1' && scan_keypad() != 11) {
            desenho_pio(ledsDesligados, pio, sm, 0.0, 0.0, 0.0);
            activate_buzzer(0, 0);
            return;
        }


        // Usa o frame atual do array
        desenho_pio(
            onda_frames[frame], 
            pio, 
            sm, 
            onda_cores[frame][0],
            onda_cores[frame][1],
            onda_cores[frame][2]
        );

        // Toca o som correspondente
        activate_buzzer(onda_frequencias[frame], 100);

        // Atualiza frame
        frame += direcao;
        if (frame >= 9 || frame <= 0) direcao *= -1;

        sleep_ms(150);
 // Mantém o timing da animação
        printf("num_repeticoes: %d\n", num_repeticoes);

    }
}



void arco_iris_dinamico_iterativo(PIO pio, uint sm, float *hue_base) {
    
    float step = 1.0 / NUM_PIXELS; // Diferença de matiz entre os LEDs
    int n = 200;

    while(n-->0) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        int posicao_fisica = mapa_leds[i]; // Obter posição física do LED
        float hue = fmod(*hue_base + i * step, 1.0); // Matiz do LED (0.0 a 1.0)
        
        // Converter HSV para RGB
        float r, g, b;
        hsv_to_rgb(hue, 1.0, 1.0, &r, &g, &b);

        // Definir cor do LED
        uint32_t cor = matrix_rgb(r, g, b);
        pio_sm_put_blocking(pio, sm, cor);

        //interromper a execução ao pressionar qualquer tecla diferente de 6
        char key = scan_keypad();
        if ( key != '6' && key != 11) {
            return;
        }

    }

    *hue_base = fmod(*hue_base + 0.01, 1.0); // Incrementar matiz base
    sleep_ms(50); // Pequeno delay para suavizar a transição
    }
}

// Tecla D -- ligar todos os LEDs na cor verde na intensidade de 50% //
void ligar_leds_verde(PIO pio, uint sm) {
    printf("Ligando todos os LEDs na cor verde (50%% de intensidade)...\n");
    desenho_pio(ledsLIGADOS, pio, sm, 0.0, 0.5, 0.0);
}
// Tecla # -- ligar todos os LEDs na cor branca na intensidade de 20% //
void ligar_leds_branco(PIO pio, uint sm) {
    printf("Ligando todos os LEDs na cor branca (20%% de intensidade)...\n");
    desenho_pio(ledsLIGADOS, pio, sm, 0.2, 0.2, 0.2);
}

void ligar_leds_azul(PIO pio, uint sm) {
    printf("Ligando todos os LEDs na cor azul (100%% de intensidade)...\n");
    desenho_pio(ledsLIGADOS, pio, sm, 0.0, 0.0, 1.0);
}

void desligar_leds(PIO pio, uint sm) {
    printf("Desligando todos os LEDs...\n");
    desenho_pio(ledsDesligados, pio, sm, 0.0, 0.0, 0.0);
}

// -- animação 4 -- Tecla 3 -- quadrado crescendo //
void animacao_quadrado(PIO pio, uint sm) {
    double desenho[25] = {0};
    printf("Executando animação de quadrado crescendo...\n");

    // Camadas do quadrado crescendo
    int camadas[5][25] = {
        {12},                                  // Ponto central (primeiro frame)
        {6, 8, 12, 16, 18},                   // Segundo frame com "X"
        {0, 4, 20, 24, 2, 10, 14, 22, 6, 8, 16, 18, 12}, // Quadrado maior com "X"
        {0, 1, 2, 3, 4, 5, 9, 10, 14, 15, 19, 20, 21, 22, 23, 24}, // Quadrado completo
        {0, 4, 20, 24, 2, 10, 14, 22, 6, 8, 16, 18, 12} // Quadrado maior com "X" (repetição)
    };

    // Cores para cada camada (mudança de cor por frame)
    float cores[5][3] = {
        {1.0, 0.0, 0.0}, // Vermelho
        {0.0, 1.0, 0.0}, // Verde
        {0.0, 0.0, 1.0}, // Azul
        {1.0, 1.0, 0.0},  // Amarelo
        {1.0, 0.0, 1.0}  // Magenta
    };

    // Executa cada frame
    for (int frame = 0; frame < 5; frame++) {
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

// -- animação 5 -- Tecla 4 -- Frase "I S2 NORDESTE" //

void escrever_nordeste(PIO pio, uint sm) {
    double matriz[25] = {0}; 
    float cor[3] = {1.0, 0.0, 0.0}; 

    double letras[10][25] = {
        {1, 1, 1, 1, 1,
         0, 0, 1, 0, 0,
         0, 0, 1, 0, 0,
         0, 0, 1, 0, 0,
         1, 1, 1, 1, 1},
        {0, 1, 0, 1, 0,
         1, 1, 1, 1, 1,
         1, 1, 1, 1, 1,
         0, 1, 1, 1, 0,
         0, 0, 1, 0, 0},
        {1, 0, 0, 0, 1, 
         1, 1, 0, 0, 1,
         1, 0, 1, 0, 1,
         1, 0, 0, 1, 1,
         1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0, 
         1, 0, 0, 0, 1,
         1, 0, 0, 0, 1,
         1, 0, 0, 0, 1,
         0, 1, 1, 1, 0},
        {1, 1, 1, 1, 1,  
         1, 0, 0, 0, 1,
         1, 1, 1, 1, 0,
         1, 0, 0, 1, 0,
         1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0, 
         1, 0, 0, 0, 1,
         1, 0, 0, 0, 1,
         1, 0, 0, 0, 1,
         1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 
         1, 0, 0, 0, 0,
         1, 1, 1, 1, 1,
         1, 0, 0, 0, 0,
         1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1,  
         1, 0, 0, 0, 0,
         1, 1, 1, 1, 1,
         0, 0, 0, 0, 1,
         1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 
         0, 0, 1, 0, 0,
         0, 0, 1, 0, 0,
         0, 0, 1, 0, 0,
         0, 0, 1, 0, 0},
        {1, 1, 1, 1, 1,  
         1, 0, 0, 0, 0,
         1, 1, 1, 1, 1,
         1, 0, 0, 0, 0,
         1, 1, 1, 1, 1}
    };

    // Exibir cada letra por 2 segundos
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 25; j++) {
            matriz[j] = letras[i][j];
        }
        desenho_pio(matriz, pio, sm, cor[0], cor[1], cor[2]);
        sleep_ms(2000);
    }

    // Apaga a frase
    for (int i = 0; i < 25; i++) {
        matriz[i] = 0.0;
    }
    desenho_pio(matriz, pio, sm, 0.0, 0.0, 0.0);
}


// -- animação 8 -- Tecla 7 -- xadrez //
void xadrez(PIO pio, uint sm) {
    // Padrão inicial: todos os LEDs na cor vermelha
    double vermelho[25] = {
        1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0
    };

    // Padrão alternado: todos os LEDs na cor branca
    double branco[25] = {
        1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0
    };

    printf("Iniciando animação de xadrez...\n");

    char current_key = 11;
    char last_key = '7'; // Assume que '7' é a tecla para iniciar a animação
    int num_repeticoes = 3;

    while (num_repeticoes > 0) {
        // Verifica se outra tecla foi pressionada
        current_key = scan_keypad();
        if (current_key != 11 && current_key != last_key) {
            printf("Tecla pressionada: %c. Encerrando animação.\n", current_key);
            break; // Sai do loop ao detectar outra tecla
        }
        printf("Repetições restantes: %d\n", num_repeticoes);
        // Etapa 1: Alternar entre vermelho e branco
        desenho_pio(vermelho, pio, sm, 1.0, 0.0, 0.0); // Vermelho
        sleep_ms(200);
        desenho_pio(branco, pio, sm, 1.0, 1.0, 1.0); // Branco
        sleep_ms(200);

        // Etapa 2: Piscar rapidamente
        for (int i = 0; i < 5; i++) {
            desenho_pio(vermelho, pio, sm, 1.0, 0.0, 0.0); // Vermelho
            sleep_ms(100);
            desenho_pio(branco, pio, sm, 1.0, 1.0, 1.0); // Branco
            sleep_ms(100);
        }

        // Etapa 3: Alternância de padrões
        double padrao1[25] = {
            1.0, 0.0, 1.0, 0.0, 1.0,
            0.0, 1.0, 0.0, 1.0, 0.0,
            1.0, 0.0, 1.0, 0.0, 1.0,
            0.0, 1.0, 0.0, 1.0, 0.0,
            1.0, 0.0, 1.0, 0.0, 1.0
        };

        double padrao2[25] = {
            0.0, 1.0, 0.0, 1.0, 0.0,
            1.0, 0.0, 1.0, 0.0, 1.0,
            0.0, 1.0, 0.0, 1.0, 0.0,
            1.0, 0.0, 1.0, 0.0, 1.0,
            0.0, 1.0, 0.0, 1.0, 0.0
        };

        for (int i = 0; i < 5; i++) {
            desenho_pio((i % 2 == 0) ? padrao1 : padrao2, pio, sm, 1.0, 0.0, 0.0); // Vermelho
            sleep_ms(200);
            desenho_pio((i % 2 == 0) ? padrao1 : padrao2, pio, sm, 1.0, 1.0, 1.0); // Branco
            sleep_ms(200);
        }
        num_repeticoes--;
    }

    // Finaliza apagando os LEDs
    double apagar[25] = {0};
    desenho_pio(apagar, pio, sm, 0.0, 0.0, 0.0);
    printf("Animação de xadrez encerrada.\n");
}

// -- animação 9 -- Tecla 8 -- Explosão Radial -- //
void explosao_radial(PIO pio, uint sm, float *hue_base) {
    int layer0[] = {12};
    int layer1[] = {6, 7, 8, 11, 13, 16, 17, 18};
    int layer2[] = {0,1,2,3,4,5,9,10,14,15,19,20,21,22,23,24};
    int* layers[] = {layer0, layer1, layer2};
    int layer_sizes[] = {1, 8, 16};
    int num_layers = 3;

    // Frequências para expansão (C4, E4, G4) e retração (G4, E4)
    float freq_expand[] = {261.63, 329.63, 392.00};
    float freq_retract[] = {392.00, 329.63};
    int num_repeticoes = 5;

    while (num_repeticoes-- > 0) {
        // Expansão com sons ascendentes
        for (int i = 0; i < num_layers; i++) {
            double desenho[25] = {0.0};
            for (int j = 0; j < layer_sizes[i]; j++) desenho[layers[i][j]] = 1.0;
            
            float hue = fmod(*hue_base + i * 0.3, 1.0);
            float r, g, b;
            hsv_to_rgb(hue, 1.0, 1.0, &r, &g, &b);
            desenho_pio(desenho, pio, sm, r, g, b);
            
            activate_buzzer(freq_expand[i], 100); // Som da camada
            if (scan_keypad() != '5' && scan_keypad() != 11) {
                desenho_pio(ledsDesligados, pio, sm, 0.0, 0.0, 0.0);
                activate_buzzer(0, 0);
                return;
            }
        }

        // Retração com sons descendentes
        for (int i = num_layers - 2; i >= 0; i--) {
            double desenho[25] = {0.0};
            for (int j = 0; j < layer_sizes[i]; j++) desenho[layers[i][j]] = 1.0;
            
            float hue = fmod(*hue_base + i * 0.3, 1.0);
            float r, g, b;
            hsv_to_rgb(hue, 1.0, 1.0, &r, &g, &b);
            desenho_pio(desenho, pio, sm, r, g, b);
            
            activate_buzzer(freq_retract[i], 200); // Som da camada
            if (scan_keypad() != '5' && scan_keypad() != 11) {
                desenho_pio(ledsDesligados, pio, sm, 0.0, 0.0, 0.0);
                activate_buzzer(0, 0);
                return;
            }
        }

        *hue_base += 0.05;
        if (*hue_base >= 1.0) *hue_base -= 1.0;

        printf("num_repeticoes: %d\n", num_repeticoes);
    }
}

// Animacao 10 - Tecla 9 - Movimento cardinal com rebote//
// Frames para animação 9 
double anim10_frame0[25] = { 
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double anim10_frame1[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double anim10_frame2[25] = {
    1.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0
};

double anim10_frame3[25] = {
    0.0, 1.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0, 0.0
};

double anim10_frame4[25] = {
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0
};

double anim10_frame5[25] = {
    0.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 1.0, 0.0
};

double anim10_frame6[25] = {
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0
};

double anim10_frame7[25] = {
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0
};

double anim10_frame8[25] = {
    1.0, 0.0, 1.0, 0.0, 1.0,
    0.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 0.0, 1.0, 0.0, 1.0
};

// Array de frames e cores
double* anim10_frames[] = {
    anim10_frame0, anim10_frame1, anim10_frame2,
    anim10_frame3, anim10_frame4, anim10_frame5,
    anim10_frame6, anim10_frame7, anim10_frame8
};

float anim10_cores[9][3] = {
    {0.5, 0.0, 0.5}, // Roxo (centro)
    {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, // Azul (cima)
    {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, // Azul (direita)
    {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, // Azul (baixo)
    {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}  // Azul (esquerda)
};


void animacao10(PIO pio, uint sm) {
    printf("Iniciando animação de movimento cardinal...\n");
    
    const int total_frames = 9;
    int atual_frame = 0;
    int direcao = 1; // 1 para frente, -1 para trás
    char atual_key;
    int num_frames = 45;

    while(num_frames-- > 0) {
        // Verificação de interrupção
        atual_key = scan_keypad();
        if(atual_key != 11 && atual_key != '9') {
            printf("Tecla %c pressionada. Parando animação.\n", atual_key);
            desenho_pio(ledsDesligados, pio, sm, 0.0, 0.0, 0.0);
            return;
        }

        // Atualiza frame com cores
        desenho_pio(
            anim10_frames[atual_frame], 
            pio, 
            sm, 
            anim10_cores[atual_frame][0],
            anim10_cores[atual_frame][1],
            anim10_cores[atual_frame][2]
        );

        // Controle de fluxo de frames
        if(direcao == 1) {
            if(atual_frame < total_frames-1) atual_frame++;
            else direcao = -1;
        } else {
            if(atual_frame > 0) atual_frame--;
            else direcao = 1;
        }

        sleep_ms(300); 
        printf("Frame restantes: %d\n", num_frames);
    }

}


// -- animação 6 -- Tecla 5 -- Espiral  //

double anim6_frame0[25] = {
    1.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double anim6_frame1[25] = {
    1.0, 1.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double anim6_frame2[25] = {
    1.0, 1.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double anim6_frame3[25] = {
    1.0, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double anim6_frame4[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double anim6_frame5[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double anim6_frame6[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double anim6_frame7[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

double anim6_frame8[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0
};

double anim6_frame9[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 1.0, 1.0
};

double anim6_frame10[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 1.0, 1.0, 1.0
};

double anim6_frame11[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame12[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame13[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame14[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame15[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame16[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame17[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame18[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame19[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame20[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame21[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 0.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame22[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame23[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 0.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim6_frame24[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double* anim6_frames[] = {
    anim6_frame0, anim6_frame1, anim6_frame2,
    anim6_frame3, anim6_frame4, anim6_frame5,
    anim6_frame6, anim6_frame7, anim6_frame8, 
    anim6_frame9, anim6_frame10, anim6_frame11,
    anim6_frame12, anim6_frame13, anim6_frame14,
    anim6_frame15, anim6_frame16, anim6_frame17,
    anim6_frame18, anim6_frame19, anim6_frame20,
    anim6_frame21, anim6_frame22, anim6_frame23,
    anim6_frame24
};

float anim6_cores[5][3] = {
    {0.5, 0.0, 0.5}, // Roxo (centro)
    {0.0, 0.0, 1.0}, {1.0, 0.0, 1.0}, // Azul (cima)
    {1.0, 0.0, 0.0}, {0.0, 1.0, 1.0}, // Azul (direita) 
};

void animacao6(PIO pio, uint sm) {
    printf("Iniciando animação 6 linha-coluna...\n");
    
    const int total_frames = 25;
    int atual_frame = 0;
    int direcao = 1; // 1 para frente, -1 para trás
    char atual_key;
    int num_frames = 50;

    while(num_frames-- > 0) {
        // Verificação de interrupção
        atual_key = scan_keypad();
        if(atual_key != 11 && atual_key != '5') {
            printf("Tecla %c pressionada. Parando animação.\n", atual_key);
            desenho_pio(ledsDesligados, pio, sm, 0.0, 0.0, 0.0);
            return;
        }

        // Atualiza frame com cores
        desenho_pio(
            anim6_frames[atual_frame], 
            pio, 
            sm, 
            anim6_cores[atual_frame % 5][0],
            anim6_cores[atual_frame % 5][1],
            anim6_cores[atual_frame% 5][2]
        );

        // Controle de fluxo de frames
        if(direcao == 1) {
            if(atual_frame < total_frames-1) atual_frame++;
            else direcao = -1;
        } else {
            if(atual_frame > 0) atual_frame--;
            else direcao = 1;
        }

        sleep_ms(300); 
        printf("Frames restantes: %d\n", num_frames);
    }
}

//ANIMAÇÃO 3 - Tecla 1 -- linha - coluna

double anim3_frame0[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim3_frame1[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim3_frame2[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim3_frame3[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim3_frame4[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim3_frame5[25] = {
    1.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0
};

double anim3_frame6[25] = {
    1.0, 1.0, 1.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 0.0, 0.0
};

double anim3_frame7[25] = {
    1.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 0.0, 0.0, 0.0
};

double anim3_frame8[25] = { 
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0
};

double anim3_frame9[25] = { 
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

// Array de frames e cores
double* anim3_frames[] = {
    anim3_frame0, anim3_frame1, anim3_frame2,
    anim3_frame3, anim3_frame4, anim3_frame5,
    anim3_frame6, anim3_frame7, anim3_frame8, 
    anim3_frame9
};

float anim3_cores[10][3] = {
    {0.5, 0.0, 0.5}, // Roxo (centro)
    {0.0, 0.0, 1.0}, {1.0, 0.0, 1.0}, 
    {1.0, 0.0, 0.0}, {0.0, 1.0, 1.0}, 
    {0.0, 0.0, 1.0}, {1.0, 0.0, 1.0},
    {1.0, 0.0, 0.0}, {0.0, 1.0, 1.0}, 
    {0.5, 0.0, 0.5} 
};


void animacao3(PIO pio, uint sm) {
    printf("Iniciando animação linha-coluna...\n");
    
    const int total_frames = 10;
    int atual_frame = 0;
    int direcao = 1; // 1 para frente, -1 para trás
    char atual_key;
    int num_frames = 25;

    while(num_frames-- > 0) {
        // Verificação de interrupção
        atual_key = scan_keypad();
        if(atual_key != 11 && atual_key != '2') {
            printf("Tecla %c pressionada. Parando animação.\n", atual_key);
            desenho_pio(ledsDesligados, pio, sm, 0.0, 0.0, 0.0);
            return;
        }

        // Atualiza frame com cores
        desenho_pio(
            anim3_frames[atual_frame], 
            pio, 
            sm, 
            anim3_cores[atual_frame][0],
            anim3_cores[atual_frame][1],
            anim3_cores[atual_frame][2]
        );

        // Controle de fluxo de frames
        if(direcao == 1) {
            if(atual_frame < total_frames-1) atual_frame++;
            else direcao = -1;
        } else {
            if(atual_frame > 0) atual_frame--;
            else direcao = 1;
        }

        sleep_ms(300); 
        printf("Frame restantes: %d\n", num_frames);
    }
}
