# Teoria e fórmulas

## Grandezas medidas

O projeto simula três grandezas de uma carga monofásica usando potenciômetros ligados entre 5 V e GND. O Arduino lê cada cursor com o conversor analógico-digital de 10 bits (valores de 0 a 1023) e converte para a grandeza real.

| Grandeza | Entrada | Conversão | Faixa |
|---|---|---|---|
| Tensão `V` | A0 | `V = raw / 1023 × 250` | 0 a 250 V |
| Corrente `I` | A1 | `I = raw / 1023 × 20` | 0 a 20 A |
| Fator de potência `FP` | A2 | `FP = 0,50 + raw / 1023 × 0,50` | 0,50 a 1,00 |

O fator de potência começa em 0,50 e não em zero, porque cargas reais não trabalham com FP nulo.

## Potências

- **Potência aparente:** `S = V · I` (em VA)
- **Potência ativa:** `P = S · FP` (em W)

O alerta usa a potência ativa.

## Energia e custo

A energia é a integral da potência no tempo. No código, ela é calculada de forma numérica, somando uma parcela a cada volta do laço:

```
E(kWh) += (P / 1000) × (Δt × ACEL / 3600)
```

- `Δt` é o tempo real entre duas voltas do laço, em segundos
- `ACEL` é o fator de aceleração do tempo (60 no projeto): 1 s real equivale a 60 s simulados
- O custo estimado é `E × TARIFA`

A aceleração existe porque, em tempo real, o consumo em kWh sobe devagar demais para ser visto em uma simulação curta. Por exemplo, 3000 W durante 1 s real acumulam 0,05 kWh simulados.

## Alerta com histerese

O alerta liga quando `P > 3000 W` e só desliga quando `P < 2850 W` (95% do limite). Sem a histerese, uma potência oscilando em torno de 3000 W faria o LED e o buzzer ficarem ligando e desligando.

Como a potência depende dos três potenciômetros, a corrente em que o alerta dispara muda conforme a tensão e o fator de potência: `I = 3000 / (V · FP)`. Com FP no mínimo (0,50), o maior valor possível é 250 × 20 × 0,5 = 2500 W, então o alerta não dispara.

## Média de amostras

Cada leitura analógica descarta a primeira amostra, para deixar o multiplexador do conversor estabilizar, e faz a média de 8 amostras. Isso reduz oscilações entre uma leitura e outra.

## Limitações da simulação

- Os potenciômetros fornecem tensões constantes (CC). Em uma instalação real em corrente alternada seria preciso amostrar a forma de onda e calcular os valores RMS.
- O fator de potência aqui é um valor escolhido pelo usuário. Em um medidor real ele seria calculado a partir da defasagem entre tensão e corrente.
- A tarifa usada (R$ 0,95/kWh) é um valor de exemplo.

## Caminho para hardware real

| Grandeza | Sensor sugerido | Observação |
|---|---|---|
| Tensão | ZMPT101B | Transformador de tensão com saída analógica, requer calibração |
| Corrente | ACS712 (5 A, 20 A ou 30 A) | Saída com offset de 2,5 V e sensibilidade de 185, 100 ou 66 mV/A conforme o modelo |

Nos dois casos, as constantes `V_MAX` e `I_MAX` seriam substituídas por fatores de calibração, e o código passaria a calcular o RMS a partir de várias amostras por ciclo da rede.