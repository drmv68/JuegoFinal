# ESPECIFICACION TECNICA: azar_con_semilla.h / azar_con_semilla.cpp

## Archivo y Ubicacion
`logica/herramientas/azar_con_semilla.h`
`logica/herramientas/azar_con_semilla.cpp`

## Proposito y Responsabilidad Unica
Generador determinista de numeros pseudoaleatorios basado en el algoritmo xorshift32 propio. Garantiza secuencias identicas en cualquier sistema operativo y compilador ante una misma semilla.

## Dependencias
### Para azar_con_semilla.h
- `#include <cstdint>`
- `#include <vector>`
- `#include <cassert>`
- `#include "herramientas/tipos_espaciales.h"`

### Para azar_con_semilla.cpp
- `#include "herramientas/azar_con_semilla.h"`
- `#include <chrono>`

## Definicion Exacta de Cabecera (azar_con_semilla.h)

```cpp
#ifndef AZAR_CON_SEMILLA_H
#define AZAR_CON_SEMILLA_H

#include <cstdint>
#include <vector>
#include <cassert>
#include "herramientas/tipos_espaciales.h"

class AzarConSemilla final {
public:
    static constexpr uint32_t semillaPorDefectoSiEsCero = 2463534242u;
    static constexpr double divisorMaximoEnteroSinSigno = 4294967295.0;
    static constexpr double porcentajeMinimoValido = 0.0;
    static constexpr double porcentajeMaximoValido = 100.0;

    AzarConSemilla();
    explicit AzarConSemilla(uint32_t semillaInicial);
    ~AzarConSemilla() = default;

    AzarConSemilla(const AzarConSemilla&) = default;
    AzarConSemilla& operator=(const AzarConSemilla&) = default;

    void inicializarConSemillaDelSistema();
    void inicializarConSemillaFija(uint32_t semillaManual);
    uint32_t obtenerNumeroDeSemillaActiva() const;

    int generarEnteroEnRango(int minimoIncluido, int maximoIncluido);
    double generarDecimalEnRango(double minimoIncluido, double maximoIncluido);
    bool evaluarProbabilidadPorcentual(double porcentajeDeExito);
    void barajarListaDeCasillas(std::vector<CoordenadaCasilla>& listaDeCasillas);

private:
    uint32_t estadoInternoXorshift;
    uint32_t numeroDeSemillaActiva;

    uint32_t avanzarPasoXorshift();
};

#endif
```

## Algoritmo Paso a Paso (azar_con_semilla.cpp)

### Constructor por defecto y sobrecargado
1. En `AzarConSemilla::AzarConSemilla()` invocar `inicializarConSemillaDelSistema()`.
2. En `AzarConSemilla::AzarConSemilla(uint32_t semillaInicial)` invocar `inicializarConSemillaFija(semillaInicial)`.

### avanzarPasoXorshift()
1. Asignar `registroTemporal = estadoInternoXorshift`.
2. Aplicar desplazamiento y xor: `registroTemporal ^= (registroTemporal << 13)`.
3. Aplicar desplazamiento y xor: `registroTemporal ^= (registroTemporal >> 17)`.
4. Aplicar desplazamiento y xor: `registroTemporal ^= (registroTemporal << 5)`.
5. Asignar `estadoInternoXorshift = registroTemporal`.
6. Retornar `estadoInternoXorshift`.

### inicializarConSemillaDelSistema()
1. Obtener la marca temporal de alta resolucion en nanosegundos mediante `std::chrono::high_resolution_clock::now().time_since_epoch().count()`.
2. Mezclar el tiempo con constantes primas: `semillaGenerada = static_cast<uint32_t>(tiempoNanosegundos ^ (tiempoNanosegundos >> 32))`.
3. Invocar `inicializarConSemillaFija(semillaGenerada)`.

### inicializarConSemillaFija(uint32_t semillaManual)
1. Si `semillaManual == 0u`, asignar `semillaManual = semillaPorDefectoSiEsCero`.
2. Asignar `numeroDeSemillaActiva = semillaManual`.
3. Asignar `estadoInternoXorshift = semillaManual`.

### obtenerNumeroDeSemillaActiva() const
1. Retornar `numeroDeSemillaActiva`.

### generarEnteroEnRango(int minimoIncluido, int maximoIncluido)
1. `assert(minimoIncluido <= maximoIncluido);`
2. Si `minimoIncluido == maximoIncluido`, retornar `minimoIncluido`.
3. Calcular la cantidad de opciones: `amplitudRango = static_cast<uint32_t>((maximoIncluido - minimoIncluido) + 1)`.
4. Obtener `valorCrudo = avanzarPasoXorshift()`.
5. Retornar `minimoIncluido + static_cast<int>(valorCrudo % amplitudRango)`.

### generarDecimalEnRango(double minimoIncluido, double maximoIncluido)
1. `assert(minimoIncluido <= maximoIncluido);`
2. Obtener `valorNormalizado = static_cast<double>(avanzarPasoXorshift()) / divisorMaximoEnteroSinSigno`.
3. Retornar `minimoIncluido + (valorNormalizado * (maximoIncluido - minimoIncluido))`.

### evaluarProbabilidadPorcentual(double porcentajeDeExito)
1. Si `porcentajeDeExito <= porcentajeMinimoValido`, retornar `false`.
2. Si `porcentajeDeExito >= porcentajeMaximoValido`, retornar `true`.
3. Retornar `generarDecimalEnRango(0.0, 100.0) <= porcentajeDeExito`.

### barajarListaDeCasillas(std::vector<CoordenadaCasilla>& listaDeCasillas)
1. Obtener `tamanoLista = listaDeCasillas.size()`.
2. Si `tamanoLista <= 1`, retornar sin operar.
3. Ejecutar algoritmo Fisher-Yates: iterar `indice = static_cast<int>(tamanoLista) - 1` de forma descendente hasta 1:
   - Obtener `indiceAleatorio = generarEnteroEnRango(0, indice)`.
   - Intercambiar `std::swap(listaDeCasillas[indice], listaDeCasillas[indiceAleatorio])`.
