# Controle de Teclado Matricial e Matriz LED com Raspberry Pi Pico

Projeto que integra um teclado matricial 4x4 a uma matriz LED RGB 5x5, com animações interativas e feedback sonoro.

## 📋 Funcionalidades
- **Teclado Matricial 4x4**
- **Matriz LED 5x5** com controle via PIO
- **10+ Animações** controladas por teclas
- **Buzzer** para feedback sonoro
- **Reboot via software** (tecla *)

## 🛠 Hardware
| Componente       | GPIO no Pico |
|------------------|--------------|
| Linhas do Teclado| 16, 17, 18, 19 | 
| Colunas do Teclado| 20, 4, 9, 8 |
| Dados da Matriz LED| 7          |
| Buzzer           | 21           |


## ⌨️ Mapeamento de Teclas
| Tecla | Ação                          | Cor/Efeito                 |
|-------|-------------------------------|----------------------------|
| 0     | Contagem regressiva + ❤️     | Vermelho + buzzer          |
| 1     | Onda de Cores                | Multicorido + buzzer        |
| 2     | Onda diagonal                | Roxo                       |
| 3     | Quadrado em expansão         | Multicolorido              |
| 4     | I ❤️ NORDESTE                | Vermelho                   |
| 5     | Espiral LED por LED          | Azul                       |
| 6     | Arco-íris dinâmico           | 🌈                         |
| 7     | Padrão xadrez                | Vermelho/sem cor           |
| 8     | Explosão Radial              | Multicolorido + buzzer     |
| 9     | Movimento cardinal           | Azul                       |
| A     | Todos LEDs Dsligados         | -                          |
| B     | Todos Leds Azuis             | 100% brilho                |
| C     | Todos LEDs vermelhos         | 80% brilho                 |
| D     | Todos LEDs verdes            | 50% brilho                 |
| #     | Todos LEDs brancos           | 20% brilho                 |
| *     | Reiniciar dispositivo        | -                          |

### 🎨 Guia de Utilização

1. **Contagem Regressiva + Coração com buzzer**  
   - Tecla: `0`  
   - Ação: Pressione uma vez e aguarde a sequência completa

2. **Onda de Cores com Buzzer**  
   - Tecla: `1`  
   - Operação: Pressione para iniciar, outra tecla para parar

3. **Onda Diagonal Roxa**  
   - Tecla: `2`  
   - Como usar: Pressione para iniciar, qualquer tecla para parar

4. **Quadrado Colorido Expansivo**  
   - Tecla: `3`  
   - Comportamento: Executa 5 fases automaticamente (não interrompível)

5. **X Piscante com Transições**  
   - Tecla: `4`  
   - Operação: Pressione para ciclo de 10 cores, outra tecla para parar

6. **Espiral LED por LED Azul**  
   - Tecla: `5`  
   - Modo de uso: Pressione para iniciar, outra tecla para parar

7. **Arco-Íris Dinâmico**  
   - Tecla: `6`  
   - Modo de uso: Toque para iniciar

8. **Padrão Xadrez Interativo**  
   - Tecla: `7`  
   - Controle: Pressione para alternância rápida, toque novamente para parar

9. **Explosão Radial**  
   - Tecla: `8`  
   - Modo de uso: Pressione para iniciar, outra tecla para parar

10. **Movimento Cardinal Azul**  
   - Tecla: `9`  
   - Funcionamento: Executa 3 ciclos completos automaticamente

11. **LEDs Desligados**  
   - Tecla: `A`  
   - Efeito: Toque único para ativar, outra tecla para desligar

12 **LEDs Azuis Fixos - 100% Brilho**  
   - Tecla: `B`  
   - Efeito: Toque único para ativar, outra tecla para desligar

13. **LEDs Vermelhos Fixos - 80% Brilho**  
   - Tecla: `C`  
   - Efeito: Toque único para ativar, outra tecla para desligar

15. **LEDs Verdes Fixos - 50% Brilho**  
   - Tecla: `D`  
   - Efeito: Toque único para ativar, outra tecla para desligar

16. **Branco Suave - 20% Brilho**
   - Tecla: `#` (hashtag)  
   - Uso: Pressione para ativar, outra tecla para desativar

19. **Reboot**  
   - Tecla: `*` (asterisco)  
   - Ação: Pressione para reiniciar o dispositivo 
   
   --------------------------------------------------
**Desenvolvedores** (7 componentes): Ariel Fernando, Cíntia Conceição, Danilo Lacerda, Hércules, Julio Junior, Pedro Gabriel e Tárcio Santos.

**Vídeo Ensaio Wokwi**: 

**Vídeo Ensaio Placa Bitdoglab**: 
