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
| Linhas do Teclado| 2, 3, 4, 5   |
| Colunas do Teclado| 16, 17, 18, 19|
| Dados da Matriz LED| 7          |
| Buzzer           | 21           |


## ⌨️ Mapeamento de Teclas
| Tecla | Ação                          | Cor/Efeito                 |
|-------|-------------------------------|----------------------------|
| 0     | Contagem regressiva + ❤️      | Vermelho                   |
| 2     | Onda diagonal                | Roxo                       |
| 3     | Quadrado em expansão         | Multicolorido              |
| 4     | X piscante                   | Transição de cores         |
| 6     | Arco-íris dinâmico           | 🌈                         |
| 7     | Padrão xadrez                | Vermelho/Branco            |
| 9     | Movimento cardinal           | Azul                       |
| D     | Todos LEDs verdes            | 50% brilho                 |
| #     | Todos LEDs brancos           | 20% brilho                 |
| C     | Todos LEDs vermelhos         | 80% brilho                 |
| *     | Reiniciar dispositivo        | -                          |

### 🎨 Guia de Utilização

1. **Contagem Regressiva + Coração com buzzer**  
   - Tecla: `0`  
   - Ação: Pressione uma vez e aguarde a sequência completa

2. **Onda Diagonal Roxa**  
   - Tecla: `2`  
   - Como usar: Pressione para iniciar, qualquer tecla para parar

3. **Quadrado Colorido Expansivo**  
   - Tecla: `3`  
   - Comportamento: Executa 5 fases automaticamente (não interrompível)

4. **X Piscante com Transições**  
   - Tecla: `4`  
   - Operação: Pressione para ciclo de 10 cores, outra tecla para parar

5. **Arco-Íris Dinâmico**  
   - Tecla: `6`  
   - Modo de uso: Toque para iniciar

6. **Padrão Xadrez Interativo**  
   - Tecla: `7`  
   - Controle: Pressione para alternância rápida, toque novamente para parar

7. **Movimento Cardinal Azul**  
   - Tecla: `9`  
   - Funcionamento: Executa 3 ciclos completos automaticamente

8. **LEDs Verdes Fixos**  
   - Tecla: `D`  
   - Efeito: Toque único para ativar, outra tecla para desligar

9. **Branco Suave**  
   - Tecla: `#` (hashtag)  
   - Uso: Pressione para ativar, outra tecla para desativar

10. **Vermelho Intenso**  
    - Tecla: `C`  
    - Ação: Pressione para ativar, outra tecla para desligar

11. **Reinicialização Rápida**  
    - Tecla: `*` (asterisco)  
    - Importante: Pressione para reiniciar imediatamente (perde animações em curso)
   
    --------------------------------------------------
**Desenvolvedores** (7 componentes): Ariel Fernando, Cintia Conceição, Danilo Lacerda, Hércules, Julio Junior, Pedro Gabriel e Tárcio Santos.

**Vídeo Ensaio**: [link]
