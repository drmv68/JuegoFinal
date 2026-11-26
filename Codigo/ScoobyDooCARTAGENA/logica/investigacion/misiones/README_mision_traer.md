# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: MISIÓN TRAER

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `MisionTraer` hereda de `MisionBase` y gestiona los objetivos tipo recolección y entrega (fetch-quest). Requiere que Scooby recolecte una cantidad determinada de objetos específicos en el mapa (por ejemplo, Scooby Galletas, piezas de motor, llaves del galeón o frutas) y los transporte o entregue a un personaje destinatario o a un cofre en una ubicación determinada.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include "investigacion/misiones/mision.h"
#include "personajes/objeto_recolectable.h"
#include <cstdint>

class MundoDeCartagena;
```

## 3. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class MisionTraer : public MisionBase {
public:
    MisionTraer(
        int identificadorUnico,
        std::string tituloDescriptivo,
        std::string descripcionDetallada,
        int recompensaDePuntaje,
        TipoDeObjetoRecolectable tipoDeObjetoRequerido,
        int cantidadTotalRequerida,
        int identificadorDelDestinatario,
        CoordenadaEntera coordenadaDeEntrega
    );

    ~MisionTraer() override = default;

    void iniciar(MundoDeCartagena& mundo) override;
    void actualizar(MundoDeCartagena& mundo, float segundosTranscurridos) override;
    bool verificarCondicionDeCompletitud(MundoDeCartagena& mundo) override;
    void completar(MundoDeCartagena& mundo) override;
    void reiniciar() override;

    TipoDeObjetoRecolectable obtenerTipoDeObjetoRequerido() const;
    int obtenerCantidadTotalRequerida() const;
    int obtenerCantidadActualRecolectada() const;
    int obtenerIdentificadorDelDestinatario() const;
    const CoordenadaEntera& obtenerCoordenadaDeEntrega() const;

    void registrarObjetoRecolectado();
    bool estaListoParaEntrega() const;
    void confirmarEntregaExitosa();

private:
    TipoDeObjetoRecolectable tipoDeObjetoRequerido;
    int cantidadTotalRequerida;
    int cantidadActualRecolectada;
    int identificadorDelDestinatario;
    CoordenadaEntera coordenadaDeEntrega;
    bool entregaRealizada;
};
```

## 4. INVARIANTES DE CLASE Y REGLAS DE NEGOCIO
1. `cantidadTotalRequerida` debe ser estrictamente mayor a 0.
2. `cantidadActualRecolectada` nunca puede exceder `cantidadTotalRequerida`.
3. `estaListoParaEntrega()` retorna verdadero únicamente cuando `cantidadActualRecolectada == cantidadTotalRequerida`.
4. La entrega solo es exitosa si Scooby se encuentra dentro del radio de interacción del destinatario o de la casilla `coordenadaDeEntrega`.

## 5. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `iniciar`
1. Establece `estado = EstadoDeMision::EnProgreso`.
2. Restablece `cantidadActualRecolectada = 0`.
3. Restablece `entregaRealizada = false`.

### `actualizar`
1. Verifica si `estado == EstadoDeMision::EnProgreso`. De lo contrario, retorna.
2. Si `estaListoParaEntrega()` y Scooby activa la interacción con el destinatario o la coordenada de entrega, se invoca `confirmarEntregaExitosa()`.
3. Evalúa `verificarCondicionDeCompletitud(mundo)`. Si resulta verdadero, invoca `completar(mundo)`.

### `registrarObjetoRecolectado`
1. Si `cantidadActualRecolectada < cantidadTotalRequerida`:
   - Incrementa `cantidadActualRecolectada++`.

### `confirmarEntregaExitosa`
1. Si `estaListoParaEntrega()`:
   - Marca `entregaRealizada = true`.
