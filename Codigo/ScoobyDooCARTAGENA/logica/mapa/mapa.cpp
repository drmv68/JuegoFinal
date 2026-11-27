#include "mapa/mapa.h"
#include "herramientas/calculos_espaciales.h"
#include <queue>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <utility>

namespace {

    constexpr int cantidadVecinosOrtogonales = 4;
    constexpr int desplazamientoEnColumna[cantidadVecinosOrtogonales] = { 0, 0, 1, -1 };
    constexpr int desplazamientoEnFila[cantidadVecinosOrtogonales]    = { -1, 1, 0, 0 };
    constexpr int multiplicadorEnteroParaBresenham                  = 2;
    constexpr int costoInicialEnOrigen                              = 0;

    int calcularDistanciaManhattan(CoordenadaCasilla origen, CoordenadaCasilla destino) {
        return std::abs(destino.columna - origen.columna) + std::abs(destino.fila - origen.fila);
    }

}

Mapa::Mapa()
    : anchoEnCasillas(0)
    , altoEnCasillas(0)
    , numeroDeBusquedaAEstrellaActual(identificadorBusquedaInicial)
{}

void Mapa::inicializarDimensiones(int ancho, int alto) {
    assert(ancho >= 0);
    assert(alto >= 0);

    anchoEnCasillas = ancho;
    altoEnCasillas  = alto;

    int totalDeCasillas = ancho * alto;
    cuadriculaLineal.assign(totalDeCasillas, CasillaDelMapa{});
    bufferCostoG.assign(totalDeCasillas, costoInicialEnOrigen);
    bufferCostoF.assign(totalDeCasillas, costoInicialEnOrigen);
    bufferCameFrom.assign(totalDeCasillas, indiceSinPadreEnRuta);

    listaDeArbustos.clear();
    casillasAptasParaTrampa.clear();
    sectores.clear();
    referencias.clear();
    numeroDeBusquedaAEstrellaActual = identificadorBusquedaInicial;
}

int Mapa::obtenerAnchoEnCasillas() const {
    return anchoEnCasillas;
}

int Mapa::obtenerAltoEnCasillas() const {
    return altoEnCasillas;
}

CasillaDelMapa Mapa::obtenerCasilla(int columna, int fila) const {
    assert(esCasillaValida(columna, fila));
    return cuadriculaLineal[calcularIndiceLineal(columna, fila)];
}

CasillaDelMapa& Mapa::modificarCasilla(int columna, int fila) {
    assert(esCasillaValida(columna, fila));
    return cuadriculaLineal[calcularIndiceLineal(columna, fila)];
}

bool Mapa::esCasillaValida(int columna, int fila) const {
    return columna >= 0
        && columna < anchoEnCasillas
        && fila >= 0
        && fila < altoEnCasillas;
}

bool Mapa::esCasillaTransitableInterna(int columna, int fila, bool puedeEntrarAZonasRestringidas) const {
    if (!esCasillaValida(columna, fila)) {
        return false;
    }
    const CasillaDelMapa& casillaConsultada = cuadriculaLineal[calcularIndiceLineal(columna, fila)];
    if (casillaConsultada.esTransitable == banderaApagada || casillaConsultada.tienePasoBloqueado == banderaEncendida) {
        return false;
    }
    if (casillaConsultada.esZonaRestringida == banderaEncendida && !puedeEntrarAZonasRestringidas) {
        return false;
    }
    return true;
}

bool Mapa::esPosicionTransitable(double posicionX, double posicionY, bool puedeEntrarAZonasRestringidas) const {
    CoordenadaCasilla casillaEvaluada = CalculosEspaciales::convertirPosicionACasilla(posicionX, posicionY);
    return esCasillaTransitableInterna(casillaEvaluada.columna, casillaEvaluada.fila, puedeEntrarAZonasRestringidas);
}

bool Mapa::tieneLineaDeVista(CoordenadaCasilla origen, CoordenadaCasilla destino) const {
    if (!esCasillaValida(origen.columna, origen.fila) || !esCasillaValida(destino.columna, destino.fila)) {
        return false;
    }
    if (origen == destino) {
        return true;
    }

    int diferenciaEnColumnas = std::abs(destino.columna - origen.columna);
    int diferenciaEnFilas    = std::abs(destino.fila - origen.fila);
    int pasoEnColumna        = (origen.columna < destino.columna) ? costoPorPasoOrtogonal : -costoPorPasoOrtogonal;
    int pasoEnFila           = (origen.fila < destino.fila) ? costoPorPasoOrtogonal : -costoPorPasoOrtogonal;
    int errorDeTrazo         = diferenciaEnColumnas - diferenciaEnFilas;

    int columnaActual = origen.columna;
    int filaActual    = origen.fila;

    while (columnaActual != destino.columna || filaActual != destino.fila) {
        if (columnaActual != origen.columna || filaActual != origen.fila) {
            const CasillaDelMapa& casillaIntermedia = cuadriculaLineal[calcularIndiceLineal(columnaActual, filaActual)];
            if (casillaIntermedia.esTransitable == banderaApagada || casillaIntermedia.tienePasoBloqueado == banderaEncendida) {
                return false;
            }
        }

        int dobleErrorDeTrazo = multiplicadorEnteroParaBresenham * errorDeTrazo;
        if (dobleErrorDeTrazo > -diferenciaEnFilas) {
            errorDeTrazo  -= diferenciaEnFilas;
            columnaActual += pasoEnColumna;
        }
        if (dobleErrorDeTrazo < diferenciaEnColumnas) {
            errorDeTrazo += diferenciaEnColumnas;
            filaActual   += pasoEnFila;
        }
    }

    return true;
}

CoordenadaCasilla Mapa::calcularSiguientePasoHacia(CoordenadaCasilla origen,
                                                   CoordenadaCasilla destino,
                                                   bool puedeEntrarAZonasRestringidas) {
    if (origen == destino) {
        return origen;
    }
    if (!esCasillaValida(origen.columna, origen.fila) || !esCasillaValida(destino.columna, destino.fila)) {
        return origen;
    }

    ++numeroDeBusquedaAEstrellaActual;
    if (numeroDeBusquedaAEstrellaActual == identificadorBusquedaSinMarca) {
        numeroDeBusquedaAEstrellaActual = identificadorBusquedaInicial;
        for (CasillaDelMapa& casilla : cuadriculaLineal) {
            casilla.identificadorDeBusquedaAEstrella = identificadorBusquedaSinMarca;
        }
    }

    using EntradaColaPrioridad = std::pair<int, int>;
    std::vector<EntradaColaPrioridad> almacenamientoCola;
    almacenamientoCola.reserve(static_cast<std::size_t>(limiteMaximoNodosExploradosAEstrella));
    std::priority_queue<
        EntradaColaPrioridad,
        std::vector<EntradaColaPrioridad>,
        std::greater<EntradaColaPrioridad>
    > colaDePrioridad(std::greater<EntradaColaPrioridad>(), std::move(almacenamientoCola));

    int indiceOrigen  = calcularIndiceLineal(origen.columna, origen.fila);
    int indiceDestino = calcularIndiceLineal(destino.columna, destino.fila);
    int distanciaInicialAlDestino = calcularDistanciaManhattan(origen, destino);

    cuadriculaLineal[indiceOrigen].identificadorDeBusquedaAEstrella = numeroDeBusquedaAEstrellaActual;
    bufferCostoG[indiceOrigen]   = costoInicialEnOrigen;
    bufferCostoF[indiceOrigen]   = distanciaInicialAlDestino;
    bufferCameFrom[indiceOrigen] = indiceSinPadreEnRuta;
    colaDePrioridad.emplace(distanciaInicialAlDestino, indiceOrigen);

    int nodosExplorados                  = 0;
    bool seAlcanzoElDestino              = false;
    int indiceMasCercanoAlDestino        = indiceOrigen;
    int menorDistanciaManhattanAlDestino = distanciaInicialAlDestino;

    while (!colaDePrioridad.empty() && nodosExplorados < limiteMaximoNodosExploradosAEstrella) {
        EntradaColaPrioridad entradaActual = colaDePrioridad.top();
        colaDePrioridad.pop();

        int costoFEnCola = entradaActual.first;
        int indiceActual = entradaActual.second;

        if (costoFEnCola > bufferCostoF[indiceActual]) {
            continue;
        }

        if (indiceActual == indiceDestino) {
            seAlcanzoElDestino = true;
            break;
        }

        ++nodosExplorados;

        int columnaActual = indiceActual % anchoEnCasillas;
        int filaActual    = indiceActual / anchoEnCasillas;

        for (int indiceDireccion = 0; indiceDireccion < cantidadVecinosOrtogonales; ++indiceDireccion) {
            int columnaVecina = columnaActual + desplazamientoEnColumna[indiceDireccion];
            int filaVecina    = filaActual + desplazamientoEnFila[indiceDireccion];

            if (!esCasillaTransitableInterna(columnaVecina, filaVecina, puedeEntrarAZonasRestringidas)) {
                continue;
            }

            int indiceVecino = calcularIndiceLineal(columnaVecina, filaVecina);
            int nuevoCostoG  = bufferCostoG[indiceActual] + costoPorPasoOrtogonal;
            bool fueVisitadoEnEstaBusqueda =
                (cuadriculaLineal[indiceVecino].identificadorDeBusquedaAEstrella == numeroDeBusquedaAEstrellaActual);

            if (!fueVisitadoEnEstaBusqueda || nuevoCostoG < bufferCostoG[indiceVecino]) {
                int distanciaRestante = calcularDistanciaManhattan(
                    CoordenadaCasilla{ columnaVecina, filaVecina },
                    destino
                );
                int nuevoCostoF = nuevoCostoG + distanciaRestante;

                cuadriculaLineal[indiceVecino].identificadorDeBusquedaAEstrella = numeroDeBusquedaAEstrellaActual;
                bufferCameFrom[indiceVecino] = indiceActual;
                bufferCostoG[indiceVecino]   = nuevoCostoG;
                bufferCostoF[indiceVecino]   = nuevoCostoF;
                colaDePrioridad.emplace(nuevoCostoF, indiceVecino);

                if (distanciaRestante < menorDistanciaManhattanAlDestino) {
                    menorDistanciaManhattanAlDestino = distanciaRestante;
                    indiceMasCercanoAlDestino        = indiceVecino;
                }
            }
        }
    }

    int indiceObjetivoDeReconstruccion = seAlcanzoElDestino ? indiceDestino : indiceMasCercanoAlDestino;
    if (indiceObjetivoDeReconstruccion == indiceOrigen || bufferCameFrom[indiceObjetivoDeReconstruccion] == indiceSinPadreEnRuta) {
        return origen;
    }

    int indicePasoSiguiente = indiceObjetivoDeReconstruccion;
    while (bufferCameFrom[indicePasoSiguiente] != indiceOrigen && bufferCameFrom[indicePasoSiguiente] != indiceSinPadreEnRuta) {
        indicePasoSiguiente = bufferCameFrom[indicePasoSiguiente];
    }

    return CoordenadaCasilla{
        indicePasoSiguiente % anchoEnCasillas,
        indicePasoSiguiente / anchoEnCasillas
    };
}

void Mapa::desbloquearZonaCompleta(uint8_t numeroDeZona) {
    for (CasillaDelMapa& casilla : cuadriculaLineal) {
        if (casilla.numeroDeZona == numeroDeZona) {
            casilla.tienePasoBloqueado = banderaApagada;
        }
    }
}

void Mapa::cambiarBloqueoDeCasilla(int columna, int fila, bool bloquearPaso) {
    if (!esCasillaValida(columna, fila)) {
        return;
    }
    cuadriculaLineal[calcularIndiceLineal(columna, fila)].tienePasoBloqueado =
        bloquearPaso ? banderaEncendida : banderaApagada;
}

QString Mapa::obtenerSectorEnCasilla(CoordenadaCasilla casilla) const {
    for (const SectorDelMapa& sector : sectores) {
        bool dentroEnColumnas = casilla.columna >= sector.esquinaSuperiorIzquierda.columna
                             && casilla.columna <= sector.esquinaInferiorDerecha.columna;
        bool dentroEnFilas    = casilla.fila >= sector.esquinaSuperiorIzquierda.fila
                             && casilla.fila <= sector.esquinaInferiorDerecha.fila;
        if (dentroEnColumnas && dentroEnFilas) {
            return sector.nombre;
        }
    }
    return QString();
}

bool Mapa::cumpleAreaCompletaParaTrampa(int columnaCentral, int filaCentral) const {
    for (int desplazamientoFila = -radioMitadZonaTrampaEnCasillas;
         desplazamientoFila <= radioMitadZonaTrampaEnCasillas;
         ++desplazamientoFila) {
        for (int desplazamientoColumna = -radioMitadZonaTrampaEnCasillas;
             desplazamientoColumna <= radioMitadZonaTrampaEnCasillas;
             ++desplazamientoColumna) {
            int columnaEvaluada = columnaCentral + desplazamientoColumna;
            int filaEvaluada    = filaCentral + desplazamientoFila;
            if (!esCasillaValida(columnaEvaluada, filaEvaluada)) {
                return false;
            }
            const CasillaDelMapa& casillaEvaluada = cuadriculaLineal[calcularIndiceLineal(columnaEvaluada, filaEvaluada)];
            if (casillaEvaluada.esTransitable == banderaApagada
                || casillaEvaluada.tienePasoBloqueado == banderaEncendida
                || casillaEvaluada.esAlcanzableDesdeElInicio == banderaApagada) {
                return false;
            }
        }
    }
    return true;
}

void Mapa::marcarCasillasAlcanzablesDesde(CoordenadaCasilla casillaDeInicio) {
    casillasAptasParaTrampa.clear();
    for (CasillaDelMapa& casilla : cuadriculaLineal) {
        casilla.esAlcanzableDesdeElInicio = banderaApagada;
        casilla.esAptaParaTrampa          = banderaApagada;
    }

    if (!esCasillaTransitableInterna(casillaDeInicio.columna, casillaDeInicio.fila, true)) {
        return;
    }

    std::queue<CoordenadaCasilla> colaPorExplorar;
    int indiceInicio = calcularIndiceLineal(casillaDeInicio.columna, casillaDeInicio.fila);
    cuadriculaLineal[indiceInicio].esAlcanzableDesdeElInicio = banderaEncendida;
    colaPorExplorar.push(casillaDeInicio);

    while (!colaPorExplorar.empty()) {
        CoordenadaCasilla casillaActual = colaPorExplorar.front();
        colaPorExplorar.pop();

        for (int indiceDireccion = 0; indiceDireccion < cantidadVecinosOrtogonales; ++indiceDireccion) {
            int columnaVecina = casillaActual.columna + desplazamientoEnColumna[indiceDireccion];
            int filaVecina    = casillaActual.fila + desplazamientoEnFila[indiceDireccion];

            if (!esCasillaValida(columnaVecina, filaVecina)) {
                continue;
            }

            int indiceVecino = calcularIndiceLineal(columnaVecina, filaVecina);
            CasillaDelMapa& casillaVecina = cuadriculaLineal[indiceVecino];

            if (casillaVecina.esAlcanzableDesdeElInicio == banderaEncendida) {
                continue;
            }
            if (casillaVecina.esTransitable == banderaApagada || casillaVecina.tienePasoBloqueado == banderaEncendida) {
                continue;
            }

            casillaVecina.esAlcanzableDesdeElInicio = banderaEncendida;
            colaPorExplorar.push(CoordenadaCasilla{ columnaVecina, filaVecina });
        }
    }

    for (int fila = 0; fila < altoEnCasillas; ++fila) {
        for (int columna = 0; columna < anchoEnCasillas; ++columna) {
            int indiceEvaluado = calcularIndiceLineal(columna, fila);
            if (cuadriculaLineal[indiceEvaluado].esAlcanzableDesdeElInicio == banderaApagada) {
                continue;
            }
            if (cumpleAreaCompletaParaTrampa(columna, fila)) {
                cuadriculaLineal[indiceEvaluado].esAptaParaTrampa = banderaEncendida;
                casillasAptasParaTrampa.push_back(CoordenadaCasilla{ columna, fila });
            }
        }
    }
}

const std::vector<CoordenadaCasilla>& Mapa::obtenerListaDeArbustos() const {
    return listaDeArbustos;
}

const std::vector<CoordenadaCasilla>& Mapa::obtenerCasillasAptasParaTrampa() const {
    return casillasAptasParaTrampa;
}

const std::vector<SectorDelMapa>& Mapa::obtenerSectores() const {
    return sectores;
}

const std::vector<ReferenciaDelMapa>& Mapa::obtenerReferencias() const {
    return referencias;
}

void Mapa::agregarSector(const SectorDelMapa& sector) {
    sectores.push_back(sector);
}

void Mapa::agregarReferencia(const ReferenciaDelMapa& referencia) {
    referencias.push_back(referencia);
}

void Mapa::agregarArbusto(CoordenadaCasilla casilla) {
    listaDeArbustos.push_back(casilla);
}
