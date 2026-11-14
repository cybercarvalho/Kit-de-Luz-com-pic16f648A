#ifndef TRATACANAL2_H
#define TRATACANAL2_H
unsigned char FreioAtivo;
unsigned char FiltroRuidoFReio;
unsigned char ContadorFreio;
// Estado compartilhado com TrataLuzRe.h; declarado antes do primeiro uso.
unsigned char EstadoRe;

void TrataCanal2_Iniciar() {
    FreioAtivo = 0;
    FiltroRuidoFReio = 0;
    ContadorFreio = 0;
    FreioPWM(0);
}

void TrataCanal2(unsigned int pulso, unsigned char novaLeitura) {
    unsigned char EstadoFreio;
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
            if (EstadoFreio != FiltroRuidoFReio) {
                FiltroRuidoFReio = EstadoFreio;
                ContadorFreio = 0;
            }
            if (++ContadorFreio == 3) {
                FreioAtivo = EstadoFreio;
                ContadorFreio = 0;
            }
        }
    }
    if (FreioAtivo && (EstadoRe == 1)) FreioPWM(PWM_ACIONADO);
    else FreioPWM(PWMBaseFreio);
}
#endif
