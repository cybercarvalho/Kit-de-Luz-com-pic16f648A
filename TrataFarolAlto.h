#ifndef TRATAFAROLALTO_H
#define TRATAFAROLALTO_H
// Flag: 1 habilita farol alto ao acelerar, 0 desabilita.
unsigned char FarolAltoAceleracao = 0;
unsigned char AceleracaoAtiva = 0;
unsigned char CandidatoAceleracao = 0;
unsigned char ContadorAceleracao = 0;

void TrataFarolAlto_Reiniciar() {
    AceleracaoAtiva = 0;
    CandidatoAceleracao = 0;
    ContadorAceleracao = 0;
}

void TrataFarolAlto(unsigned int pulso, unsigned char novaLeitura) {
    unsigned char candidato;
    if (novaLeitura) {
        candidato = AceleracaoAtiva;
        if (AceleracaoAtiva) {
            // Histerese de 3% ao retornar ao neutro.
            if (FreioInvertido) {
                if (pulso <= Limite2Soltar) candidato = 0;
            } else {
                if (pulso >= Limite2SoltarInv) candidato = 0;
            }
        } else {
            // Aceleracao e oposta ao sentido de frenagem.
            if (FreioInvertido) {
                if (pulso > Max2) candidato = 1;
            } else {
                if (pulso < Min2) candidato = 1;
            }
        }
        if (candidato == AceleracaoAtiva) {
            ContadorAceleracao = 0;
        } else {
            if (candidato != CandidatoAceleracao) {
                CandidatoAceleracao = candidato;
                ContadorAceleracao = 0;
            }
            if (++ContadorAceleracao >= 3) {
                AceleracaoAtiva = candidato;
                ContadorAceleracao = 0;
            }
        }
    }
    // Sobrescreve os PWM finais; efeito segue evoluindo normalmente.
    if (FarolAltoAceleracao && !ModoStrobo && AceleracaoAtiva) {
        NivelPWMFarois = 16;
        NivelPWMFarolEsquerdo = 16;
    }
}
#endif
