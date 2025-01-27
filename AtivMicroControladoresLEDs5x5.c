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
void piscar_X(PIO pio, uint sm);
void xadrez(PIO pio, uint sm);
void animacao1();
void animacao_diagonal(PIO pio, uint sm);
void animacao10(PIO pio, uint sm);
void animacao2(PIO pio, uint sm);
void ligar_leds_azul(PIO pio, uint sm);
void desligar_leds(PIO pio, uint sm);
void animacao6(PIO pio, uint sm);

// Função principal
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
            //animação 2
            printf("Iniciando linha coluna colorida...\n");
            animacao2(pio, sm);
            break;
        case '2':
            //animação 3
            printf("Iniciando animação diagonal...\n");
            animacao_diagonal(pio, sm);
            break;
        case '3':
            //animação 4
            printf("Iniciando quadrado crescendo...\n");
            animacao_quadrado(pio, sm);
            break;            
        case '4':
            //animação 5
            printf("Iniciando piscar X com 10 cores...\n");
            piscar_X(pio, sm);
            break;         
        case '5':
            //animação 6
            printf("Iniciando espiral...\n");
            animacao6(pio, sm);
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
            break;
        case '8':
            //animação 9
            break;
        case '9':
            //animação 10
            printf("Iniciando movimento cardinal...\n");
            animacao10(pio, sm);
            break;
        case 'A':
           //Desligar todos os LEDs
           printf("Desligando todos os LEDs...\n");
            desligar_leds(pio, sm);
            break;
        case 'B':
            //Ligar todos os LEDs na cor azul, 100% de intensidade
            printf("Ligando todos os LEDs na cor azul (100%% de intensidade)...\n");
            ligar_leds_azul(pio, sm);
            break;
        case 'C':
            //Ligar todos os LEDs na cor vermelha, 80% de intensidade
             desenho_pio(ledsLIGADOS, pio, sm, 0.8, 0.0, 0.0); // LEDs vermelhos, 80%
             printf("Todos os LEDs ligados na cor vermelha com 80%% de intensidade\n");
            break;
        case 'D':
            //Ligar todos os LEDs na cor verde, 50% de intensidade
            printf("Ligando todos os LEDs na cor verde (50%% de intensidade)...\n");
            ligar_leds_verde(pio, sm);
            break;
        case '#':
            //Ligar todos os LEDs na cor branca, 20% de intensidade
            printf("Ligando todos os LEDs na cor branca (20%% de intensidade)...\n");
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
        gpio_put(row_pins[row], 0); 
        for (int col = 0; col < COLS; col++) {
            if (gpio_get(col_pins[col]) == 0) { 
                gpio_put(row_pins[row], 1);
                return keys[row][col]; 
            }
        }
        gpio_put(row_pins[row], 1); 
    }
    return 11; 
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

// -- animação 2 -- Tecla 1 -- arco iris //
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
        if (scan_keypad() != '6' && scan_keypad() != 11) {
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

// -- animação 3 -- Tecla 3 -- quadrado crescendo //
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

// -- animação 4 -- Tecla 4 -- piscar X //
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

// -- animação 3 -- Tecla 2 --  //
// Frames da animação diagonal 
double diagonal_frame0[25] = {
    1.0,0.0,0.0,0.0,0.0,
    0.0,1.0,0.0,0.0,0.0,
    0.0,0.0,1.0,0.0,0.0,
    0.0,0.0,0.0,1.0,0.0,
    0.0,0.0,0.0,0.0,1.0
};

double diagonal_frame1[25] = {
    0.0,1.0,0.0,0.0,0.0,
    0.0,0.0,1.0,0.0,0.0,
    0.0,0.0,0.0,1.0,0.0,
    0.0,0.0,0.0,0.0,1.0,
    1.0,0.0,0.0,0.0,0.0
};

double diagonal_frame2[25] = {
    0.0,0.0,1.0,0.0,0.0,
    0.0,0.0,0.0,1.0,0.0,
    0.0,0.0,0.0,0.0,1.0,
    1.0,0.0,0.0,0.0,0.0,
    0.0,1.0,0.0,0.0,0.0
};

double diagonal_frame3[25] = {
    0.0,0.0,0.0,1.0,0.0,
    0.0,0.0,0.0,0.0,1.0,
    1.0,0.0,0.0,0.0,0.0,
    0.0,1.0,0.0,0.0,0.0,
    0.0,0.0,1.0,0.0,0.0
};

double diagonal_frame4[25] = {
    0.0,0.0,0.0,0.0,1.0,
    1.0,0.0,0.0,0.0,0.0,
    0.0,1.0,0.0,0.0,0.0,
    0.0,0.0,1.0,0.0,0.0,
    0.0,0.0,0.0,1.0,0.0
};

// -- animação 3 -- Tecla 2 -- Diagonal giratória -- //
void animacao_diagonal(PIO pio, uint sm) {
    double *frames[] = {diagonal_frame0, diagonal_frame1, diagonal_frame2, diagonal_frame3, diagonal_frame4};
    int num_frames = 5;
    
    printf("Iniciando animação diagonal...\n");
    
    char current_key;
    char last_key = '2'; // Tecla que inicia a animação
    int num_repeticoes = 3;

    while (num_repeticoes > 0) {
        printf("Repetições restantes: %d\n", num_repeticoes);
        for (int j = 0; j < num_frames; j++) {
            // Verifica se outra tecla foi pressionada
            current_key = scan_keypad();
            if (current_key != 11 && current_key != last_key) {
                printf("Tecla pressionada: %c. Parando animação.\n", current_key);
                desenho_pio(ledsDesligados, pio, sm, 0.0, 0.0, 0.0);
                return;
            }
            
            // Atualiza os LEDs com o frame atual (cor roxa: R=1.0, G=0.0, B=1.0)
            desenho_pio(frames[j], pio, sm, 1.0, 0.0, 1.0);
            sleep_ms(500); // Atraso entre frames
        }
        num_repeticoes--;
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

//ANIMAÇÃO 2 - Tecla 1 -- linha - coluna

double anim2_frame0[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim2_frame1[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim2_frame2[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim2_frame3[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim2_frame4[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

double anim2_frame5[25] = {
    1.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0
};

double anim2_frame6[25] = {
    1.0, 1.0, 1.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 0.0, 0.0
};

double anim2_frame7[25] = {
    1.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 0.0, 0.0, 0.0
};

double anim2_frame8[25] = { 
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0
};

double anim2_frame9[25] = { 
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};

// Array de frames e cores
double* anim2_frames[] = {
    anim2_frame0, anim2_frame1, anim2_frame2,
    anim2_frame3, anim2_frame4, anim2_frame5,
    anim2_frame6, anim2_frame7, anim2_frame8, 
    anim2_frame9
};

float anim2_cores[10][3] = {
    {0.5, 0.0, 0.5}, // Roxo (centro)
    {0.0, 0.0, 1.0}, {1.0, 0.0, 1.0}, // Azul (cima)
    {1.0, 0.0, 0.0}, {0.0, 1.0, 1.0}, // Azul (direita)
    {0.0, 0.0, 1.0}, {1.0, 0.0, 1.0}, // Azul (baixo)
    {1.0, 0.0, 0.0}, {0.0, 1.0, 1.0}, 
    {0.5, 0.0, 0.5}  // Azul (esquerda)
};


void animacao2(PIO pio, uint sm) {
    printf("Iniciando animação linha-coluna...\n");
    
    const int total_frames = 10;
    int atual_frame = 0;
    int direcao = 1; // 1 para frente, -1 para trás
    char atual_key;
    int num_frames = 45;

    while(num_frames-- > 0) {
        // Verificação de interrupção
        atual_key = scan_keypad();
        if(atual_key != 11 && atual_key != '1') {
            printf("Tecla %c pressionada. Parando animação.\n", atual_key);
            desenho_pio(ledsDesligados, pio, sm, 0.0, 0.0, 0.0);
            return;
        }

        // Atualiza frame com cores
        desenho_pio(
            anim2_frames[atual_frame], 
            pio, 
            sm, 
            anim2_cores[atual_frame][0],
            anim2_cores[atual_frame][1],
            anim2_cores[atual_frame][2]
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

