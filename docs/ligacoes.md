# Ligações e montagem

Este documento descreve cada fio do circuito montado no Tinkercad Circuits.

## Convenções

- **Placa A** e **Placa B** são duas placas de ensaio pequenas (30 linhas).
- Em uma placa, os 5 furos de uma mesma linha em cada metade (a–e e f–j) são ligados entre si.
- Trilho **+** é 5 V e trilho **−** é GND.
- Cores dos fios: vermelho para 5 V, preto para GND, demais cores para sinais.

## Arduino ↔ LCD 16x2

O LCD do Tinkercad usa os nomes GND e VCC no lugar de VSS e VDD.

| Pino do LCD | Liga em | Cor |
|---|---|---|
| GND | GND | Preto |
| VCC | 5V | Vermelho |
| VO | Cursor do potenciômetro de contraste | Roxo |
| RS | D12 | Azul |
| RW | GND | Preto |
| E | D11 | Verde |
| DB0 a DB3 | Sem ligação | |
| DB4 | D5 | Amarelo |
| DB5 | D4 | Laranja |
| DB6 | D3 | Marrom |
| DB7 | D2 | Branco |
| LED (1º pino, esquerda) | 5V, através do resistor R1 de 220 Ω | Vermelho |
| LED (2º pino, direita) | GND | Preto |

## Placa A: entradas simuladas

Os potenciômetros ficam girados 90°, com os pinos na coluna **a**. Os fios saem da coluna **d** da mesma linha.

| Potenciômetro | Linhas | Pino de cima | Pino do meio | Pino de baixo |
|---|---|---|---|---|
| Pot 1, tensão | 12, 13, 14 | Trilho + | **A0** | Trilho − |
| Pot 2, corrente | 17, 18, 19 | Trilho + | **A1** | Trilho − |
| Pot 3, fator de potência | 22, 23, 24 | Trilho + | **A2** | Trilho − |
| Pot 4, contraste | 27, 28, 29 | Trilho + | **VO** do LCD | Trilho − |

Resistor R1 (220 Ω, backlight do LCD): na coluna **a**, linhas 5 a 9. Um fio vermelho sai de **e9** (perna de baixo) até o trilho + da direita; outro fio vermelho sai de **c5** (perna de cima) até o 1º pino "LED" do LCD.

Os trilhos da direita da Placa A estão ligados ao 5V e ao GND do Arduino.

## Placa B: painel de controle

A placa fica deitada: a linha é o número de 1 a 30, contado da esquerda para a direita; a metade de cima são as colunas f–j e a de baixo são as colunas a–e. Os dois trilhos − são GND, ligados entre si por uma ponte de fio preto perto da linha 1, e o trilho − de baixo recebe o GND da Placa A.

| Componente | Posição | Ligação |
|---|---|---|
| Botão TELA | Sobre o canal, linhas 3 a 5 | c3 ao trilho −; h5 ao **D7** |
| Botão RESET | Sobre o canal, linhas 8 a 10 | c8 ao trilho −; h10 ao **D8** |
| Resistor 220 Ω (LED verde) | h14 a h18 | j14 ao **D9** |
| LED verde | Coluna j: cátodo em j17, ânodo em j18 | j17 ao trilho − de cima |
| Resistor 220 Ω (LED vermelho) | h21 a h25 | j21 ao **D10** |
| LED vermelho | Coluna j: cátodo em j24, ânodo em j25 | j24 ao trilho − de cima |

Os botões usam o resistor de pull-up interno do Arduino, então não precisam de resistor externo.

## Buzzer

O piezo fica fora da protoboard:

| Perna | Liga em |
|---|---|
| Positiva (+) | **D6** |
| Negativa (−) | Trilho − de cima da Placa B |

## Resumo dos pinos do Arduino

| Pino | Função |
|---|---|
| A0, A1, A2 | Tensão, corrente e fator de potência |
| D2, D3, D4, D5 | DB7, DB6, DB5 e DB4 do LCD |
| D6 | Buzzer |
| D7 | Botão TELA |
| D8 | Botão RESET |
| D9 | LED verde |
| D10 | LED vermelho |
| D11 | E do LCD |
| D12 | RS do LCD |
| 5V, GND | Alimentação dos trilhos e do LCD |

## Nomes dos componentes no Tinkercad

Padrão usado: prefixo do tipo, número e função, em maiúsculas e sem acentos.

| Componente | Nome | Onde fica |
|---|---|---|
| Arduino Uno R3 | `U1_ARDUINO` | Topo da área de trabalho |
| LCD 16x2 | `U2_LCD` | À direita do Arduino |
| Placa de ensaio (entradas) | `PLACA_A_ENTRADAS` | Abaixo à esquerda |
| Placa de ensaio (painel) | `PLACA_B_PAINEL` | À direita do LCD |
| Potenciômetro de tensão | `RPOT1_TENSAO` | Placa A, linhas 12 a 14 |
| Potenciômetro de corrente | `RPOT2_CORRENTE` | Placa A, linhas 17 a 19 |
| Potenciômetro de fator de potência | `RPOT3_FATOR_POTENCIA` | Placa A, linhas 22 a 24 |
| Potenciômetro de contraste | `RPOT4_CONTRASTE` | Placa A, linhas 27 a 29 |
| Resistor do backlight | `R1_BACKLIGHT` | Placa A, linhas 5 a 9 |
| Resistor do LED verde | `R2_LED_VERDE` | Placa B, h14 a h18 |
| Resistor do LED vermelho | `R3_LED_VERMELHO` | Placa B, h21 a h25 |
| Botão que troca a tela | `SW1_TELA` | Placa B, linhas 3 a 5 |
| Botão de reset | `SW2_RESET` | Placa B, linhas 8 a 10 |
| LED verde | `D1_LED_VERDE` | Placa B, coluna j, linhas 17 e 18 |
| LED vermelho | `D2_LED_VERMELHO` | Placa B, coluna j, linhas 24 e 25 |
| Buzzer piezo | `BZ1_BUZZER` | À direita da Placa B |