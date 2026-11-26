# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: MISIÓN EXAMINAR

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `MisionExaminar` hereda de `MisionBase` y gestiona la inspección minuciosa de una zona del escenario o de un objeto estático (estatua, reloj antiguo, mecanismo de defensa, cañón colonial o marca en la pared). Requiere que el jugador permanezca en la zona de inspección durante un tiempo sostenido (o active la lupa de investigación) sin ser interrumpido por enemigos.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include "investigacion/misiones/mision.h"
#include "herramientas/tipos_espaciales.h"
#include <cstdint>

class MundoDeCartagena;
```

## 3. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class MisionExaminar : public MisionBase {
public:
    MisionExaminar(
        int identificadorUnico,
        std::string tituloDescriptivo,
        std::string descripcionDetallada,
        int recompensaDePuntaje,
        CoordenadaEntera casillaDeInspeccion,
        float radioDeTolerancia,
        float tiempoRequeridoDeInspeccionSostenida
    );

    ~MisionExaminar() override = default;

    void iniciar(MundoDeCartagena& mundo) override;
    void actualizar(MundoDeCartagena& mundo, float segundosTranscurridos) override;
    bool verificarCondicionDeCompletitud(MundoDeCartagena& mundo) override;
    void completar(MundoDeCartagena& mundo) override;
    void reiniciar() override;

    const CoordenadaEntera& obtenerCasillaDeInspeccion() const;
    float obtenerTiempoRequeridoDeInspeccionSostenida() const;
    float obtenerTiempoAcumuladoDeInspeccion() const;
    float obtenerPorcentajeDeProgreso() const;

private:
    CoordenadaEntera casillaDeInspeccion;
    float radioDeTolerancia;
    float radioDeToleranciaCuadrado;
    float tiempoRequeridoDeInspeccionSostenida;
    float tiempoAcumuladoDeInspeccion;
};
```

## 4. INVARIANTES DE CLASE Y REGLAS DE NEGOCIO
1. `tiempoRequeridoDeInspeccionSostenida` debe ser estrictamente mayor a 0.0f.
2. `tiempoAcumuladoDeInspeccion` se incrementa únicamente si la posición de Scooby está dentro del radio de tolerancia. Si Scooby sale del radio, el tiempo acumulado decae progresivamente a cero (castigo por interrupción).
3. `obtenerPorcentajeDeProgreso()` retorna un valor flotante normalizado entre `0.0f` y `1.0f`.

## 5. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `iniciar`
1. Establece `estado = EstadoDeMision::EnProgreso`.
2. Restablece `tiempoAcumuladoDeInspeccion = 0.0f`.
3. Precalcula `radioDeToleranciaCuadrado = radioDeTolerancia * radioDeTolerancia`.

### `actualizar`
1. Si el estado no es `EstadoDeMision::EnProgreso`, retorna.
2. Obtiene la posición en píxeles del centro de `casillaDeInspeccion`.
3. Obtiene la posición de Scooby desde `mundo.obtenerScooby().obtenerPosicionActual()`.
4. Calcula la distancia cuadrada entre ambas posiciones.
5. Si la distancia cuadrada es menor o igual a `radioDeToleranciaCuadrado`:
   - Incrementa `tiempoAcumuladoDeInspeccion += segundosTranscurridos`.
   - Si `tiempoAcumuladoDeInspeccion > tiempoRequeridoDeInspeccionSostenida`, lo fija en ese valor.
6. Si Scooby está fuera del radio:
   - Decrementa `tiempoAcumuladoDeInspeccion -= segundosTranscurridos * 1.5f`.
   - Si es menor a cero, lo fija en `0.0f`.
7. Si `verificarCondicionDeCompletitud(mundo)` es verdadero, invoca `completar(mundo)`.

### `verificarCondicionDeCompletitud`
Retorna `tiempoAcumuladoDeInspeccion >= tiempoRequeridoDeInspeccionSostenida`.
