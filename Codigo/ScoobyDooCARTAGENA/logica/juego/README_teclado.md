# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: TECLADO Y ENTRADA DE USUARIO

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `GestorDeTeclado` (y su estructura auxiliar desacoplada `EstadoDeTeclado`) aísla la entrada del usuario capturada en los widgets de interfaz gráfica del motor de lógica pura. Registra teclas sostenidas (movimiento continuo) y pulsaciones únicas (disparos de interacción o uso de objetos), proporcionando un método de limpieza de eventos instantáneos al inicio o fin de cada fotograma lógico.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include <cstdint>
```

## 3. ESTRUCTURA DE ESTADO
```cpp
struct EstadoDeTeclado {
    bool arribaPresionado;
    bool abajoPresionado;
    bool izquierdaPresionado;
    bool derechaPresionado;
    bool correrPresionado;
    bool interactuarPulsado;
    bool usarObjetoPulsado;
    bool pausarPulsado;

    void reiniciar() {
        arribaPresionado = false;
        abajoPresionado = false;
        izquierdaPresionado = false;
        derechaPresionado = false;
        correrPresionado = false;
        interactuarPulsado = false;
        usarObjetoPulsado = false;
        pausarPulsado = false;
    }

    void limpiarPulsacionesInstantaneas() {
        interactuarPulsado = false;
        usarObjetoPulsado = false;
        pausarPulsado = false;
    }
};
```

## 4. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class GestorDeTeclado {
public:
    GestorDeTeclado();
    ~GestorDeTeclado() = default;

    void registrarPresionDeTecla(int codigoDeTeclaQt);
    void registrarLiberacionDeTecla(int codigoDeTeclaQt);

    const EstadoDeTeclado& obtenerEstadoActual() const;
    void reiniciarEstadoCompleto();
    void limpiarPulsacionesInstantaneas();

private:
    EstadoDeTeclado estadoActual;
};
```

## 5. INVARIANTES DE CLASE Y REGLAS DE NEGOCIO
1. Los eventos de movimiento (`arriba`, `abajo`, `izquierda`, `derecha`, `correr`) representan estado continuo y persisten hasta que se registre la liberación de la tecla.
2. Los eventos de acción (`interactuarPulsado`, `usarObjetoPulsado`, `pausarPulsado`) son pulsaciones de disparo único (edge-triggered). Tras ser consumidos en el ciclo de actualización de `ControladorDelJuego`, deben ser reiniciados mediante `limpiarPulsacionesInstantaneas()`.

## 6. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `registrarPresionDeTecla`
Evalúa el código numérico de tecla de Qt (por ejemplo: `Qt::Key_W`, `Qt::Key_Up`, `Qt::Key_E`, `Qt::Key_Space`, `Qt::Key_Shift`):
- Direccionales: asigna verdadero al booleano correspondiente.
- Acciones: asigna verdadero al booleano de pulsación instantánea.

### `registrarLiberacionDeTecla`
Evalúa el código y asigna falso a las teclas continuas correspondientes.
