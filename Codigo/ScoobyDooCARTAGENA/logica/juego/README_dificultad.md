# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: DIFICULTAD DEL JUEGO

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
El namespace `ConfiguracionDeDificultad` provee funciones puras y constantes inmutables `constexpr` que parametrizan la experiencia de juego según el nivel seleccionado (Fácil, Normal, Difícil, Experto). No mantiene estado mutable ni requiere instanciación, facilitando su consulta directa por `MundoDeCartagena`, `Scooby`, la IA y el ciclo día/noche.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include <cstdint>
```

## 3. ENUMERACIONES Y ESTRUCTURAS
```cpp
enum class NivelDeDificultad : uint8_t {
    Facil = 0,
    Normal = 1,
    Dificil = 2,
    Experto = 3
};

struct ParametrosDeDificultad {
    float multiplicadorVelocidadEnemigos;
    float tiempoMaximoDeLuzDiurnaSegundos;
    float tiempoMaximoDeOscuridadNocturnaSegundos;
    float tasaConsumoDeEstamina;
    float tasaRecuperacionDeEstamina;
    int cantidadPistasRequeridasParaAcusar;
    int vidasInicialesDeScooby;
    bool habilitarNieblaDensa;
};
```

## 4. DECLARACIÓN DE FUNCIONES PURAS
```cpp
namespace ConfiguracionDeDificultad {
    ParametrosDeDificultad obtenerParametros(NivelDeDificultad nivel);
    float obtenerMultiplicadorVelocidadEnemigos(NivelDeDificultad nivel);
    int obtenerVidasIniciales(NivelDeDificultad nivel);
    int obtenerPistasRequeridas(NivelDeDificultad nivel);
}
```

## 5. TABLA DE PARÁMETROS POR NIVEL DE DIFICULTAD
1. `Facil`:
   - `multiplicadorVelocidadEnemigos`: `0.80f`
   - `tiempoMaximoDeLuzDiurnaSegundos`: `90.0f`
   - `tiempoMaximoDeOscuridadNocturnaSegundos`: `45.0f`
   - `tasaConsumoDeEstamina`: `12.0f`
   - `tasaRecuperacionDeEstamina`: `25.0f`
   - `cantidadPistasRequeridasParaAcusar`: `3`
   - `vidasInicialesDeScooby`: `5`
   - `habilitarNieblaDensa`: `false`
2. `Normal`:
   - `multiplicadorVelocidadEnemigos`: `1.00f`
   - `tiempoMaximoDeLuzDiurnaSegundos`: `75.0f`
   - `tiempoMaximoDeOscuridadNocturnaSegundos`: `60.0f`
   - `tasaConsumoDeEstamina`: `16.0f`
   - `tasaRecuperacionDeEstamina`: `20.0f`
   - `cantidadPistasRequeridasParaAcusar`: `4`
   - `vidasInicialesDeScooby`: `3`
   - `habilitarNieblaDensa`: `false`
3. `Dificil`:
   - `multiplicadorVelocidadEnemigos`: `1.15f`
   - `tiempoMaximoDeLuzDiurnaSegundos`: `60.0f`
   - `tiempoMaximoDeOscuridadNocturnaSegundos`: `75.0f`
   - `tasaConsumoDeEstamina`: `22.0f`
   - `tasaRecuperacionDeEstamina`: `15.0f`
   - `cantidadPistasRequeridasParaAcusar`: `5`
   - `vidasInicialesDeScooby`: `2`
   - `habilitarNieblaDensa`: `true`
4. `Experto`:
   - `multiplicadorVelocidadEnemigos`: `1.30f`
   - `tiempoMaximoDeLuzDiurnaSegundos`: `45.0f`
   - `tiempoMaximoDeOscuridadNocturnaSegundos`: `90.0f`
   - `tasaConsumoDeEstamina`: `28.0f`
   - `tasaRecuperacionDeEstamina`: `10.0f`
   - `cantidadPistasRequeridasParaAcusar`: `5`
   - `vidasInicialesDeScooby`: `1`
   - `habilitarNieblaDensa`: `true`

## 6. LÓGICA DE IMPLEMENTACIÓN
Implementa un bloque `switch(nivel)` exhaustivo que retorna directamente una estructura `ParametrosDeDificultad` configurada con valores `constexpr`. No realiza alocaciones dinámicas.
