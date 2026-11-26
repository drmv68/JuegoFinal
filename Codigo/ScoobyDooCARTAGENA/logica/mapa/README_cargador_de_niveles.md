# ESPECIFICACION TECNICA: cargador_de_niveles.h / cargador_de_niveles.cpp

## Archivo y Ubicacion
`logica/mapa/cargador_de_niveles.h`
`logica/mapa/cargador_de_niveles.cpp`

## Proposito y Responsabilidad Unica
Namespace de fabricacion y parseo que lee archivos de texto incrustados en memoria y bancos JSON para poblar el escenario de Cartagena sin poseer estado interno.

## Dependencias
### Para cargador_de_niveles.h
- `#include <QString>`
- `#include <QJsonObject>`
- `#include <vector>`
- `#include "mapa/mapa.h"`

### Para cargador_de_niveles.cpp
- `#include "mapa/cargador_de_niveles.h"`
- `#include <QFile>`
- `#include <QTextStream>`
- `#include <QJsonDocument>`
- `#include <stdexcept>`

## Definicion Exacta de Cabecera (cargador_de_niveles.h)

```cpp
#ifndef CARGADOR_DE_NIVELES_H
#define CARGADOR_DE_NIVELES_H

#include <QString>
#include <QJsonObject>
#include <vector>
#include "mapa/mapa.h"

struct AmigoInicial {
    QString rolTexto;
    CoordenadaCasilla casilla;
};

struct CiudadanoInicial {
    QString claveDialogo;
    CoordenadaCasilla casilla;
};

struct SecuazInicial {
    bool esRutaFija;
    std::vector<CoordenadaCasilla> puntosDeControl;
};

struct DatosInicialesDelNivel {
    CoordenadaCasilla casillaInicialScooby;
    std::vector<AmigoInicial> amigos;
    std::vector<CiudadanoInicial> ciudadanos;
    std::vector<SecuazInicial> secuaces;
    CoordenadaCasilla casillaCentinelaAlmacen;
    double radioCentinelaAlmacen;
    CoordenadaCasilla casillaAlmacen;
    CoordenadaCasilla casillaLampara;
    std::vector<CoordenadaCasilla> casillasCanones;
};

namespace CargadorDeNiveles {
    void cargarNivelDesdeArchivos(const QString& rutaTerreno, const QString& rutaDetalles, int numeroDeNivel, Mapa& mapaDestino, DatosInicialesDelNivel& datosSalida);
    QJsonObject leerBancoDeDatos(const QString& rutaDelBanco);
}

#endif
```

## Algoritmo Paso a Paso (cargador_de_niveles.cpp)

### cargarNivelDesdeArchivos(const QString& rutaTerreno, const QString& rutaDetalles, int numeroDeNivel, Mapa& mapaDestino, DatosInicialesDelNivel& datosSalida)
1. Abrir `QFile` en `rutaTerreno`. Si falla la apertura, lanzar `std::runtime_error("No se pudo abrir el archivo de terreno en la ruta especificada.")`.
2. Leer lineas con `QTextStream`. Determinar `alto = lineas.size()` y `ancho = lineas[0].size()`.
3. Invocar `mapaDestino.inicializarDimensiones(ancho, alto)`.
4. Procesar matriz de caracteres:
   - `#`: `terreno = TipoDeTerreno::ParedSolida; esTransitable = 0; tienePasoBloqueado = 0;`
   - `.`: `terreno = TipoDeTerreno::SueloTransitable; esTransitable = 1; tienePasoBloqueado = 0;`
   - `+`: `terreno = TipoDeTerreno::CaminoDeAdoquin; esTransitable = 1; tienePasoBloqueado = 0;`
   - `R`: `terreno = TipoDeTerreno::SueloTransitable; esTransitable = 1; esZonaRestringida = 1; tienePasoBloqueado = 0;`
   - `A`: `terreno = TipoDeTerreno::SueloTransitable; esTransitable = 1; esArbustoParaOcultarse = 1; tienePasoBloqueado = 0;` registrar en `mapaDestino.agregarArbusto({columna, fila})`.
   - `B`: `terreno = TipoDeTerreno::AguaMuralla; esTransitable = 0; tienePasoBloqueado = 0;`
   - Cualquier otro caracter: lanzar `std::runtime_error` reportando caracter desconocido y linea.
5. Validar integridad de bordes perimetrales: las cuatro fronteras exteriores deben ser `#` o `B`.
6. Abrir `QFile` en `rutaDetalles`. Si falla, lanzar `std::runtime_error("No se pudo abrir el archivo de detalles del nivel.")`.
7. Procesar comandos por linea mediante `QStringList partes = linea.split(' ', Qt::SkipEmptyParts);`:
   - `SCOOBY`: parsear columna y fila, asignar a `datosSalida.casillaInicialScooby`.
   - `AMIGO`: parsear rol y casilla, agregar a `datosSalida.amigos`.
   - `CIUDADANO`: parsear clave de dialogo y casilla, agregar a `datosSalida.ciudadanos`.
   - `SECUAZ RUTA_FIJA`: parsear pares subsiguientes y agregar a `datosSalida.secuaces`.
   - `SECUAZ PATRULLA_LIBRE`: parsear casilla y agregar con `esRutaFija = false`.
   - `CENTINELA_ALMACEN`: parsear columna, fila y radio.
   - `ALMACEN`: parsear casilla del almacen.
   - `LAMPARA`: parsear casilla de la lampara.
   - `CANON`: parsear casilla y agregar a `datosSalida.casillasCanones`.
   - `ZONA`: parsear rectangulo y asignar `numeroDeZona`. Si `numeroDeNivel == 1 && numeroDeZona != 1u`, asignar `tienePasoBloqueado = 1u`.
   - `SECTOR`: parsear nombre y rectangulo, agregar a `mapaDestino.agregarSector(...)`.
   - `REFERENCIA`: parsear nombre y casilla, agregar a `mapaDestino.agregarReferencia(...)`.
8. Comprobar que ninguna entidad nazca sobre pared: si la casilla inicial de Scooby o de cualquier personaje tiene `esTransitable == 0u`, lanzar `std::runtime_error("Entidad generada sobre obstaculo solido.")`.
9. Invocar `mapaDestino.marcarCasillasAlcanzablesDesde(datosSalida.casillaInicialScooby)`.

### leerBancoDeDatos(const QString& rutaDelBanco)
1. Abrir `QFile` en `rutaDelBanco`. Si falla, lanzar `std::runtime_error("Fallo al abrir archivo de banco JSON.")`.
2. Leer contenido y parsear con `QJsonDocument::fromJson(archivo.readAll(), &errorParseo)`.
3. Si `errorParseo.error != QJsonParseError::NoError`, lanzar `std::runtime_error("Sintaxis JSON corrupta en banco de datos.")`.
4. Retornar `documento.object()`.
