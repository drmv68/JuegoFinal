# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: MISIÓN HABLAR

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `MisionHablar` hereda de `MisionBase` y gestiona los objetivos en los que Scooby debe localizar a un personaje no jugable específico (ciudadano, pescador, comerciante o miembro de Mystery Inc.), interactuar con él dentro de un radio de proximidad definido y sostener el diálogo hasta recibir la pista o confirmación del avance de la investigación.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include "investigacion/misiones/mision.h"
#include <string>

class MundoDeCartagena;
```

## 3. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class MisionHablar : public MisionBase {
public:
    MisionHablar(
        int identificadorUnico,
        std::string tituloDescriptivo,
        std::string descripcionDetallada,
        int recompensaDePuntaje,
        int identificadorDelPersonajeObjetivo,
        std::string dialogoAlInteractuar,
        float distanciaMaximaDeInteraccion
    );

    ~MisionHablar() override = default;

    void iniciar(MundoDeCartagena& mundo) override;
    void actualizar(MundoDeCartagena& mundo, float segundosTranscurridos) override;
    bool verificarCondicionDeCompletitud(MundoDeCartagena& mundo) override;
    void completar(MundoDeCartagena& mundo) override;
    void reiniciar() override;

    int obtenerIdentificadorDelPersonajeObjetivo() const;
    const std::string& obtenerDialogoAlInteractuar() const;
    bool estaConversacionFinalizada() const;

    void registrarInteraccionDelJugador();

private:
    int identificadorDelPersonajeObjetivo;
    std::string dialogoAlInteractuar;
    float distanciaMaximaDeInteraccion;
    float distanciaMaximaCuadrada;
    bool interaccionRegistrada;
    bool conversacionFinalizada;
};
```

## 4. CONSTANTES DE COMPORTAMIENTO
```cpp
namespace ConstantesMisionHablar {
    inline constexpr float DISTANCIA_POR_DEFECTO_DE_INTERACCION = 48.0f;
}
```

## 5. INVARIANTES DE CLASE Y REGLAS DE NEGOCIO
1. `identificadorDelPersonajeObjetivo` debe coincidir con el identificador asignado al NPC correspondiente en `MundoDeCartagena`.
2. Para evitar operaciones costosas de raíz cuadrada en cada ciclo de actualización, se almacena `distanciaMaximaCuadrada = distanciaMaximaDeInteraccion * distanciaMaximaDeInteraccion`.
3. La misión solo se considera completada si `interaccionRegistrada` y `conversacionFinalizada` son verdaderos, y Scooby se encuentra dentro del rango de proximidad al momento del suceso.

## 6. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `iniciar`
1. Cambia el estado a `EstadoDeMision::EnProgreso`.
2. Restablece `interaccionRegistrada = false` y `conversacionFinalizada = false`.

### `actualizar`
1. Si el estado no es `EstadoDeMision::EnProgreso`, retorna inmediatamente.
2. Si `interaccionRegistrada` es verdadero, no requiere cálculos de distancia continuos y espera la señal de fin de diálogo.
3. Si el jugador presiona la tecla de acción frente al personaje objetivo, el sistema externo o `MundoDeCartagena` invoca `registrarInteraccionDelJugador()`.
4. Evalúa `verificarCondicionDeCompletitud(mundo)`. Si retorna verdadero, invoca `completar(mundo)`.

### `verificarCondicionDeCompletitud`
1. Comprueba si `interaccionRegistrada && conversacionFinalizada`.
2. Retorna el resultado booleano.

### `registrarInteraccionDelJugador`
1. Marca `interaccionRegistrada = true`.
2. Marca `conversacionFinalizada = true`.
