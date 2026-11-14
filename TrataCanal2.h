#ifndef TRATACANAL2_H
#define TRATACANAL2_H
unsigned char FreioAtivo;
unsigned char CandidatoFreio;
unsigned char ContadorFreio;

void TrataCanal2_Iniciar() {
    FreioAtivo = 0;
    CandidatoFreio = 0;
    ContadorFreio = 0;
    FreioPWM(0);
}

void TrataCanal2(unsigned int pulso, unsigned char novaLeitura) {
    unsigned char candidato;
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
    }
    // O PWM do freio e aplicado no main, apos TrataLuzRe.
    // Assim nao ha pulso de frenagem durante a luz de re.
}
#endif
