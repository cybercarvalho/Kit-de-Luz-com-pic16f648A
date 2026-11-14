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

    if (novaLeitura) {
        EstadoFreio = FreioAtivo;
        // Canal 2 HIGH = freio, LOW = aceleracao.
        // Histerese: aciona em Max2 e solta em Limite2Soltar.
        if (!FreioAtivo) {
            if (pulso > Max2) EstadoFreio = 1;
        } else {
            if (pulso < Limite2Soltar) EstadoFreio = 0;
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

        // LOW (aceleracao) reinicia a sequencia de frenagens.
        // O neutro nao altera EstadoRe.
        if (pulso < Min2) {
            EstadoRe = 0;
            FreioAnteriorRe = 0;
        } else {
            // Conta apenas a transicao para freio confirmado.
            // O filtro de 3 leituras acima evita contar ruido.
            if ((pulso > Max2) && FreioAtivo &&
                !FreioAnteriorRe && (EstadoRe < 2))
                EstadoRe++;

            // Memoriza o estado confirmado para detectar
            // a proxima frenagem depois de voltar ao neutro.
            FreioAnteriorRe = FreioAtivo;
        }

        // Segunda frenagem liga a re; neutro a mantem ligada.
        Re_Lanterna = (EstadoRe == 2);
    }

    // Unico ponto de controle do PWM no modo normal.
    // Re ativa: mantem iluminacao base, mesmo durante frenagem.
    if ((EstadoRe == 1) && FreioAtivo)
        FreioPWM(PWM_ACIONADO);
    else
        FreioPWM(Programa == 0 ? 0 : 10);
}
#endif
