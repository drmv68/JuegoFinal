# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: MISIÓN PÉNDULO

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `MisionPendulo` hereda de `MisionBase` y modela los desafíos mecánicos y de sincronización temporal en los que el jugador debe interactuar con un objeto oscilante (lámpara colonial, badajo de campana de la iglesia de San Pedro Claver o trampa de péndulo) en una ventana angular específica para destrabar un pasadizo o desactivar una alarma.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include "investigacion/misiones/mision.h"
#include <cstdint>

class MundoDeCartagena;
```

## 3. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class MisionPendulo : public MisionBase {
public:
    MisionPendulo(
        int identificadorUnico,
        std::string tituloDescriptivo,
        std::string descripcionDetallada,
        int recompensaDePuntaje,
        int identificadorDelObjetoPendulo,
        float anguloObjetivoRadianes,
        float margenDeToleranciaRadianes,
        int aciertosConsecutivosRequeridos
    );

    ~MisionPendulo() override = default;

    void iniciar(MundoDeCartagena& mundo) override;
    void actualizar(MundoDeCartagena& mundo, float segundosTranscurridos) override;
    bool verificarCondicionDeCompletitud(MundoDeCartagena& mundo) override;
    void completar(MundoDeCartagena& mundo) override;
    void reiniciar() override;

    int obtenerIdentificadorDelObjetoPendulo() const;
    float obtenerAnguloObjetivoRadianes() const;
    float obtenerMargenDeToleranciaRadianes() const;
    int obtenerAciertosConsecutivosRequeridos() const;
    int obtenerAciertosActuales() const;

    bool intentarAccionDeSincronizacion(float anguloActualDelPendulo);

private:
    int identificadorDelObjetoPendulo;
    float anguloObjetivoRadianes;
    float margenDeToleranciaRadianes;
    int aciertosConsecutivosRequeridos;
    int aciertosActuales;
};
```

## 4. INVARIANTES DE CLASE Y REGLAS DE NEGOCIO
1. `aciertosConsecutivosRequeridos` debe ser al menos 1.
2. Si un intento de sincronización falla (`intentarAccionDeSincronizacion` con ángulo fuera del margen tolerado), `aciertosActuales` se reinicia a cero (penalización estricta de ritmo).
3. `verificarCondicionDeCompletitud` evalúa `aciertosActuales >= aciertosConsecutivosRequeridos`.

## 5. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `intentarAccionDeSincronizacion`
1. Calcula la diferencia absoluta `std::abs(anguloActualDelPendulo - anguloObjetivoRadianes)`.
2. Si la diferencia es menor o igual a `margenDeToleranciaRadianes`:
   - Incrementa `aciertosActuales++`.
   - Retorna verdadero.
3. Si la diferencia es mayor:
   - Reinicia `aciertosActuales = 0`.
   - Retorna falso.
