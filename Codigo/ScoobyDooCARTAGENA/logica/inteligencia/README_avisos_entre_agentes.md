# ESPECIFICACION TECNICA: avisos_entre_agentes.h / avisos_entre_agentes.cpp

## Archivo y Ubicacion
`logica/inteligencia/avisos_entre_agentes.h`
`logica/inteligencia/avisos_entre_agentes.cpp`

## Proposito y Responsabilidad Unica
Gestiona el buzon compartido estatico de comunicacion entre agentes de seguridad colonial, administrando colas de alertas, resolviendo saturaciones por sobreescritura del aviso mas antiguo y canalizando alertas globales con coordenada objetivo.

## Dependencias
### Para avisos_entre_agentes.h
- `#include <array>`
- `#include "herramientas/tipos_espaciales.h"`

### Para avisos_entre_agentes.cpp
- `#include "inteligencia/avisos_entre_agentes.h"`
- `#include "herramientas/calculos_espaciales.h"`

## Definicion Exacta de Cabecera (avisos_entre_agentes.h)

```cpp
#ifndef AVISOS_ENTRE_AGENTES_H
#define AVISOS_ENTRE_AGENTES_H

#include <array>
#include "herramientas/tipos_espaciales.h"

struct AvisoDeVigilancia {
    int numeroDeSerieDelEmisor;
    CoordenadaCasilla casillaReportada;
    double segundosRestantesDeVigencia;
    bool esOrdenDirectaDelVillano;
};

class BuzonCompartidoDeAvisos final {
public:
    static constexpr int capacidadMaximaDeAvisos = 16;
    static constexpr double distanciaMaximaAtencionAvisoEnCasillas = 10.0;
    static constexpr double duracionVigenciaAvisoPorDefectoEnSegundos = 8.0;

    BuzonCompartidoDeAvisos();
    ~BuzonCompartidoDeAvisos() = default;

    BuzonCompartidoDeAvisos(const BuzonCompartidoDeAvisos&) = default;
    BuzonCompartidoDeAvisos& operator=(const BuzonCompartidoDeAvisos&) = default;

    void publicarNuevoAviso(int emisor, CoordenadaCasilla casilla, bool ordenVillano, double vigenciaSegundos = duracionVigenciaAvisoPorDefectoEnSegundos);
    void descontarTiempoDeVigencia(double segundosTranscurridos);
    bool buscarAvisoCercanoParaElAgente(CoordenadaCasilla posicionDelAgente, AvisoDeVigilancia& avisoEncontrado) const;
    void activarAlertaGlobal(double duracionEnSegundos, CoordenadaCasilla casillaObjetivoAlerta);

    int obtenerCantidadDeAvisosActivos() const;
    bool estaAlertaGlobalActiva() const;
    CoordenadaCasilla obtenerCasillaObjetivoDeAlertaGlobal() const;

private:
    std::array<AvisoDeVigilancia, capacidadMaximaDeAvisos> listaDeAvisosActivos;
    int cantidadDeAvisosActivos;
    double segundosDeAlertaGlobalRestantes;
    CoordenadaCasilla casillaObjetivoAlertaGlobal;
};

#endif
```

## Algoritmo Paso a Paso (avisos_entre_agentes.cpp)

### Constructor
1. Asignar `cantidadDeAvisosActivos = 0; segundosDeAlertaGlobalRestantes = 0.0; casillaObjetivoAlertaGlobal = CoordenadaCasilla{0, 0};`.

### publicarNuevoAviso(int emisor, CoordenadaCasilla casilla, bool ordenVillano, double vigenciaSegundos)
1. Buscar si ya existe un aviso previo publicado por el mismo emisor:
   - Iterar `indice` desde 0 hasta `cantidadDeAvisosActivos - 1`:
     - Si `listaDeAvisosActivos[indice].numeroDeSerieDelEmisor == emisor`:
       - Actualizar in-situ: `listaDeAvisosActivos[indice].casillaReportada = casilla; listaDeAvisosActivos[indice].segundosRestantesDeVigencia = vigenciaSegundos; listaDeAvisosActivos[indice].esOrdenDirectaDelVillano = ordenVillano;`
       - Terminar ejecucion.
2. Si es un emisor nuevo y `cantidadDeAvisosActivos < capacidadMaximaDeAvisos`:
   - `listaDeAvisosActivos[cantidadDeAvisosActivos] = AvisoDeVigilancia{ emisor, casilla, vigenciaSegundos, ordenVillano };`
   - `cantidadDeAvisosActivos++;`
   - Terminar ejecucion.
3. Si el arreglo de 16 esta lleno:
   - Encontrar el indice del aviso con menor `segundosRestantesDeVigencia` que no sea orden directa del villano (o el mas antiguo en general).
   - Sobreescribir dicho espacio con el nuevo aviso.

### descontarTiempoDeVigencia(double segundosTranscurridos)
1. Si `segundosDeAlertaGlobalRestantes > 0.0`:
   - `segundosDeAlertaGlobalRestantes -= segundosTranscurridos;`
   - Si `segundosDeAlertaGlobalRestantes < 0.0`, asignar `segundosDeAlertaGlobalRestantes = 0.0;`.
2. Iterar `indice = 0` mientras `indice < cantidadDeAvisosActivos`:
   - `listaDeAvisosActivos[indice].segundosRestantesDeVigencia -= segundosTranscurridos;`
   - Si `listaDeAvisosActivos[indice].segundosRestantesDeVigencia <= 0.0`:
     - Reemplazar por el ultimo: `listaDeAvisosActivos[indice] = listaDeAvisosActivos[cantidadDeAvisosActivos - 1];`
     - Decrementar `cantidadDeAvisosActivos--;`
   - Si no expiro: `indice++;`

### buscarAvisoCercanoParaElAgente(CoordenadaCasilla posicionDelAgente, AvisoDeVigilancia& avisoEncontrado) const
1. Si `segundosDeAlertaGlobalRestantes > 0.0`:
   - `avisoEncontrado = AvisoDeVigilancia{ -1, casillaObjetivoAlertaGlobal, segundosDeAlertaGlobalRestantes, true };`
   - Retornar `true;`
2. Para `indice` desde 0 hasta `cantidadDeAvisosActivos - 1`:
   - `const AvisoDeVigilancia& aviso = listaDeAvisosActivos[indice];`
   - Si `aviso.esOrdenDirectaDelVillano`:
     - `avisoEncontrado = aviso;`
     - Retornar `true;`
   - Calcular `distanciaAlCuadrado = CalculosEspaciales::calcularDistanciaAlCuadrado(posicionDelAgente.columna, posicionDelAgente.fila, aviso.casillaReportada.columna, aviso.casillaReportada.fila)`.
   - Si `distanciaAlCuadrado <= (distanciaMaximaAtencionAvisoEnCasillas * distanciaMaximaAtencionAvisoEnCasillas)`:
     - `avisoEncontrado = aviso;`
     - Retornar `true;`
3. Retornar `false`.

### activarAlertaGlobal(double duracionEnSegundos, CoordenadaCasilla casillaObjetivoAlerta)
1. `segundosDeAlertaGlobalRestantes = duracionEnSegundos;`
2. `casillaObjetivoAlertaGlobal = casillaObjetivoAlerta;`
