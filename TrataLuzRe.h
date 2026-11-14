#ifndef TRATALUZRE_H
#define TRATALUZRE_H
// EstadoRe: 0=aguardando primeiro freio, 1=primeiro freio,
// 2=re acionada. Segue a logica do KitLuzArduino.
unsigned char EstadoRe;
unsigned char FreioAnteriorRe;

void LuzRe_Reiniciar() {
    EstadoRe = 0;
    FreioAnteriorRe = 0;
    Re_Lanterna = 0;
}

void TrataLuzRe(unsigned int pulso, unsigned char novaLeitura) {
    unsigned char acelerando;
    if (!novaLeitura) return;
    // O freio ja passou pelo filtro de 3 leituras do Canal 2.
    acelerando = FreioInvertido ? (pulso > Max2) : (pulso < Min2);
    if (acelerando) {
        EstadoRe = 0;
    } else if (FreioAtivo && !FreioAnteriorRe && EstadoRe < 2) {
        EstadoRe++;
    }
    FreioAnteriorRe = FreioAtivo;
    Re_Lanterna = (EstadoRe == 2);
}
#endif
