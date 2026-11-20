# ESPECIFICACION TECNICA: mapa.h / mapa.cpp

## Archivo de Cabecera: `mapa.h`

### Guardas de inclusion
```cpp
#ifndef MAPA_H
#define MAPA_H
```

### Inclusiones requeridas
```cpp
#include <vector>
#include <QString>
```

### Estructuras y Enumeraciones
```cpp
struct CoordenadaCasilla {
    int columna;
    int fila;

    bool operator==(const CoordenadaCasilla& otraCasilla) const {
        return (columna == otraCasilla.columna && fila == otraCasilla.fila);
    }

    bool operator!=(const CoordenadaCasilla& otraCasilla) const {
        return !(*this == otraCasilla);
    }
};

enum class TipoDeTerreno {
    SueloTransitable,
    CaminoDeAdoquin,
    ParedSolida,
    AguaMuralla
};

struct CasillaDelMapa {
    TipoDeTerreno terreno;
    int numeroDeZona;
    bool esTransitable;
    bool esZonaRestringida;
    bool tienePasoBloqueado;
    bool esArbustoParaOcultarse;
    bool esAptaParaTrampa;
    bool esAlcanzableDesdeElInicio;
};

struct SectorDelMapa {
    QString nombre;
    CoordenadaCasilla esquinaSuperiorIzquierda;
    CoordenadaCasilla esquinaInferiorDerecha;
};

struct ReferenciaDelMapa {
    QString nombre;
    CoordenadaCasilla casilla;
};
```

### Declaracion de Clase `Mapa`
```cpp
class Mapa {
private:
    int anchoEnCasillas;
    int altoEnCasillas;
    std::vector<CasillaDelMapa> cuadriculaLineal;
    std::vector<CoordenadaCasilla> listaDeArbustos;
    std::vector<CoordenadaCasilla> casillasAptasParaTrampa;
    std::vector<SectorDelMapa> sectores;
    std::vector<ReferenciaDelMapa> referencias;

    inline int calcularIndiceLineal(int columna, int fila) const {
        return fila * anchoEnCasillas + columna;
    }

public:
    Mapa();
    void inicializarDimensiones(int ancho, int alto);
    int obtenerAnchoEnCasillas() const;
    int obtenerAltoEnCasillas() const;

    CasillaDelMapa obtenerCasilla(int columna, int fila) const;
    CasillaDelMapa& modificarCasilla(int columna, int fila);

    bool esCasillaValida(int columna, int fila) const;
    bool esPosicionTransitable(double posicionX, double posicionY, bool puedeEntrarARestringidas) const;
    bool tieneLineaDeVista(CoordenadaCasilla origen, CoordenadaCasilla destino) const;
    CoordenadaCasilla calcularSiguientePasoHacia(CoordenadaCasilla origen, CoordenadaCasilla destino, bool puedeEntrarARestringidas) const;

    void desbloquearZonaCompleta(int numeroDeZona);
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
};

#endif
```

---

## Archivo de Implementacion: `mapa.cpp`

### Inclusiones requeridas
```cpp
#include "mapa/mapa.h"
#include "herramientas/calculos_espaciales.h"
#include <queue>
#include <cmath>
```

### Implementacion de Metodos

#### 1. Constructor
- Inicializa `anchoEnCasillas = 0; altoEnCasillas = 0;`

#### 2. `inicializarDimensiones`
- Redimensiona `cuadriculaLineal` con `ancho * alto` elementos por defecto.
- Limpia listas internas (`listaDeArbustos`, `casillasAptasParaTrampa`, `sectores`, `referencias`).

#### 3. `esCasillaValida`
- Retorna `(columna >= 0 && columna < anchoEnCasillas && fila >= 0 && fila < altoEnCasillas);`

#### 4. `obtenerCasilla` y `modificarCasilla`
- Retornan `cuadriculaLineal[calcularIndiceLineal(columna, fila)];`

#### 5. `esPosicionTransitable`
- Calcula columna y fila con `std::floor(posicionX)` y `std::floor(posicionY)`.
- Si `!esCasillaValida(col, fil)` retorna `false`.
- `const CasillaDelMapa& casilla = obtenerCasilla(col, fil);`
- Retorna `false` si: `!casilla.esTransitable || casilla.tienePasoBloqueado || (casilla.esZonaRestringida && !puedeEntrarARestringidas)`.
- En caso contrario retorna `true`.

#### 6. `tieneLineaDeVista` (Algoritmo Bresenham)
- `int x0 = origen.columna, y0 = origen.fila;`
- `int x1 = destino.columna, y1 = destino.fila;`
- `int dx = std::abs(x1 - x0);`
- `int dy = std::abs(y1 - y0);`
- `int sx = (x0 < x1) ? 1 : -1;`
- `int sy = (y0 < y1) ? 1 : -1;`
- `int err = dx - dy;`
- Bucle `while (x0 != x1 || y0 != y1)`:
  - `int e2 = 2 * err;`
  - Si `e2 > -dy`: `err -= dy; x0 += sx;`
  - Si `e2 < dx`: `err += dx; y0 += sy;`
  - Si `(x0 == x1 && y0 == y1)` sale del bucle (el destino es valido).
  - Consulta casilla `(x0, y0)`: si `!casilla.esTransitable || casilla.tienePasoBloqueado`, retorna `false`.
- Retorna `true`.

#### 7. `calcularSiguientePasoHacia` (Algoritmo A* acotado)
- Si `origen == destino` retorna `origen`.
- Utiliza cola de prioridad con costo `f = g + h`. Heuristica `h = std::abs(col - destino.columna) + std::abs(fil - destino.fila)`.
- Maximo de expansiones de nodo: 300.
- Movimiento ortogonal en 4 direcciones.
- Reconstruye el camino desde el nodo destino alcanzado (o el mas cercano si se agoto el limite) hasta el origen, y retorna la casilla inmediatamente posterior al origen.

#### 8. `desbloquearZonaCompleta`
- Recorre `cuadriculaLineal`. Si `casilla.numeroDeZona == numeroDeZona`, asigna `casilla.tienePasoBloqueado = false`.

#### 9. `cambiarBloqueoDeCasilla`
- `modificarCasilla(columna, fila).tienePasoBloqueado = bloquearPaso;`

#### 10. `obtenerSectorEnCasilla`
- Itera en `sectores`. Si la casilla cae en el rectangulo delimitado por `esquinaSuperiorIzquierda` y `esquinaInferiorDerecha`, retorna `sector.nombre`.
- Retorna `""` si no pertenece a ningun sector nombrado.

#### 11. `marcarCasillasAlcanzablesDesde`
- Ejecuta BFS desde `casillaDeInicio`. Marca `esAlcanzableDesdeElInicio = true` en cada casilla alcanzable.
- Para cada casilla `(c, f)` alcanzable, valida si el bloque de 5x5 centrado en ella (rango `[c - 2, c + 2]` y `[f - 2, f + 2]`) tiene sus 25 casillas validas, transitables, sin bloqueo y alcanzables. Si cumple, agrega `(c, f)` a `casillasAptasParaTrampa`.
