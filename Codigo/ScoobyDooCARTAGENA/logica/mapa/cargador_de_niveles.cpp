#include "mapa/cargador_de_niveles.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QStringList>
#include <stdexcept>
#include <algorithm>

namespace {

    constexpr QChar   caracterParedSolida                    = QLatin1Char('#');
    constexpr QChar   caracterSueloTransitable               = QLatin1Char('.');
    constexpr QChar   caracterCaminoDeAdoquin                = QLatin1Char('+');
    constexpr QChar   caracterZonaRestringida                = QLatin1Char('R');
    constexpr QChar   caracterArbustoParaOcultarse           = QLatin1Char('A');
    constexpr QChar   caracterAguaMuralla                    = QLatin1Char('B');
    constexpr QChar   separadorDeEspacio                     = QLatin1Char(' ');
    constexpr int     numeroDeNivelTutorial                  = 1;
    constexpr uint8_t zonaInicialDesbloqueada                = 1u;
    constexpr int     zonaMaximaPermitidaEnCuatroBits        = 15;
    constexpr int     cantidadMinimaDimensionTerreno         = 3;
    constexpr int     indiceComandoEnLinea                   = 0;
    constexpr int     cantidadParametrosScooby               = 3;
    constexpr int     cantidadParametrosAmigo                = 4;
    constexpr int     cantidadParametrosCiudadano            = 4;
    constexpr int     cantidadMinimaParametrosSecuaz         = 4;
    constexpr int     indiceTipoPatrullaSecuaz               = 1;
    constexpr int     indiceInicioCoordenadasSecuaz          = 2;
    constexpr int     cantidadParametrosSecuazLibre          = 4;
    constexpr int     cantidadParametrosCentinela            = 4;
    constexpr int     cantidadParametrosAlmacen              = 3;
    constexpr int     cantidadParametrosLampara              = 3;
    constexpr int     cantidadParametrosCanon                = 3;
    constexpr int     cantidadParametrosZona                 = 6;
    constexpr int     cantidadParametrosSector               = 6;
    constexpr int     cantidadParametrosReferencia           = 4;
    constexpr int     pasoDeParesDeCoordenadas               = 2;

    [[noreturn]] void lanzarErrorConArchivoYLinea(const QString& rutaArchivo,
                                                  int            numeroDeLinea,
                                                  const QString& detalleDelError) {
        QString mensajeCompleto = QStringLiteral("Error en archivo '%1' linea %2: %3")
                                      .arg(rutaArchivo)
                                      .arg(numeroDeLinea)
                                      .arg(detalleDelError);
        throw std::runtime_error(mensajeCompleto.toStdString());
    }

    int convertirTextoAEnteroValidado(const QString& texto,
                                      const QString& rutaArchivo,
                                      int            numeroDeLinea) {
        bool conversionExitosa = false;
        int  valorConvertido   = texto.toInt(&conversionExitosa);
        if (!conversionExitosa) {
            lanzarErrorConArchivoYLinea(
                rutaArchivo,
                numeroDeLinea,
                QStringLiteral("Se esperaba un numero entero valido pero se encontro '%1'.").arg(texto));
        }
        return valorConvertido;
    }

    double convertirTextoADecimalValidado(const QString& texto,
                                          const QString& rutaArchivo,
                                          int            numeroDeLinea) {
        bool   conversionExitosa = false;
        double valorConvertido   = texto.toDouble(&conversionExitosa);
        if (!conversionExitosa) {
            lanzarErrorConArchivoYLinea(
                rutaArchivo,
                numeroDeLinea,
                QStringLiteral("Se esperaba un numero decimal valido pero se encontro '%1'.").arg(texto));
        }
        return valorConvertido;
    }

    void validarCantidadExactaDeParametros(const QStringList& partes,
                                           int                cantidadEsperada,
                                           const QString&     rutaArchivo,
                                           int                numeroDeLinea) {
        if (partes.size() != cantidadEsperada) {
            lanzarErrorConArchivoYLinea(
                rutaArchivo,
                numeroDeLinea,
                QStringLiteral("Cantidad invalida de parametros para el comando '%1'.")
                    .arg(partes.value(indiceComandoEnLinea)));
        }
    }

    void validarCoordenadaDentroDelMapa(const Mapa&       mapa,
                                        CoordenadaCasilla casilla,
                                        const QString&    rutaArchivo,
                                        int               numeroDeLinea) {
        if (!mapa.esCasillaValida(casilla.columna, casilla.fila)) {
            lanzarErrorConArchivoYLinea(
                rutaArchivo,
                numeroDeLinea,
                QStringLiteral("Coordenada (%1, %2) fuera de los limites del mapa.")
                    .arg(casilla.columna)
                    .arg(casilla.fila));
        }
    }

    void comprobarBordesCerrados(const Mapa& mapa, const QString& rutaTerreno) {
        int ancho = mapa.obtenerAnchoEnCasillas();
        int alto  = mapa.obtenerAltoEnCasillas();

        for (int columna = 0; columna < ancho; ++columna) {
            CasillaDelMapa bordeSuperior = mapa.obtenerCasilla(columna, 0);
            CasillaDelMapa bordeInferior = mapa.obtenerCasilla(columna, alto - 1);
            bool superiorCerrado = (bordeSuperior.terreno == TipoDeTerreno::ParedSolida
                                 || bordeSuperior.terreno == TipoDeTerreno::AguaMuralla);
            bool inferiorCerrado = (bordeInferior.terreno == TipoDeTerreno::ParedSolida
                                 || bordeInferior.terreno == TipoDeTerreno::AguaMuralla);
            if (!superiorCerrado || !inferiorCerrado) {
                lanzarErrorConArchivoYLinea(
                    rutaTerreno,
                    superiorCerrado ? alto : 1,
                    QStringLiteral("El borde horizontal exterior del mapa debe estar completamente cerrado."));
            }
        }

        for (int fila = 0; fila < alto; ++fila) {
            CasillaDelMapa bordeIzquierdo = mapa.obtenerCasilla(0, fila);
            CasillaDelMapa bordeDerecho   = mapa.obtenerCasilla(ancho - 1, fila);
            bool izquierdoCerrado = (bordeIzquierdo.terreno == TipoDeTerreno::ParedSolida
                                  || bordeIzquierdo.terreno == TipoDeTerreno::AguaMuralla);
            bool derechoCerrado   = (bordeDerecho.terreno == TipoDeTerreno::ParedSolida
                                  || bordeDerecho.terreno == TipoDeTerreno::AguaMuralla);
            if (!izquierdoCerrado || !derechoCerrado) {
                lanzarErrorConArchivoYLinea(
                    rutaTerreno,
                    fila + 1,
                    QStringLiteral("El borde vertical exterior del mapa debe estar completamente cerrado."));
            }
        }
    }

    void procesarArchivoTerreno(const QString& rutaTerreno, Mapa& mapaDestino) {
        QFile archivoTerreno(rutaTerreno);
        if (!archivoTerreno.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("No se pudo abrir el archivo de terreno en la ruta especificada.");
        }

        QTextStream flujoEntrada(&archivoTerreno);
        std::vector<QString> lineasLimpias;
        while (!flujoEntrada.atEnd()) {
            QString lineaLeida = flujoEntrada.readLine().trimmed();
            if (!lineaLeida.isEmpty()) {
                lineasLimpias.push_back(lineaLeida);
            }
        }

        if (static_cast<int>(lineasLimpias.size()) < cantidadMinimaDimensionTerreno) {
            throw std::runtime_error("El archivo de terreno esta vacio o es demasiado pequeno.");
        }

        int alto  = static_cast<int>(lineasLimpias.size());
        int ancho = static_cast<int>(lineasLimpias[0].size());
        if (ancho < cantidadMinimaDimensionTerreno) {
            throw std::runtime_error("El ancho del archivo de terreno es demasiado pequeno.");
        }

        mapaDestino.inicializarDimensiones(ancho, alto);

        for (int fila = 0; fila < alto; ++fila) {
            const QString& lineaActual = lineasLimpias[static_cast<std::size_t>(fila)];
            if (static_cast<int>(lineaActual.size()) != ancho) {
                lanzarErrorConArchivoYLinea(
                    rutaTerreno,
                    fila + 1,
                    QStringLiteral("Todas las filas del terreno deben tener exactamente el mismo ancho."));
            }

            for (int columna = 0; columna < ancho; ++columna) {
                QChar           simbolo = lineaActual.at(columna);
                CasillaDelMapa& casilla = mapaDestino.modificarCasilla(columna, fila);
                casilla.numeroDeZona       = zonaInicialDesbloqueada;
                casilla.tienePasoBloqueado = Mapa::banderaApagada;

                if (simbolo == caracterParedSolida) {
                    casilla.terreno       = TipoDeTerreno::ParedSolida;
                    casilla.esTransitable = Mapa::banderaApagada;
                } else if (simbolo == caracterSueloTransitable) {
                    casilla.terreno       = TipoDeTerreno::SueloTransitable;
                    casilla.esTransitable = Mapa::banderaEncendida;
                } else if (simbolo == caracterCaminoDeAdoquin) {
                    casilla.terreno       = TipoDeTerreno::CaminoDeAdoquin;
                    casilla.esTransitable = Mapa::banderaEncendida;
                } else if (simbolo == caracterZonaRestringida) {
                    casilla.terreno           = TipoDeTerreno::SueloTransitable;
                    casilla.esTransitable     = Mapa::banderaEncendida;
                    casilla.esZonaRestringida = Mapa::banderaEncendida;
                } else if (simbolo == caracterArbustoParaOcultarse) {
                    casilla.terreno                = TipoDeTerreno::SueloTransitable;
                    casilla.esTransitable          = Mapa::banderaEncendida;
                    casilla.esArbustoParaOcultarse = Mapa::banderaEncendida;
                    mapaDestino.agregarArbusto(CoordenadaCasilla{ columna, fila });
                } else if (simbolo == caracterAguaMuralla) {
                    casilla.terreno       = TipoDeTerreno::AguaMuralla;
                    casilla.esTransitable = Mapa::banderaApagada;
                } else {
                    lanzarErrorConArchivoYLinea(
                        rutaTerreno,
                        fila + 1,
                        QStringLiteral("Caracter desconocido '%1' en matriz de terreno.").arg(simbolo));
                }
            }
        }

        comprobarBordesCerrados(mapaDestino, rutaTerreno);
    }

    void validarEntidadEnCasillaTransitable(const Mapa&       mapa,
                                            CoordenadaCasilla casilla,
                                            const QString&    nombreEntidad) {
        if (!mapa.esCasillaValida(casilla.columna, casilla.fila)
            || mapa.obtenerCasilla(casilla.columna, casilla.fila).esTransitable == Mapa::banderaApagada) {
            QString mensaje = QStringLiteral("Entidad '%1' generada sobre obstaculo solido o fuera del mapa en (%2, %3).")
                                  .arg(nombreEntidad)
                                  .arg(casilla.columna)
                                  .arg(casilla.fila);
            throw std::runtime_error(mensaje.toStdString());
        }
    }

    void procesarArchivoDetalles(const QString&          rutaDetalles,
                                 int                     numeroDeNivel,
                                 Mapa&                   mapaDestino,
                                 DatosInicialesDelNivel& datosSalida,
                                 bool&                   scoobyFueConfigurado,
                                 bool&                   centinelaFueConfigurado) {
        QFile archivoDetalles(rutaDetalles);
        if (!archivoDetalles.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("No se pudo abrir el archivo de detalles del nivel.");
        }

        datosSalida = DatosInicialesDelNivel{};
        scoobyFueConfigurado    = false;
        centinelaFueConfigurado = false;

        QTextStream flujoEntrada(&archivoDetalles);
        int         numeroDeLinea = 0;

        while (!flujoEntrada.atEnd()) {
            ++numeroDeLinea;
            QString lineaActual = flujoEntrada.readLine().trimmed();
            if (lineaActual.isEmpty()) {
                continue;
            }

            QStringList partes  = lineaActual.split(separadorDeEspacio, Qt::SkipEmptyParts);
            QString     comando = partes[indiceComandoEnLinea];

            if (comando == QStringLiteral("SCOOBY")) {
                validarCantidadExactaDeParametros(partes, cantidadParametrosScooby, rutaDetalles, numeroDeLinea);
                int columna = convertirTextoAEnteroValidado(partes[1], rutaDetalles, numeroDeLinea);
                int fila    = convertirTextoAEnteroValidado(partes[2], rutaDetalles, numeroDeLinea);
                CoordenadaCasilla casilla{ columna, fila };
                validarCoordenadaDentroDelMapa(mapaDestino, casilla, rutaDetalles, numeroDeLinea);
                datosSalida.casillaInicialScooby = casilla;
                scoobyFueConfigurado             = true;
            } else if (comando == QStringLiteral("AMIGO")) {
                validarCantidadExactaDeParametros(partes, cantidadParametrosAmigo, rutaDetalles, numeroDeLinea);
                int columna = convertirTextoAEnteroValidado(partes[2], rutaDetalles, numeroDeLinea);
                int fila    = convertirTextoAEnteroValidado(partes[3], rutaDetalles, numeroDeLinea);
                CoordenadaCasilla casilla{ columna, fila };
                validarCoordenadaDentroDelMapa(mapaDestino, casilla, rutaDetalles, numeroDeLinea);
                datosSalida.amigos.push_back(AmigoInicial{ partes[1], casilla });
            } else if (comando == QStringLiteral("CIUDADANO")) {
                validarCantidadExactaDeParametros(partes, cantidadParametrosCiudadano, rutaDetalles, numeroDeLinea);
                int columna = convertirTextoAEnteroValidado(partes[2], rutaDetalles, numeroDeLinea);
                int fila    = convertirTextoAEnteroValidado(partes[3], rutaDetalles, numeroDeLinea);
                CoordenadaCasilla casilla{ columna, fila };
                validarCoordenadaDentroDelMapa(mapaDestino, casilla, rutaDetalles, numeroDeLinea);
                datosSalida.ciudadanos.push_back(CiudadanoInicial{ partes[1], casilla });
            } else if (comando == QStringLiteral("SECUAZ")) {
                if (partes.size() < cantidadMinimaParametrosSecuaz) {
                    lanzarErrorConArchivoYLinea(rutaDetalles, numeroDeLinea, QStringLiteral("Comando SECUAZ incompleto."));
                }
                QString       tipoPatrulla = partes[indiceTipoPatrullaSecuaz];
                SecuazInicial secuazNuevo;

                if (tipoPatrulla == QStringLiteral("RUTA_FIJA")) {
                    int cantidadCoordenadas = static_cast<int>(partes.size()) - indiceInicioCoordenadasSecuaz;
                    if (cantidadCoordenadas < pasoDeParesDeCoordenadas || (cantidadCoordenadas % pasoDeParesDeCoordenadas) != 0) {
                        lanzarErrorConArchivoYLinea(
                            rutaDetalles,
                            numeroDeLinea,
                            QStringLiteral("Los puntos de control de RUTA_FIJA deben venir en pares columna fila."));
                    }
                    secuazNuevo.esRutaFija = true;
                    for (int indice = indiceInicioCoordenadasSecuaz; indice < partes.size(); indice += pasoDeParesDeCoordenadas) {
                        int columna = convertirTextoAEnteroValidado(partes[indice], rutaDetalles, numeroDeLinea);
                        int fila    = convertirTextoAEnteroValidado(partes[indice + 1], rutaDetalles, numeroDeLinea);
                        CoordenadaCasilla punto{ columna, fila };
                        validarCoordenadaDentroDelMapa(mapaDestino, punto, rutaDetalles, numeroDeLinea);
                        secuazNuevo.puntosDeControl.push_back(punto);
                    }
                } else if (tipoPatrulla == QStringLiteral("PATRULLA_LIBRE")) {
                    validarCantidadExactaDeParametros(partes, cantidadParametrosSecuazLibre, rutaDetalles, numeroDeLinea);
                    int columna = convertirTextoAEnteroValidado(partes[2], rutaDetalles, numeroDeLinea);
                    int fila    = convertirTextoAEnteroValidado(partes[3], rutaDetalles, numeroDeLinea);
                    CoordenadaCasilla puntoInicial{ columna, fila };
                    validarCoordenadaDentroDelMapa(mapaDestino, puntoInicial, rutaDetalles, numeroDeLinea);
                    secuazNuevo.esRutaFija = false;
                    secuazNuevo.puntosDeControl.push_back(puntoInicial);
                } else {
                    lanzarErrorConArchivoYLinea(
                        rutaDetalles,
                        numeroDeLinea,
                        QStringLiteral("Modo de patrulla desconocido '%1' en comando SECUAZ.").arg(tipoPatrulla));
                }

                datosSalida.secuaces.push_back(std::move(secuazNuevo));
            } else if (comando == QStringLiteral("CENTINELA_ALMACEN")) {
                validarCantidadExactaDeParametros(partes, cantidadParametrosCentinela, rutaDetalles, numeroDeLinea);
                int    columna = convertirTextoAEnteroValidado(partes[1], rutaDetalles, numeroDeLinea);
                int    fila    = convertirTextoAEnteroValidado(partes[2], rutaDetalles, numeroDeLinea);
                double radio   = convertirTextoADecimalValidado(partes[3], rutaDetalles, numeroDeLinea);
                CoordenadaCasilla casilla{ columna, fila };
                validarCoordenadaDentroDelMapa(mapaDestino, casilla, rutaDetalles, numeroDeLinea);
                datosSalida.casillaCentinelaAlmacen = casilla;
                datosSalida.radioCentinelaAlmacen   = radio;
                centinelaFueConfigurado             = true;
            } else if (comando == QStringLiteral("ALMACEN")) {
                validarCantidadExactaDeParametros(partes, cantidadParametrosAlmacen, rutaDetalles, numeroDeLinea);
                int columna = convertirTextoAEnteroValidado(partes[1], rutaDetalles, numeroDeLinea);
                int fila    = convertirTextoAEnteroValidado(partes[2], rutaDetalles, numeroDeLinea);
                CoordenadaCasilla casilla{ columna, fila };
                validarCoordenadaDentroDelMapa(mapaDestino, casilla, rutaDetalles, numeroDeLinea);
                datosSalida.casillaAlmacen = casilla;
            } else if (comando == QStringLiteral("LAMPARA")) {
                validarCantidadExactaDeParametros(partes, cantidadParametrosLampara, rutaDetalles, numeroDeLinea);
                int columna = convertirTextoAEnteroValidado(partes[1], rutaDetalles, numeroDeLinea);
                int fila    = convertirTextoAEnteroValidado(partes[2], rutaDetalles, numeroDeLinea);
                CoordenadaCasilla casilla{ columna, fila };
                validarCoordenadaDentroDelMapa(mapaDestino, casilla, rutaDetalles, numeroDeLinea);
                datosSalida.casillaLampara = casilla;
            } else if (comando == QStringLiteral("CANON")) {
                validarCantidadExactaDeParametros(partes, cantidadParametrosCanon, rutaDetalles, numeroDeLinea);
                int columna = convertirTextoAEnteroValidado(partes[1], rutaDetalles, numeroDeLinea);
                int fila    = convertirTextoAEnteroValidado(partes[2], rutaDetalles, numeroDeLinea);
                CoordenadaCasilla casilla{ columna, fila };
                validarCoordenadaDentroDelMapa(mapaDestino, casilla, rutaDetalles, numeroDeLinea);
                datosSalida.casillasCanones.push_back(casilla);
            } else if (comando == QStringLiteral("ZONA")) {
                validarCantidadExactaDeParametros(partes, cantidadParametrosZona, rutaDetalles, numeroDeLinea);
                int numeroDeZonaEntero = convertirTextoAEnteroValidado(partes[1], rutaDetalles, numeroDeLinea);
                if (numeroDeZonaEntero < 0 || numeroDeZonaEntero > zonaMaximaPermitidaEnCuatroBits) {
                    lanzarErrorConArchivoYLinea(rutaDetalles, numeroDeLinea, QStringLiteral("El numero de zona debe estar entre 0 y 15."));
                }
                int columnaInicio = convertirTextoAEnteroValidado(partes[2], rutaDetalles, numeroDeLinea);
                int filaInicio    = convertirTextoAEnteroValidado(partes[3], rutaDetalles, numeroDeLinea);
                int columnaFin    = convertirTextoAEnteroValidado(partes[4], rutaDetalles, numeroDeLinea);
                int filaFin       = convertirTextoAEnteroValidado(partes[5], rutaDetalles, numeroDeLinea);

                int columnaMinima = std::max(0, std::min(columnaInicio, columnaFin));
                int columnaMaxima = std::min(mapaDestino.obtenerAnchoEnCasillas() - 1, std::max(columnaInicio, columnaFin));
                int filaMinima    = std::max(0, std::min(filaInicio, filaFin));
                int filaMaxima    = std::min(mapaDestino.obtenerAltoEnCasillas() - 1, std::max(filaInicio, filaFin));

                uint8_t numeroDeZona     = static_cast<uint8_t>(numeroDeZonaEntero);
                bool    bloquearEnInicio = (numeroDeNivel == numeroDeNivelTutorial && numeroDeZona != zonaInicialDesbloqueada);

                for (int fila = filaMinima; fila <= filaMaxima; ++fila) {
                    for (int columna = columnaMinima; columna <= columnaMaxima; ++columna) {
                        CasillaDelMapa& casilla = mapaDestino.modificarCasilla(columna, fila);
                        casilla.numeroDeZona = numeroDeZona;
                        if (bloquearEnInicio && casilla.esTransitable == Mapa::banderaEncendida) {
                            casilla.tienePasoBloqueado = Mapa::banderaEncendida;
                        }
                    }
                }
            } else if (comando == QStringLiteral("SECTOR")) {
                validarCantidadExactaDeParametros(partes, cantidadParametrosSector, rutaDetalles, numeroDeLinea);
                int columnaInicio = convertirTextoAEnteroValidado(partes[2], rutaDetalles, numeroDeLinea);
                int filaInicio    = convertirTextoAEnteroValidado(partes[3], rutaDetalles, numeroDeLinea);
                int columnaFin    = convertirTextoAEnteroValidado(partes[4], rutaDetalles, numeroDeLinea);
                int filaFin       = convertirTextoAEnteroValidado(partes[5], rutaDetalles, numeroDeLinea);

                SectorDelMapa sector;
                sector.nombre = partes[1];
                sector.esquinaSuperiorIzquierda = CoordenadaCasilla{ std::min(columnaInicio, columnaFin), std::min(filaInicio, filaFin) };
                sector.esquinaInferiorDerecha   = CoordenadaCasilla{ std::max(columnaInicio, columnaFin), std::max(filaInicio, filaFin) };
                mapaDestino.agregarSector(sector);
            } else if (comando == QStringLiteral("REFERENCIA")) {
                validarCantidadExactaDeParametros(partes, cantidadParametrosReferencia, rutaDetalles, numeroDeLinea);
                int columna = convertirTextoAEnteroValidado(partes[2], rutaDetalles, numeroDeLinea);
                int fila    = convertirTextoAEnteroValidado(partes[3], rutaDetalles, numeroDeLinea);
                CoordenadaCasilla casilla{ columna, fila };
                validarCoordenadaDentroDelMapa(mapaDestino, casilla, rutaDetalles, numeroDeLinea);
                mapaDestino.agregarReferencia(ReferenciaDelMapa{ partes[1], casilla });
            } else {
                lanzarErrorConArchivoYLinea(
                    rutaDetalles,
                    numeroDeLinea,
                    QStringLiteral("Comando desconocido '%1' en archivo de detalles.").arg(comando));
            }
        }
    }

    void comprobarQueNadieNazcaSobreObstaculos(const Mapa&                   mapa,
                                               const DatosInicialesDelNivel& datos,
                                               bool                          scoobyFueConfigurado,
                                               bool                          centinelaFueConfigurado) {
        if (!scoobyFueConfigurado) {
            throw std::runtime_error("El archivo de detalles no define la posicion inicial de SCOOBY.");
        }
        validarEntidadEnCasillaTransitable(mapa, datos.casillaInicialScooby, QStringLiteral("SCOOBY"));

        for (const AmigoInicial& amigo : datos.amigos) {
            validarEntidadEnCasillaTransitable(mapa, amigo.casilla, amigo.rolTexto);
        }
        for (const CiudadanoInicial& ciudadano : datos.ciudadanos) {
            validarEntidadEnCasillaTransitable(mapa, ciudadano.casilla, ciudadano.claveDialogo);
        }
        for (const SecuazInicial& secuaz : datos.secuaces) {
            for (const CoordenadaCasilla& punto : secuaz.puntosDeControl) {
                validarEntidadEnCasillaTransitable(mapa, punto, QStringLiteral("SECUAZ"));
            }
        }
        if (centinelaFueConfigurado) {
            validarEntidadEnCasillaTransitable(mapa, datos.casillaCentinelaAlmacen, QStringLiteral("CENTINELA_ALMACEN"));
        }
    }

}

namespace CargadorDeNiveles {

    void cargarNivelDesdeArchivos(const QString&          rutaTerreno,
                                  const QString&          rutaDetalles,
                                  int                     numeroDeNivel,
                                  Mapa&                   mapaDestino,
                                  DatosInicialesDelNivel& datosSalida) {
        procesarArchivoTerreno(rutaTerreno, mapaDestino);

        bool scoobyFueConfigurado    = false;
        bool centinelaFueConfigurado = false;
        procesarArchivoDetalles(
            rutaDetalles,
            numeroDeNivel,
            mapaDestino,
            datosSalida,
            scoobyFueConfigurado,
            centinelaFueConfigurado);

        comprobarQueNadieNazcaSobreObstaculos(
            mapaDestino,
            datosSalida,
            scoobyFueConfigurado,
            centinelaFueConfigurado);

        mapaDestino.marcarCasillasAlcanzablesDesde(datosSalida.casillaInicialScooby);
    }

    QJsonObject leerBancoDeDatos(const QString& rutaDelBanco) {
        QFile archivoJson(rutaDelBanco);
        if (!archivoJson.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("Fallo al abrir archivo de banco JSON.");
        }

        QByteArray      contenidoCrudo = archivoJson.readAll();
        QJsonParseError errorDeParseo;
        QJsonDocument   documento = QJsonDocument::fromJson(contenidoCrudo, &errorDeParseo);

        if (errorDeParseo.error != QJsonParseError::NoError || !documento.isObject()) {
            throw std::runtime_error("Sintaxis JSON corrupta en banco de datos.");
        }

        return documento.object();
    }

}
