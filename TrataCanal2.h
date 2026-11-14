#ifndef TRATACANAL2_H
#define TRATACANAL2_H

unsigned char FreioAtivo;
unsigned char TrataRuidoFreio;
unsigned char ContadorFreio;

// EstadoRe: 0=sem frenagem, 1=primeira frenagem, 2=re ligada.
unsigned char EstadoRe;
unsigned char FreioAnteriorRe; // ultimo estado HIGH do Canal 2

void LuzRe_Reiniciar() {
    EstadoRe = 0;
    FreioAnteriorRe = 0;
    Re_Lanterna = 0;
}

void TrataCanal2_Iniciar() {
    FreioAtivo = 0;
    TrataRuidoFreio = 0;
    ContadorFreio = 0;
    LuzRe_Reiniciar();
    FreioPWM(0);
}

void TrataCanal2(unsigned int pulso, unsigned char novaLeitura) {
    unsigned char EstadoFreio;
    unsigned char acelerando;

    if (novaLeitura) {
        EstadoFreio = FreioAtivo;
        if (!FreioAtivo) {
            if ((!FreioInvertido && (pulso > Max2)) ||
                (FreioInvertido && (pulso < Min2))) EstadoFreio = 1;
        } else {
            if ((!FreioInvertido && (pulso < Limite2Soltar)) ||
                (FreioInvertido && (pulso > Limite2SoltarInv))) EstadoFreio = 0;
        }
        if (EstadoFreio == FreioAtivo) {
            ContadorFreio = 0;
        } else {
            if (EstadoFreio != TrataRuidoFreio) {
                TrataRuidoFreio = EstadoFreio;
                ContadorFreio = 0;
            }
            if (++ContadorFreio == 3) {
                FreioAtivo = EstadoFreio;
                ContadorFreio = 0;
            }
        }

        // HIGH conta uma nova frenagem; neutro preserva o estado.
        // LOW e a unica condicao RC que reinicia a sequencia.
        // A contagem da re e fixa em HIGH, independente da
        // configuracao de inversao do freio.
        if (pulso < Min2) {
            EstadoRe = 0;
            FreioAnteriorRe = 0;
        } else {
            acelerando = (pulso > Max2);
            if (acelerando && !FreioAnteriorRe && EstadoRe < 2)
                EstadoRe++;
            FreioAnteriorRe = acelerando;
        }
        Re_Lanterna = (EstadoRe == 2);
    }

    // Unico ponto de controle do PWM no modo normal.
    // Re ativa: mantem iluminacao base, mesmo durante frenagem.
    if ((EstadoRe != 2) && FreioAtivo)
        FreioPWM(PWM_ACIONADO);
    else
        FreioPWM(Programa == 0 ? 0 : 10);
}
#endif
