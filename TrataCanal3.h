#ifndef TRATACANAL3_H
#define TRATACANAL3_H
// ModoProg: 0=normal, 1=sequencia inicial, 2=apagado 2s,
// 3=pisca 100ms, 4=apagado apos confirmar inversao.
// LuzProg: 0=apagado, 1=setas, 2=farois+freio, 3=todas.
unsigned char Programa, ModoProg, Ch3Estado, Ch2Armado, Ch2Altos, Ch1Armado, Ch1Altos, Ch2BaixoArmado, Ch2Baixos;
unsigned char FreioInvertido, SetasInvertidas, BrakeLight, LuzProg, PassoLuz;
unsigned int TempoCh3, TempoProg;

// Configuracao central dos treze programas.
// A execucao do strobo, setas e freio permanece nas rotinas dos canais.
unsigned char Neon, PWMBaseFreio, ModoStrobo, PiscaAlerta;
unsigned char NivelBaseFarois;

void ConfigurarPrograma() {
    switch (Programa) {
        case 0:
            NivelBaseFarois = 0;
            PWMBaseFreio = 0;
            Neon = 0;
            ModoStrobo = 0;
            PiscaAlerta = 0;
            break;
        case 1:
            NivelBaseFarois = 1;
            PWMBaseFreio = 10;
            Neon = 1;
            ModoStrobo = 0;
            PiscaAlerta = 0;
            break;
        case 2:
            NivelBaseFarois = 16;
            PWMBaseFreio = 10;
            Neon = 1;
            ModoStrobo = 0;
            PiscaAlerta = 0;
            break;
        case 3:
            NivelBaseFarois = 16;
            PWMBaseFreio = 10;
            Neon = 1;
            ModoStrobo = 0;
            PiscaAlerta = 0;
            break;
        case 4:
            NivelBaseFarois = 0;
            PWMBaseFreio = 10;
            Neon = 1;
            ModoStrobo = 1;
            PiscaAlerta = 0;
            break;
        case 5:
            NivelBaseFarois = 0;
            PWMBaseFreio = 10;
            Neon = 0;
            ModoStrobo = 2;
            PiscaAlerta = 0;
            break;
        case 6:
            NivelBaseFarois = 0;
            PWMBaseFreio = 10;
            Neon = 1;
            ModoStrobo = 3;
            PiscaAlerta = 0;
            break;
        case 7:
            NivelBaseFarois = 0;
            PWMBaseFreio = 10;
            Neon = 1;
            ModoStrobo = 4;
            PiscaAlerta = 0;
            break;
        case 8:
            NivelBaseFarois = 0;
            PWMBaseFreio = 10;
            Neon = 1;
            ModoStrobo = 1;
            PiscaAlerta = 1;
            break;
        case 9:
            NivelBaseFarois = 0;
            PWMBaseFreio = 10;
            Neon = 1;
            ModoStrobo = 2;
            PiscaAlerta = 1;
            break;
        case 10:
            NivelBaseFarois = 0;
            PWMBaseFreio = 10;
            Neon = 1;
            ModoStrobo = 3;
            PiscaAlerta = 1;
            break;
        case 11:
            NivelBaseFarois = 0;
            PWMBaseFreio = 10;
            Neon = 1;
            ModoStrobo = 4;
            PiscaAlerta = 1;
            break;
        case 12:
            NivelBaseFarois = 16;
            PWMBaseFreio = 10;
            Neon = 1;
            ModoStrobo = 0;
            PiscaAlerta = 1;
            break;
        default:
            Programa = 0;
            NivelBaseFarois = 0;
            PWMBaseFreio = 0;
            Neon = 0;
            ModoStrobo = 0;
            PiscaAlerta = 0;
            break;
    }
}


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
            if (Programa == 13) Programa = 0;
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
    // Programacao: Canal 2 +8% inverte freio, -8% alterna Brake Light.
    // Ambos exigem passagem pelo neutro e tres leituras consecutivas.
    if (novo2) {
        if (c2 > Limite2Prog) {
            Ch2Baixos = 0;
            if (Ch2Armado && ++Ch2Altos >= 3) {
                Ch2Armado = 0;
                Ch2Altos = 0;
                FreioInvertido = !FreioInvertido;
                SalvarFreioEEPROM(FreioInvertido);
                ModoProg = 4;
                LuzProg = 0;
                return;
            }
        } else if (c2 < Limite2ProgBaixo) {
            Ch2Altos = 0;
            if (Ch2BaixoArmado && ++Ch2Baixos >= 3) {
                Ch2BaixoArmado = 0;
                Ch2Baixos = 0;
                BrakeLight = !BrakeLight;
                SalvarBrakeLightEEPROM(BrakeLight);
                ModoProg = 4;
                LuzProg = 0;
                return;
            }
        } else {
            Ch2Armado = 1;
            Ch2BaixoArmado = 1;
            Ch2Altos = 0;
            Ch2Baixos = 0;
        }
    }
    // Canal 1 +8%: inverte as setas.
    if (novo1) {
        if (c1 <= Limite1Prog) {
            Ch1Armado = 1;
            Ch1Altos = 0;
        } else if (Ch1Armado && ++Ch1Altos >= 3) {
            Ch1Armado = 0;
            Ch1Altos = 0;
            SetasInvertidas = !SetasInvertidas;
            SalvarSetasEEPROM(SetasInvertidas);
            ModoProg = 4;
            LuzProg = 0;
            return;
        }
    }
    intervalo = 100;
    if ((unsigned int)(agora - TempoProg) >= intervalo) {
        TempoProg = agora;
        LuzProg = LuzProg ? 0 : 3;
    }
}
#endif
