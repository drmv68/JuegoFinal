# ESPECIFICACION TECNICA: cargador_de_niveles.h / cargador_de_niveles.cpp

## Archivo de Cabecera: `cargador_de_niveles.h`

### Guardas de inclusion
```cpp
#ifndef CARGADOR_DE_NIVELES_H
#define CARGADOR_DE_NIVELES_H
```

### Inclusiones requeridas
```cpp
#include <QString>
#include <QJsonObject>
#include <vector>
#include "mapa/mapa.h"
```

### Estructuras
```cpp
struct DatosInicialesDelNivel {
    CoordenadaCasilla casillaInicialScooby;

    struct AmigoInicial {
        QString rolTexto;
        CoordenadaCasilla casilla;
    };
    std::vector<AmigoInicial> amigos;

    struct CiudadanoInicial {
        QString claveDialogo;
        CoordenadaCasilla casilla;
    };
    std::vector<CiudadanoInicial> ciudadanos;

    struct SecuazInicial {
        bool esRutaFija;
        std::vector<CoordenadaCasilla> puntosDeControl;
    };
    std::vector<SecuazInicial> secuaces;

    CoordenadaCasilla casillaCentinelaAlmacen;
    double radioCentinelaAlmacen;
    CoordenadaCasilla casillaAlmacen;
    CoordenadaCasilla casillaLampara;
    std::vector<CoordenadaCasilla> casillasCanones;
};
```

### Declaracion de Clase `CargadorDeNiveles`
```cpp
class CargadorDeNiveles {
private:
    void procesarArchivoTerreno(const QString& ruta, Mapa& mapa);
    void procesarArchivoDetalles(const QString& ruta, Mapa& mapa, DatosInicialesDelNivel& datos, int numeroDeNivel);
    void comprobarBordesCerradosYDimensiones(const Mapa& mapa, const QString& rutaArchivo);
    void comprobarQueNadieNazcaSobreObstaculos(const Mapa& mapa, const DatosInicialesDelNivel& datos, const QString& rutaArchivo);

public:
    CargadorDeNiveles() = default;
    void cargarNivelDesdeArchivos(const QString& rutaTerreno, const QString& rutaDetalles, int numeroDeNivel, Mapa& mapaDestino, DatosInicialesDelNivel& datosSalida);
    QJsonObject leerBancoDeDatos(const QString& rutaDelBanco);
};

#endif
```

---

## Archivo de Implementacion: `cargador_de_niveles.cpp`

### Inclusiones requeridas
```cpp
#include "mapa/cargador_de_niveles.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <stdexcept>
```

### Implementacion de Metodos

#### 1. `cargarNivelDesdeArchivos`
- Llama a `procesarArchivoTerreno(rutaTerreno, mapaDestino);`
- Llama a `comprobarBordesCerradosYDimensiones(mapaDestino, rutaTerreno);`
- Llama a `procesarArchivoDetalles(rutaDetalles, mapaDestino, datosSalida, numeroDeNivel);`
- Llama a `comprobarQueNadieNazcaSobreObstaculos(mapaDestino, datosSalida, rutaDetalles);`
- Llama a `mapaDestino.marcarCasillasAlcanzablesDesde(datosSalida.casillaInicialScooby);`

#### 2. `procesarArchivoTerreno`
- Abre `QFile` en modo lectura texto. Si no abre, lanza `std::runtime_error("No se pudo abrir terreno: " + ruta.toStdString());`
- Lee todas las lineas no vacias.
- `int alto = lineas.size(); int ancho = lineas[0].size();`
- `mapa.inicializarDimensiones(ancho, alto);`
- Recorre `fila` y `columna`:
  - Caracter `#`: `terreno = TipoDeTerreno::ParedSolida; esTransitable = false;`
  - Caracter `.`: `terreno = TipoDeTerreno::SueloTransitable; esTransitable = true;`
  - Caracter `+`: `terreno = TipoDeTerreno::CaminoDeAdoquin; esTransitable = true;`
  - Caracter `R`: `terreno = TipoDeTerreno::SueloTransitable; esTransitable = true; esZonaRestringida = true;`
  - Caracter `A`: `terreno = TipoDeTerreno::SueloTransitable; esTransitable = true; esArbustoParaOcultarse = true;` agrega a lista de arbustos.
  - Caracter `B`: `terreno = TipoDeTerreno::AguaMuralla; esTransitable = false;`
  - Cualquier otro caracter: lanza `std::runtime_error` especificando simbolo y linea.

#### 3. `comprobarBordesCerradosYDimensiones`
- Verifica que todas las filas tengan exactamente `mapa.obtenerAnchoEnCasillas()` caracteres.
- Verifica que el perimetro superior, inferior, izquierdo y derecho tengan `terreno == ParedSolida || terreno == AguaMuralla`.
- Si alguna casilla perimetral es transitable, lanza `std::runtime_error("Borde no cerrado en perimetro del mapa");`

#### 4. `procesarArchivoDetalles`
- Abre `QFile` en modo lectura texto. Si no abre, lanza `std::runtime_error`.
- Por cada linea no vacia y que no inicie con `#`:
  - Divide por espacios: `QStringList partes = linea.split(' ', Qt::SkipEmptyParts);`
  - Token `SCOOBY`: asigna `casillaInicialScooby = { col, fila }`.
  - Token `AMIGO`: agrega a `amigos` con `rolTexto = partes[1]` y `{ col, fila }`.
  - Token `CIUDADANO`: agrega a `ciudadanos` con `claveDialogo = partes[1]` y `{ col, fila }`.
  - Token `SECUAZ`: si `partes[1] == "RUTA_FIJA"`, lee pares de coordenadas subsiguientes; si `PATRULLA_LIBRE`, lee coordenada inicial.
  - Token `CENTINELA_ALMACEN`: lee `{ col, fila, radio }`.
  - Token `ALMACEN`: lee `{ col, fila }`.
  - Token `LAMPARA`: lee `{ col, fila }`.
  - Token `CANON`: lee `{ col, fila }`, agrega a `casillasCanones`.
  - Token `ZONA`: lee `numZona, colA, filA, colB, filB` y asigna `numeroDeZona` a las casillas del rectangulo. En Nivel 1, si la casilla no es Zona 1, asigna `tienePasoBloqueado = true`.
  - Token `SECTOR`: lee `nombre, colA, filA, colB, filB`, agrega sector al mapa.
  - Token `REFERENCIA`: lee `nombre, col, fila`, agrega referencia al mapa.

#### 5. `comprobarQueNadieNazcaSobreObstaculos`
- Verifica que `casillaInicialScooby`, cada amigo, ciudadano y secuaz inicien en casillas con `esTransitable == true`. Si alguno inicia sobre una pared, lanza `std::runtime_error`.

#### 6. `leerBancoDeDatos`
- Abre `QFile` en `rutaDelBanco`. Lanza `std::runtime_error` si no abre.
- `QJsonParseError errorJson; QJsonDocument doc = QJsonDocument::fromJson(archivo.readAll(), &errorJson);`
- Si `errorJson.error != QJsonParseError::NoError`, lanza `std::runtime_error("Error JSON en banco: " + errorJson.errorString().toStdString());`
- Retorna `doc.object();`
