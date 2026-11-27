#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include <QString>
#include <cstdint>
#include <cassert>
#include "herramientas/tipos_espaciales.h"

enum class TipoDeTerreno : uint8_t {
    SueloTransitable,
    CaminoDeAdoquin,
    ParedSolida,
    AguaMuralla
};

#pragma pack(push, 1)
struct CasillaDelMapa {
    TipoDeTerreno terreno : 4;
    uint8_t numeroDeZona : 4;
    uint8_t esTransitable : 1;
    uint8_t esZonaRestringida : 1;
    uint8_t tienePasoBloqueado : 1;
    uint8_t esArbustoParaOcultarse : 1;
    uint8_t esAptaParaTrampa : 1;
    uint8_t esAlcanzableDesdeElInicio : 1;
    uint8_t reservadoParaAlineacion : 2;
    uint16_t identificadorDeBusquedaAEstrella;
};
#pragma pack(pop)

static_assert(sizeof(CasillaDelMapa) == 4, "La estructura CasillaDelMapa debe ocupar exactamente cuatro bytes.");

struct SectorDelMapa {
    QString nombre;
    CoordenadaCasilla esquinaSuperiorIzquierda;
    CoordenadaCasilla esquinaInferiorDerecha;
};

struct ReferenciaDelMapa {
    QString nombre;
    CoordenadaCasilla casilla;
};

class Mapa final {
public:
    static constexpr int limiteMaximoNodosExploradosAEstrella = 300;
    static constexpr int tamanoLadoZonaTrampaEnCasillas       = 5;
    static constexpr int radioMitadZonaTrampaEnCasillas      = 2;
    static constexpr uint8_t banderaEncendida                = 1u;
    static constexpr uint8_t banderaApagada                  = 0u;
    static constexpr uint16_t identificadorBusquedaInicial   = 1u;
    static constexpr uint16_t identificadorBusquedaSinMarca  = 0u;
    static constexpr int indiceSinPadreEnRuta                = -1;
    static constexpr int costoPorPasoOrtogonal               = 1;

    Mapa();
    ~Mapa() = default;

    Mapa(const Mapa&) = delete;
    Mapa& operator=(const Mapa&) = delete;

    void inicializarDimensiones(int ancho, int alto);
    int  obtenerAnchoEnCasillas() const;
    int  obtenerAltoEnCasillas() const;

    CasillaDelMapa  obtenerCasilla(int columna, int fila) const;
    CasillaDelMapa& modificarCasilla(int columna, int fila);

    bool esCasillaValida(int columna, int fila) const;
    bool esPosicionTransitable(double posicionX, double posicionY, bool puedeEntrarAZonasRestringidas) const;
    bool tieneLineaDeVista(CoordenadaCasilla origen, CoordenadaCasilla destino) const;
    CoordenadaCasilla calcularSiguientePasoHacia(CoordenadaCasilla origen,
                                                 CoordenadaCasilla destino,
                                                 bool puedeEntrarAZonasRestringidas);

    void    desbloquearZonaCompleta(uint8_t numeroDeZona);
    void    cambiarBloqueoDeCasilla(int columna, int fila, bool bloquearPaso);
    QString obtenerSectorEnCasilla(CoordenadaCasilla casilla) const;
    void    marcarCasillasAlcanzablesDesde(CoordenadaCasilla casillaDeInicio);

    const std::vector<CoordenadaCasilla>& obtenerListaDeArbustos() const;
    const std::vector<CoordenadaCasilla>& obtenerCasillasAptasParaTrampa() const;
    const std::vector<SectorDelMapa>&     obtenerSectores() const;
    const std::vector<ReferenciaDelMapa>& obtenerReferencias() const;

    void agregarSector(const SectorDelMapa& sector);
    void agregarReferencia(const ReferenciaDelMapa& referencia);
    void agregarArbusto(CoordenadaCasilla casilla);

private:
    int anchoEnCasillas;
    int altoEnCasillas;
    std::vector<CasillaDelMapa>    cuadriculaLineal;
    std::vector<CoordenadaCasilla> listaDeArbustos;
    std::vector<CoordenadaCasilla> casillasAptasParaTrampa;
    std::vector<SectorDelMapa>     sectores;
    std::vector<ReferenciaDelMapa> referencias;

    uint16_t numeroDeBusquedaAEstrellaActual;
    std::vector<int> bufferCostoG;
    std::vector<int> bufferCostoF;
    std::vector<int> bufferCameFrom;

    inline int calcularIndiceLineal(int columna, int fila) const {
        return (fila * anchoEnCasillas) + columna;
    }

    bool esCasillaTransitableInterna(int columna, int fila, bool puedeEntrarAZonasRestringidas) const;
    bool cumpleAreaCompletaParaTrampa(int columnaCentral, int filaCentral) const;
};

#endif
