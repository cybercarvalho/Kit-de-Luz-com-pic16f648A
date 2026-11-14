#ifndef TRATACANAL2_H
#define TRATACANAL2_H

unsigned char FreioAtivo;
unsigned char CandidatoFreio;
unsigned char ContadorFreio;

// EstadoRe: 0=sem frenagem, 1=primeira frenagem, 2=re ligada.
unsigned char EstadoRe;
unsigned char FreioAnteriorRe;

void LuzRe_Reiniciar() {
    EstadoRe = 0;
    FreioAnteriorRe = 0;
    Re_Lanterna = 0;
}

void TrataCanal2_Iniciar() {
    FreioAtivo = 0;
    CandidatoFreio = 0;
    ContadorFreio = 0;
    LuzRe_Reiniciar();
    FreioPWM(0);
}

void TrataCanal2(unsigned int pulso, unsigned char novaLeitura) {
    unsigned char candidato;
    unsigned char acelerando;

    if (novaLeitura) {
        candidato = FreioAtivo;
        if (!FreioAtivo) {
            if ((!FreioInvertido && (pulso > Max2)) ||
                (FreioInvertido && (pulso < Min2))) candidato = 1;
        } else {
            if ((!FreioInvertido && (pulso < Limite2Soltar)) ||
                (FreioInvertido && (pulso > Limite2SoltarInv))) candidato = 0;
        }
        if (candidato == FreioAtivo) {
            ContadorFreio = 0;
        } else {
            if (candidato != CandidatoFreio) {
                CandidatoFreio = candidato;
                ContadorFreio = 0;
            }
            if (++ContadorFreio == 3) {
                FreioAtivo = candidato;
                ContadorFreio = 0;
            }
        }

        if (pulso == 0) {
            LuzRe_Reiniciar();
        } else {
            acelerando = FreioInvertido ? (pulso > Max2) : (pulso < Min2);
            if (acelerando) {
                EstadoRe = 0;
            } else if (FreioAtivo && !FreioAnteriorRe && EstadoRe < 2) {
                EstadoRe++;
            }
            FreioAnteriorRe = FreioAtivo;
            Re_Lanterna = (EstadoRe == 2);
        }
    }

    // Unico ponto de controle do PWM no modo normal.
    // Re ativa: mantem iluminacao base, mesmo durante frenagem.
    if ((EstadoRe != 2) && FreioAtivo)
        FreioPWM(PWM_ACIONADO);
    else
        FreioPWM(Programa == 0 ? 0 : 10);
}
#endif
