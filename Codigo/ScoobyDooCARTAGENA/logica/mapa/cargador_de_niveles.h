#ifndef CARGADOR_DE_NIVELES_H
#define CARGADOR_DE_NIVELES_H

#include <QString>
#include <QJsonObject>
#include <vector>
#include "mapa/mapa.h"

struct AmigoInicial {
    QString           rolTexto;
    CoordenadaCasilla casilla;
};

struct CiudadanoInicial {
    QString           claveDialogo;
    CoordenadaCasilla casilla;
};

struct SecuazInicial {
    bool                           esRutaFija = false;
    std::vector<CoordenadaCasilla> puntosDeControl;
};

struct DatosInicialesDelNivel {
    CoordenadaCasilla              casillaInicialScooby;
    std::vector<AmigoInicial>      amigos;
    std::vector<CiudadanoInicial>  ciudadanos;
    std::vector<SecuazInicial>     secuaces;
    CoordenadaCasilla              casillaCentinelaAlmacen;
    double                         radioCentinelaAlmacen = 0.0;
    CoordenadaCasilla              casillaAlmacen;
    CoordenadaCasilla              casillaLampara;
    std::vector<CoordenadaCasilla> casillasCanones;
};

namespace CargadorDeNiveles {

    void cargarNivelDesdeArchivos(const QString&          rutaTerreno,
                                  const QString&          rutaDetalles,
                                  int                     numeroDeNivel,
                                  Mapa&                   mapaDestino,
                                  DatosInicialesDelNivel& datosSalida);

    QJsonObject leerBancoDeDatos(const QString& rutaDelBanco);

}

#endif
