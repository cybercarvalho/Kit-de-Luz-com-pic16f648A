#ifndef TRATACANAL3_H
#define TRATACANAL3_H
// ModoProg: 0=normal, 1=sequencia inicial, 2=apagado 2s,
// 3=pisca 100ms, 4=apagado apos confirmar inversao.
// LuzProg: 0=apagado, 1=setas, 2=farois+freio, 3=todas.
unsigned char Programa, ModoProg, Ch3Estado, Ch2Armado, Ch2Altos, Ch1Armado, Ch1Altos, Ch2BaixoArmado, Ch2Baixos;
unsigned char FreioInvertido, SetasInvertidas, BrakeLight, LuzProg, PassoLuz;
unsigned int TempoCh3, TempoProg;

void TrataCanal3_Iniciar(unsigned int agora) {
    Programa = 0;
    ModoProg = 0;
    Ch3Estado = 0;
    Ch2Armado = 0;
        Ch1Armado = 0;
        Ch1Altos = 0;
    Ch2Altos = 0;
    Ch2BaixoArmado = 0;
    Ch2Baixos = 0;
    LuzProg = 0;
    PassoLuz = 0;
    TempoCh3 = agora;
    TempoProg = agora;
    FreioInvertido = (LerFreioEEPROM() == 0x5A);
    SetasInvertidas = (LerSetasEEPROM() == 0x5A);
    BrakeLight = (LerBrakeLightEEPROM() == 0x5A);
}
void TrataCanal3_Cancelar(unsigned int agora) {
    ModoProg = 0;
    Ch3Estado = 0;
    Ch2Armado = 0;
        Ch1Armado = 0;
        Ch1Altos = 0;
    Ch2Altos = 0;
    Ch2BaixoArmado = 0;
    Ch2Baixos = 0;
    LuzProg = 0;
    PassoLuz = 0;
    TempoCh3 = agora;
}
void TrataCanal3(unsigned int c3, unsigned int c2, unsigned int c1,
                 unsigned int agora, unsigned char novo2,
                 unsigned char novo1) {
    unsigned int intervalo;
    if (c3 <= 1500) {
        // Pulso curto baixo-alto-baixo troca programa.
        if ((Ch3Estado == 2) && (ModoProg == 1)) {
            Programa++;
            if (Programa == 6) Programa = 0;
        }
        Ch3Estado = 1;
        ModoProg = 0;
        Ch2Armado = 0;
        Ch1Armado = 0;
        Ch1Altos = 0;
    Ch2Altos = 0;
    Ch2BaixoArmado = 0;
    Ch2Baixos = 0;
        LuzProg = 0;
        PassoLuz = 0;
        return;
    }
    if (Ch3Estado != 2) {
        Ch3Estado = 2;
        TempoCh3 = agora;
        TempoProg = agora;
        ModoProg = 1;
        PassoLuz = 0;
        LuzProg = 1;
    }

    if (ModoProg == 1) {
        if ((unsigned int)(agora - TempoCh3) >= 10000) {
            ModoProg = 2;
            TempoProg = agora;
            LuzProg = 0;
            Ch2Armado = 0;
        Ch1Armado = 0;
        Ch1Altos = 0;
            Ch2Altos = 0;
    Ch2BaixoArmado = 0;
    Ch2Baixos = 0;
            return;
        }
        // 0-50ms: setas; 50-100ms: farois e freio;
        // 100-350ms: tudo apagado; repete enquanto C3 alto.
        intervalo = (PassoLuz == 2) ? 250 : 50;
        if ((unsigned int)(agora - TempoProg) >= intervalo) {
            TempoProg = agora;
            PassoLuz++;
            if (PassoLuz == 3) PassoLuz = 0;
            LuzProg = (PassoLuz == 0) ? 1 : (PassoLuz == 1) ? 2 : 0;
        }
        return;
    }
    // Aos 10s: todas apagadas por 2s antes da piscada rapida.
    if (ModoProg == 2) {
        LuzProg = 0;
        if ((unsigned int)(agora - TempoProg) >= 2000) {
            ModoProg = 3;
            TempoProg = agora;
            LuzProg = 3;
            Ch2Armado = 0;
        Ch1Armado = 0;
        Ch1Altos = 0;
            Ch2Altos = 0;
    Ch2BaixoArmado = 0;
    Ch2Baixos = 0;
        }
        return;
    }
    // Apos confirmar a inversao, todas as luzes ficam apagadas
    // ate o Canal 3 retornar a LOW.
    if (ModoProg == 4) {
        LuzProg = 0;
        return;
    }
    // Comando somente quando o kit esta aguardando programacao.
    // Exige Canal 2 baixo antes de uma nova transicao para alto.
    if (novo2) {
        // Limiar exclusivo da programacao: 10% acima da media.
        // O freio normal continua utilizando Max2 (8%).
        if (c2 <= Max2) {
            Ch2Armado = 1;
            Ch2Altos = 0;
    Ch2BaixoArmado = 0;
    Ch2Baixos = 0;
        }
        else if (c2 > Limite2Prog) {
            if (Ch2Armado) {
                Ch2Altos++;
                if (Ch2Altos >= 3) {
                    Ch2Armado = 0;
        Ch1Armado = 0;
        Ch1Altos = 0;
                    Ch2Altos = 0;
    Ch2BaixoArmado = 0;
    Ch2Baixos = 0;
                    FreioInvertido = !FreioInvertido;
                    SalvarFreioEEPROM(FreioInvertido);
                    ModoProg = 4;
                    LuzProg = 0;
                    return;
                }
            }
        }
        else {
            // Zona intermediaria: nao confirma HIGH.
            Ch2Altos = 0;
    Ch2BaixoArmado = 0;
    Ch2Baixos = 0;
        }
    }
    // Canal 2 abaixo de -10%: alterna a funcao Brake Light da RA2.
    // Armado no neutro; exige tres pulsos RC consecutivos no lado baixo.
    if (novo2) {
        if (c2 >= Min2 && c2 <= Max2) {
            Ch2BaixoArmado = 1;
            Ch2Baixos = 0;
        } else if (c2 < Limite2ProgBaixo) {
            if (Ch2BaixoArmado) {
                if (++Ch2Baixos >= 3) {
                    Ch2BaixoArmado = 0;
                    Ch2Baixos = 0;
                    BrakeLight = !BrakeLight;
                    SalvarBrakeLightEEPROM(BrakeLight);
                    ModoProg = 4;
                    LuzProg = 0;
                    return;
                }
            }
        } else {
            Ch2Baixos = 0;
        }
    }
    // Canal 1: mesmo procedimento do Canal 2, mas para inverter as setas.
    // Requer uma leitura em neutro/baixo para armar e 3 pulsos acima de +10%.
    if (novo1) {
        if (c1 <= Max1) {
            Ch1Armado = 1;
            Ch1Altos = 0;
        }
        else if (c1 > Limite1Prog) {
            if (Ch1Armado) {
                Ch1Altos++;
                if (Ch1Altos >= 3) {
                    Ch1Armado = 0;
                    Ch1Altos = 0;
                    SetasInvertidas = !SetasInvertidas;
                    SalvarSetasEEPROM(SetasInvertidas);
                    ModoProg = 4;
                    LuzProg = 0;
                    return;
                }
            }
        }
        else {
            Ch1Altos = 0;
        }
    }
    intervalo = 100;
    if ((unsigned int)(agora - TempoProg) >= intervalo) {
        TempoProg = agora;
        LuzProg = LuzProg ? 0 : 3;
    }
}
#endif
