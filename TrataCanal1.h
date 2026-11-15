#ifndef TRATACANAL1_H
#define TRATACANAL1_H

// 3 leituras consecutivas para trocar de estado.
// 8% para acionar; 3% para liberar (histerese).
unsigned int TempoSetas;
unsigned char FaseSetas;
unsigned char DirecaoSeta;  // 0=nenhuma, 1=esquerda, 2=direita
unsigned char CandidatoSeta;
unsigned char ContadorSeta;
unsigned int TempoStrobo;
unsigned char PassoStrobo, BlocoStrobo, EfeitoAnterior;
unsigned char FadeNivel, FadeSubindo;

// PWM por software dos dois farois: Timer2 a 4 kHz,
// 16 passos => 250 Hz. Duty 10 (escala 0..255) vira 1/16.
volatile unsigned char FasePWMFarois;
volatile unsigned char NivelPWMFarois, NivelPWMFarolEsquerdo;

void FaroisPWM_Interrupcao() {
    FasePWMFarois++;
    if (FasePWMFarois >= 16) FasePWMFarois = 0;
    FarolDireito = (FasePWMFarois < NivelPWMFarois);
    FarolEsquerdo = (FasePWMFarois < NivelPWMFarolEsquerdo);
}

void TrataCanal1_Iniciar(unsigned int agora) {
    TempoSetas = agora;
    FasePWMFarois = 0;
    NivelPWMFarois = 0;
    NivelPWMFarolEsquerdo = 0;
    EfeitoAnterior = 0;
    BlocoStrobo = 0;
    FadeNivel = 0;
    FadeSubindo = 1;
    FaseSetas = 0;
    DirecaoSeta = 0;
    CandidatoSeta = 0;
    ContadorSeta = 0;
    PassoStrobo = 0;
    TempoStrobo = agora;
    SetaEsquerda = 0;
    SetaDireita = 0;
}

void TrataCanal1(unsigned int pulso, unsigned int agora,
                 unsigned char novaLeitura) {
    unsigned char candidato;

    // Dois PWM independentes permitem efeitos alternados sem bloquear o radio.
    NivelPWMFarois = NivelBaseFarois;
    NivelPWMFarolEsquerdo = NivelBaseFarois;

    if (EfeitoAnterior != ModoStrobo) {
        EfeitoAnterior = ModoStrobo;
        PassoStrobo = 0;
        BlocoStrobo = 0;
        FadeNivel = 0;
        FadeSubindo = 1;
        TempoStrobo = agora;
    }

    if (ModoStrobo >= 1 && ModoStrobo <= 3) {
        // Dez piscadas por bloco, 25 ms ON / 25 ms OFF.
        // Pausa de 150 ms no efeito 1 e 50 ms nos efeitos 2 e 3.
        if ((unsigned int)(agora - TempoStrobo) >=
            (PassoStrobo == 20 ? (ModoStrobo == 1 ? 150 : 50) : 25)) {
            TempoStrobo = agora;
            if (++PassoStrobo == 21) {
                PassoStrobo = 0;
                if (ModoStrobo == 2) {
                    BlocoStrobo ^= 1;
                } else if (ModoStrobo == 3) {
                    if (++BlocoStrobo == 6) BlocoStrobo = 0;
                }
            }
        }
        NivelPWMFarois = 0;
        NivelPWMFarolEsquerdo = 0;
        if (PassoStrobo < 20 && !(PassoStrobo & 1)) {
            if (ModoStrobo == 1) {
                NivelPWMFarois = 16;
                NivelPWMFarolEsquerdo = 16;
            } else if (ModoStrobo == 2) {
                NivelPWMFarois = (BlocoStrobo == 0) ? 16 : 0;
                NivelPWMFarolEsquerdo = (BlocoStrobo == 1) ? 16 : 0;
            } else {
                // D, E, ambos, E, D, ambos.
                NivelPWMFarois = (BlocoStrobo == 0 || BlocoStrobo == 2 ||
                                  BlocoStrobo == 4 || BlocoStrobo == 5) ? 16 : 0;
                NivelPWMFarolEsquerdo = (BlocoStrobo == 1 || BlocoStrobo == 2 ||
                                        BlocoStrobo == 3 || BlocoStrobo == 5) ? 16 : 0;
            }
        }
    } else if (ModoStrobo == 4) {
        // Fade 0..16..0, um passo a cada 32 ms: ciclo ~1,02 s.
        // Resolucao limitada aos 16 niveis de PWM existentes no PIC.
        if ((unsigned int)(agora - TempoStrobo) >= 32) {
            TempoStrobo = agora;
            if (FadeSubindo) {
                if (++FadeNivel >= 16) FadeSubindo = 0;
            } else {
                if (--FadeNivel == 0) FadeSubindo = 1;
            }
        }
        NivelPWMFarois = FadeNivel;
        NivelPWMFarolEsquerdo = FadeNivel;
    }

    // Avaliar apenas uma vez por pulso RC recebido, nunca por giro do while.
    if (novaLeitura && !PiscaAlerta) {
        candidato = DirecaoSeta;

        if (DirecaoSeta == 1) {
            if (pulso >= (Media1 - Margem1Soltar)) candidato = 0;
        } else if (DirecaoSeta == 2) {
            if (pulso <= (Media1 + Margem1Soltar)) candidato = 0;
        } else {
            if (pulso < Min1) candidato = 1;
            else if (pulso > Max1) candidato = 2;
            else candidato = 0;
        }

        if (candidato == DirecaoSeta) {
            ContadorSeta = 0;
        } else {
            if (candidato != CandidatoSeta) {
                CandidatoSeta = candidato;
                ContadorSeta = 0;
            }
            if (++ContadorSeta == 3) {
                DirecaoSeta = candidato;
                ContadorSeta = 0;
                if (DirecaoSeta) {
                    FaseSetas = 1;
                    TempoSetas = agora;
                }
            }
        }
    }

    if ((unsigned int)(agora - TempoSetas) >= 500) {
        TempoSetas = agora;
        FaseSetas = !FaseSetas;
    }
    if (PiscaAlerta) {
        SetaEsquerda = FaseSetas;
        SetaDireita = FaseSetas;
    } else {
        SetaEsquerda = FaseSetas && (DirecaoSeta == (SetasInvertidas ? 2 : 1));
        SetaDireita = FaseSetas && (DirecaoSeta == (SetasInvertidas ? 1 : 2));
    }
}
#endif
