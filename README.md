# Controle de Temperatura PID com Acionamento Proporcional por Tempo

Este repositório contém o projeto de um controlador de temperatura de malha fechada desenvolvido para o **Laboratório de Controle da UFES - Campus São Mateus**. O sistema utiliza um algoritmo PID (focado em PD) para controlar uma carga resistiva de alta potência (1000W) através de um acionamento por relé.

##  Controle Contínuo em Atuador Digital
O maior desafio deste projeto foi aplicar a lógica de controle PID, que gera uma saída analógica/contínua, em um relé (dispositivo On/Off). 

### Solução: Controle Proporcional por Tempo (Slow PWM)
Para evitar o desgaste prematuro do relé e gerenciar a alta inércia térmica do sistema, foi implementada a técnica de **Slow PWM**:
* O PID calcula a potência necessária de 0 a 100%.
* Essa potência é convertida em um intervalo de tempo dentro de uma "janela de ciclo" (ex: 5 segundos).
* Se o PID pede 20% de potência, o relé fica ligado por 1 segundo e desligado por 4 segundos.


## Especificações do Hardware
* **Microcontrolador:** Arduino (ATmega328P).
* **Sensor de Temperatura:** DS18B20 (Digital, One-Wire) - Alta precisão e imunidade a ruído.
* **Atuador:** Relé acoplado a uma carga resistiva de 1000W.
* **Interface:** LCD 16x2 para monitoramento de temperatura e Setpoint, ajustado via potenciômetro.

## Estratégia de Controle
O sistema foi sintonizado para operar prioritariamente como um controlador **PD (Proporcional-Derivativo)**:
1.  **Ação Proporcional ($K_p$):** Garante a resposta rápida ao erro de temperatura.
2.  **Ação Derivativa ($K_d$):** Atua como amortecedor para evitar o *overshoot*, prevendo a velocidade de aquecimento e reduzindo a potência antes de atingir o Setpoint.
3.  **Filtragem de Ruído:** Implementação de média móvel nas leituras do sensor DS18B20 para evitar disparos falsos do relé.

## Estrutura do Projeto
* `/src`: Código fonte `.ino` com a implementação do algoritmo PID e lógica de temporização.
* `/docs`: Relatório técnico com análise de resposta ao degrau e sintonia de parâmetros.
* `/results`: Gráficos de performance para os setpoints de $40^{\circ}C$, $65^{\circ}C$ e $85^{\circ}C$.

## Resultados Alcançados
* **Estabilidade:** Erro estacionário praticamente nulo após a estabilização.
* **Segurança:** O controle derivativo impediu que a inércia térmica da resistência de 1000W ultrapassasse perigosamente a temperatura alvo.
* **Robustez:** Sistema validado em diferentes faixas de operação, mantendo a integridade do relé através da modulação temporal.

---
**Desenvolvido por: Norian Silva Aredes Hermsdorf
**Instituição:** UFES - CEUNES
**Data:** Agosto de 2025
