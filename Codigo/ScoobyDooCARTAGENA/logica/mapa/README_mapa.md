# ESPECIFICACION TECNICA: mapa.h / mapa.cpp

## Archivo y Ubicacion
`logica/mapa/mapa.h`
`logica/mapa/mapa.cpp`

## Proposito y Responsabilidad Unica
Almacena la matriz lineal del escenario colonial, gestiona la estructura de datos compacta en memoria cache (4 bytes por celda), traza lineas de vista con Bresenham y ejecuta A* acotado con buffer persistente sin reservar memoria dinamica en el bucle.

## Dependencias
### Para mapa.h
- `#include <vector>`
- `#include <QString>`
- `#include <cstdint>`
- `#include <cassert>`
- `#include "herramientas/tipos_espaciales.h"`

### Para mapa.cpp
- `#include "mapa/mapa.h"`
- `#include "herramientas/calculos_espaciales.h"`
- `#include <queue>`
- `#include <cmath>`

## Definicion Exacta de Cabecera (mapa.h)

```cpp
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
    static constexpr int tamanoLadoZonaTrampaEnCasillas = 5;
    static constexpr int radioMitadZonaTrampaEnCasillas = 2;

    Mapa();
    ~Mapa() = default;

    Mapa(const Mapa&) = delete;
    Mapa& operator=(const Mapa&) = delete;

    void inicializarDimensiones(int ancho, int alto);
    int obtenerAnchoEnCasillas() const;
    int obtenerAltoEnCasillas() const;

    CasillaDelMapa obtenerCasilla(int columna, int fila) const;
    CasillaDelMapa& modificarCasilla(int columna, int fila);

    bool esCasillaValida(int columna, int fila) const;
    bool esPosicionTransitable(double posicionX, double posicionY, bool puedeEntrarAZonasRestringidas) const;
    bool tieneLineaDeVista(CoordenadaCasilla origen, CoordenadaCasilla destino) const;
    CoordenadaCasilla calcularSiguientePasoHacia(CoordenadaCasilla origen, CoordenadaCasilla destino, bool puedeEntrarAZonasRestringidas);

    void desbloquearZonaCompleta(uint8_t numeroDeZona);
    void cambiarBloqueoDeCasilla(int columna, int fila, bool bloquearPaso);
    QString obtenerSectorEnCasilla(CoordenadaCasilla casilla) const;
    void marcarCasillasAlcanzablesDesde(CoordenadaCasilla casillaDeInicio);

    const std::vector<CoordenadaCasilla>& obtenerListaDeArbustos() const;
    const std::vector<CoordenadaCasilla>& obtenerCasillasAptasParaTrampa() const;
    const std::vector<SectorDelMapa>& obtenerSectores() const;
    const std::vector<ReferenciaDelMapa>& obtenerReferencias() const;

    void agregarSector(const SectorDelMapa& sector);
    void agregarReferencia(const ReferenciaDelMapa& referencia);
    void agregarArbusto(CoordenadaCasilla casilla);

private:
    int anchoEnCasillas;
    int altoEnCasillas;
    std::vector<CasillaDelMapa> cuadriculaLineal;
    std::vector<CoordenadaCasilla> listaDeArbustos;
    std::vector<CoordenadaCasilla> casillasAptasParaTrampa;
    std::vector<SectorDelMapa> sectores;
    std::vector<ReferenciaDelMapa> referencias;

    uint16_t numeroDeBusquedaAEstrellaActual;
    std::vector<int> bufferCostoG;
    std::vector<int> bufferCostoF;
    std::vector<int> bufferCameFrom;

    inline int calcularIndiceLineal(int columna, int fila) const {
        return (fila * anchoEnCasillas) + columna;
    }
};

#endif
```

## Algoritmo Paso a Paso (mapa.cpp)

### Constructor
1. Asignar `anchoEnCasillas = 0; altoEnCasillas = 0; numeroDeBusquedaAEstrellaActual = 1u;`.

### inicializarDimensiones(int ancho, int alto)
1. `anchoEnCasillas = ancho; altoEnCasillas = alto;`.
2. Calcular total de casillas: `totalCasillas = ancho * alto;`.
3. Asignar `cuadriculaLineal.assign(totalCasillas, CasillaDelMapa{});`.
4. Asignar `bufferCostoG.assign(totalCasillas, 0);`.
5. Asignar `bufferCostoF.assign(totalCasillas, 0);`.
6. Asignar `bufferCameFrom.assign(totalCasillas, -1);`.
7. Limpiar vectores `listaDeArbustos`, `casillasAptasParaTrampa`, `sectores`, `referencias`.

### esPosicionTransitable(double posicionX, double posicionY, bool puedeEntrarAZonasRestringidas) const
1. Convertir a casilla mediante `CoordenadaCasilla celda = CalculosEspaciales::convertirPosicionACasilla(posicionX, posicionY);`.
2. Si `!esCasillaValida(celda.columna, celda.fila)`, retornar `false`.
3. Consultar la casilla: `const CasillaDelMapa& casilla = obtenerCasilla(celda.columna, celda.fila);`.
4. Si `casilla.esTransitable == 0u || casilla.tienePasoBloqueado == 1u`, retornar `false`.
5. Si `casilla.esZonaRestringida == 1u && !puedeEntrarAZonasRestringidas`, retornar `false`.
6. Retornar `true`.

### tieneLineaDeVista(CoordenadaCasilla origen, CoordenadaCasilla destino) const
1. Si `origen == destino`, retornar `true`.
2. Aplicar algoritmo de Bresenham integro en casillas enteras:
   - `deltaX = std::abs(destino.columna - origen.columna);`
   - `deltaY = std::abs(destino.fila - origen.fila);`
   - `pasoX = (origen.columna < destino.columna) ? 1 : -1;`
   - `pasoY = (origen.fila < destino.fila) ? 1 : -1;`
   - `errorActual = deltaX - deltaY;`
   - `columnaActual = origen.columna; filaActual = origen.fila;`
3. En bucle continuo:
   - Si `columnaActual == destino.columna && filaActual == destino.fila`, terminar bucle.
   - Si no es la casilla de origen:
     - Consultar casilla `(columnaActual, filaActual)`. Si `casilla.esTransitable == 0u || casilla.tienePasoBloqueado == 1u`, retornar `false`.
   - `dobleError = 2 * errorActual;`
   - Si `dobleError > -deltaY`: `errorActual -= deltaY; columnaActual += pasoX;`
   - Si `dobleError < deltaX`: `errorActual += deltaX; filaActual += pasoY;`
4. Retornar `true`.

### calcularSiguientePasoHacia(CoordenadaCasilla origen, CoordenadaCasilla destino, bool puedeEntrarAZonasRestringidas)
1. Si `origen == destino`, retornar `origen`.
2. Si `!esCasillaValida(destino.columna, destino.fila)`, retornar `origen`.
3. Incrementar `numeroDeBusquedaAEstrellaActual++`. Si el contador se desborda a cero, asignar `numeroDeBusquedaAEstrellaActual = 1u;` y limpiar los identificadores de busqueda en `cuadriculaLineal`.
4. Inicializar cola de prioridad `std::priority_queue` con pares `(costoF, indiceLineal)`.
5. Insertar `indiceOrigen = calcularIndiceLineal(origen.columna, origen.fila)` con `costoG = 0` y `costoF = distanciaManhattan(origen, destino)`.
6. Asignar `cuadriculaLineal[indiceOrigen].identificadorDeBusquedaAEstrella = numeroDeBusquedaAEstrellaActual`.
7. Asignar `bufferCameFrom[indiceOrigen] = -1`.
8. Bucle mientras la cola no este vacia y `nodosExplorados < limiteMaximoNodosExploradosAEstrella`:
   - Extraer elemento con menor `costoF`.
   - Si el nodo extraido coincide con el destino, detener exploracion.
   - Para cada uno de los 4 vecinos ortogonales (Norte, Sur, Este, Oeste):
     - Si no es valido, continuar.
     - Si `!esPosicionTransitable(...)`, continuar.
     - Calcular nuevo `costoG = bufferCostoG[nodoActual] + 1`.
     - Si el vecino no habia sido visitado en esta busqueda o `nuevoCostoG < bufferCostoG[indiceVecino]`:
       - `cuadriculaLineal[indiceVecino].identificadorDeBusquedaAEstrella = numeroDeBusquedaAEstrellaActual`.
       - `bufferCameFrom[indiceVecino] = nodoActual`.
       - `bufferCostoG[indiceVecino] = nuevoCostoG`.
       - `bufferCostoF[indiceVecino] = nuevoCostoG + distanciaManhattan(vecino, destino)`.
       - Insertar en cola de prioridad.
9. Si el destino no fue alcanzado y se agoto el limite, tomar el nodo alcanzado con menor distancia al objetivo.
10. Reconstruir la ruta hacia atras usando `bufferCameFrom`.
11. Retornar la coordenada del nodo inmediatamente posterior a `origen`.

### marcarCasillasAlcanzablesDesde(CoordenadaCasilla casillaDeInicio)
1. Ejecutar recorrido BFS con cola `std::queue<CoordenadaCasilla>`.
2. Marcar `esAlcanzableDesdeElInicio = 1u` en cada celda transitable alcanzable.
3. Para cada celda alcanzable `(c, f)`:
   - Verificar cuadrado de 5x5 centrado en `(c, f)`: todas las 25 celdas entre `c - 2, f - 2` y `c + 2, f + 2` deben ser celdas validas, con `esTransitable == 1u`, `tienePasoBloqueado == 0u` y `esAlcanzableDesdeElInicio == 1u`.
   - Si cumple las 25 condiciones: asignar `cuadriculaLineal[calcularIndiceLineal(c, f)].esAptaParaTrampa = 1u;` y registrar en `casillasAptasParaTrampa`.
