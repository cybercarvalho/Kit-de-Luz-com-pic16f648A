#ifndef TRATALUZRE_H
#define TRATALUZRE_H

// Luz de re no RA0, habilitada em todos os programas (0 a 4).
// Mesmo comando do KitLuzArduino:
//   1a frenagem: arma a re (permanece apagada);
//   soltar e frear novamente: liga a re;
//   soltar o freio: permanece ligada;
//   acelerar: desliga e reinicia a sequencia.
// O sentido do Canal 2 respeita FreioInvertido.
// Sem configuracao de programacao ou EEPROM nesta etapa.

unsigned char EstadoRe;         // 0=desarmada, 1=armada, 2=ligada
unsigned char FreioAnteriorRe;  // detecta nova borda de frenagem

void LuzRe_Reiniciar() {
    EstadoRe = 0;
    FreioAnteriorRe = 0;
    Re_Lanterna = 0;
}

void TrataLuzRe(unsigned int pulso, unsigned char novaLeitura) {
    unsigned char acelerando;

    // A saida permanece no estado anterior entre os pulsos RC.
    if (!novaLeitura) return;

    // Evita considerar um pulso invalido como comando de aceleracao.
    if (pulso == 0) {
        LuzRe_Reiniciar();
        return;
    }

    // Aceleracao no sentido oposto ao freio, conforme inversao gravada.
    acelerando = FreioInvertido ? (pulso > Max2) : (pulso < Min2);

    if (acelerando) {
        EstadoRe = 0;
    } else if (FreioAtivo && !FreioAnteriorRe && EstadoRe < 2) {
        // FreioAtivo ja foi validado por 3 pulsos em TrataCanal2.
        // Conta somente novas frenagens, nao o tempo segurando o freio.
        EstadoRe++;
    }

    FreioAnteriorRe = FreioAtivo;
    Re_Lanterna = (EstadoRe == 2);
}

#endif
