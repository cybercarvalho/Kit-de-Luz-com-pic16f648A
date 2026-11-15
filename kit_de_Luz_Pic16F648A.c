#include "Configuracao.h"
#include "RC_Canais.h"
#include "Freio_Setas.h"
#include "TiraMedia.h"
#include "EEPROM_Freio.h"
#include "TrataCanal3.h"
#include "TrataCanal1.h"
#include "TrataCanal2.h"
#include "TrataLuzRe.h"
#include "FailSafe.h"

void AtualizarNeon() {
    if (BrakeLight)
        Neon_BrakeLight = (CCPR1L == PWM_ACIONADO);
    else
        Neon_BrakeLight = Neon;
}

void TrataCanal3_Sinalizar() {
    unsigned char setas, farois;
    setas = LuzProg & 1;
    farois = (LuzProg >> 1) & 1;
    NivelPWMFarois = farois ? 16 : 0;
    NivelPWMFarolEsquerdo = NivelPWMFarois;
    SetaEsquerda = setas;
    SetaDireita = setas;
    Neon_BrakeLight = farois;
    Re_Lanterna = farois;
    FreioPWM(farois ? 255 : 0);
}

volatile unsigned int Milissegundos;

void interrupt() {
    if (PIR1.TMR2IF && PIE1.TMR2IE) {
        PIR1.TMR2IF = 0;
        FaroisPWM_Interrupcao();
    }
    if (INTCON.RBIF) RC_Interrupcao();
    if (INTCON.T0IF) {
        TMR0 += 6;  // ~1 ms com 4 MHz e prescaler 1:4
        INTCON.T0IF = 0;
        Milissegundos++;
        if (Idade1 != 255) Idade1++;
        if (Idade2 != 255) Idade2++;
        if (Idade3 != 255) Idade3++;
    }
}

void main() {
    unsigned int c1, c2, c3, agora;
    unsigned char novos, perdido;
    unsigned char ModoProgAnterior = 0;

    ConfigurarPortas();
    Milissegundos = 0;
    OPTION_REG = 0x81;  // Timer0, Fosc/4, prescaler 1:4
    TMR0 = 6;
    INTCON.T0IF = 0;
    INTCON.T0IE = 1;
    RC_Inicializar();
    FreioInicializar();
    LuzRe_Reiniciar();
    FailSafe_Inicializar();
    TrataCanal3_Iniciar(0);
    INTCON.PEIE = 1;
    INTCON.GIE = 1;

    while (1) {
        INTCON.GIE = 0;
        c1 = ValorCanal1; c2 = ValorCanal2; c3 = ValorCanal3;
        novos = NovosRC;
        NovosRC = 0;
        perdido = FailSafe_SinalPerdido(Idade1, Idade2, Idade3);
        agora = Milissegundos;
        INTCON.GIE = 1;

        if (perdido) {
            NivelPWMFarois = 0;
            NivelPWMFarolEsquerdo = 0;
            Neon_BrakeLight = 0;
            LuzRe_Reiniciar();
            TrataCanal3_Cancelar(agora);
            ModoProgAnterior = 0;
            FailSafe_Executar(agora);
            continue;
        }
        FailSafe_Recuperou();
        if (EstadoSistema == CALIBRANDO) {
            TiraMedia_Atualizar(novos, c1, c2, agora);
            continue;
        }
        if (EstadoSistema == CONFIRMANDO) {
            TiraMedia_Confirmacao(agora);
            TrataCanal1_Iniciar(agora);
            TrataCanal2_Iniciar();
            LuzRe_Reiniciar();
            TrataCanal3_Cancelar(agora);
            ModoProgAnterior = 0;
            continue;
        }
        TrataCanal3(c3, c2, c1, agora, novos & 2, novos & 1);
        if (ModoProg || ModoProgAnterior) {
            FreioAtivo = 0;
            FiltroRuidoFReio = 0;
            ContadorFreio = 0;
            LuzRe_Reiniciar();
        }
        ModoProgAnterior = ModoProg;
        if (ModoProg) {
            TrataCanal3_Sinalizar();
            continue;
        }
        ConfigurarPrograma();
        TrataCanal1(c1, agora, novos & 1);
        TrataCanal2(c2, novos & 2);
        TrataLuzRe(c2, novos & 2);
        // A luz de re define se o PWM de frenagem deve ser pleno.
        if (EstadoRe != 1 || !FreioAtivo)
            FreioPWM(PWMBaseFreio);
        AtualizarNeon();
    }
}
