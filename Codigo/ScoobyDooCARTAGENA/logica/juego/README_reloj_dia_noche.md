# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: RELOJ DÍA Y NOCHE

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `RelojDiaNoche` gestiona la progresión del ciclo circadiano del juego en Cartagena de Indias. Calcula la transición suave de iluminación ambiental (de mediodía colonial a medianoche en las murallas) mediante interpolación lineal sin saltos bruscos. Notifica cambios de fase e influye directamente en el comportamiento de los personajes (comportamiento pacífico de día, patrullas y acecho de monstruos de noche).

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include <cstdint>
```

## 3. ENUMERACIONES Y TIPOS
```cpp
enum class FaseDelDia : uint8_t {
    Dia = 0,
    Atardecer = 1,
    Noche = 2,
    Amanecer = 3
};
```

## 4. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class RelojDiaNoche {
public:
    RelojDiaNoche();
    RelojDiaNoche(float duracionDiaSegundos, float duracionNocheSegundos);
    ~RelojDiaNoche() = default;

    void actualizar(float segundosTranscurridos);
    void reiniciar();

    void configurarDuraciones(float duracionDiaSegundos, float duracionNocheSegundos);

    FaseDelDia obtenerFaseActual() const;
    float obtenerFactorDeIluminacionAmbiental() const;
    float obtenerTiempoTranscurridoEnCiclo() const;
    float obtenerDuracionTotalDelCiclo() const;
    bool esDeNoche() const;

private:
    void calcularFaseEIluminacion();

    float duracionDia;
    float duracionTransicion;
    float duracionNoche;
    float tiempoAcumuladoEnCiclo;

    FaseDelDia faseActual;
    float factorDeIluminacion;
};
```

## 5. CONSTANTES Y CURVA DE ILUMINACIÓN
```cpp
namespace ConstantesRelojDiaNoche {
    inline constexpr float ILUMINACION_MAXIMA_DIA = 1.0f;
    inline constexpr float ILUMINACION_MINIMA_NOCHE = 0.22f;
    inline constexpr float DURACION_POR_DEFECTO_TRANSICION = 10.0f;
}
```

La curva se divide en cuatro segmentos continuos:
1. `Dia`: Duración `duracionDia`. Iluminación fija en `1.0f`.
2. `Atardecer`: Duración `duracionTransicion`. Interpolación lineal de `1.0f` a `0.22f`.
3. `Noche`: Duración `duracionNoche`. Iluminación fija en `0.22f`.
4. `Amanecer`: Duración `duracionTransicion`. Interpolación lineal de `0.22f` a `1.0f`.

## 6. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `actualizar`
1. Incrementa `tiempoAcumuladoEnCiclo += segundosTranscurridos`.
2. Calcula `duracionTotal = duracionDia + duracionTransicion + duracionNoche + duracionTransicion`.
3. Si `tiempoAcumuladoEnCiclo >= duracionTotal`:
   - `tiempoAcumuladoEnCiclo = std::fmod(tiempoAcumuladoEnCiclo, duracionTotal)`.
4. Invoca `calcularFaseEIluminacion()`.

### `calcularFaseEIluminacion`
1. Límite 1 = `duracionDia`.
   - Si `t < Limite 1`: `faseActual = FaseDelDia::Dia`, `factorDeIluminacion = 1.0f`.
2. Límite 2 = `Limite 1 + duracionTransicion`.
   - Si `t < Limite 2`: `faseActual = FaseDelDia::Atardecer`.
   - Fracción normalizada `u = (t - Limite 1) / duracionTransicion`.
   - `factorDeIluminacion = 1.0f - u * (1.0f - ILUMINACION_MINIMA_NOCHE)`.
3. Límite 3 = `Limite 2 + duracionNoche`.
   - Si `t < Limite 3`: `faseActual = FaseDelDia::Noche`, `factorDeIluminacion = ILUMINACION_MINIMA_NOCHE`.
4. En cualquier otro caso (`t < duracionTotal`):
   - `faseActual = FaseDelDia::Amanecer`.
   - Fracción normalizada `u = (t - Limite 3) / duracionTransicion`.
   - `factorDeIluminacion = ILUMINACION_MINIMA_NOCHE + u * (1.0f - ILUMINACION_MINIMA_NOCHE)`.
