# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: PERFIL DE USUARIO

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `PerfilDeUsuario` encapsula los datos persistentes del jugador: nombre del detective, puntaje histórico, casos resueltos, tiempo acumulado de juego y rango obtenido. Debe estar completamente encapsulada con atributos privados y métodos con invariantes de validación para garantizar la coherencia antes de serializar o tras deserializar del disco.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include <string>
#include <vector>
#include <cstdint>
```

## 3. ENUMERACIONES Y TIPOS
```cpp
enum class RangoDelDetective : uint8_t {
    NovatoDeMisterio = 0,
    RastreadorDeCartagena = 1,
    SabuesoDeBaluartes = 2,
    InvestigadorMaestro = 3
};
```

## 4. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class PerfilDeUsuario {
public:
    PerfilDeUsuario();
    explicit PerfilDeUsuario(std::string nombreDelDetective);

    const std::string& obtenerNombre() const;
    int obtenerPuntajeTotal() const;
    int obtenerCasosResueltos() const;
    float obtenerTiempoTotalJugadoSegundos() const;
    RangoDelDetective obtenerRango() const;
    const std::vector<std::string>& obtenerHistorialDeMisiones() const;

    void establecerNombre(const std::string& nuevoNombre);
    void sumarPuntaje(int puntos);
    void decrementarPuntaje(int puntos);
    void registrarCasoResuelto();
    void acumularTiempoJugado(float segundos);
    void agregarMisionAlHistorial(const std::string& descripcionMision);

    void reiniciarEstadisticas();

private:
    void actualizarRangoSegunProgreso();

    std::string nombre;
    int puntajeTotal;
    int casosResueltos;
    float tiempoTotalJugadoSegundos;
    RangoDelDetective rango;
    std::vector<std::string> historialDeMisiones;
};
```

## 5. INVARIANTES DE CLASE Y REGLAS DE NEGOCIO
1. El `nombre` no puede ser una cadena vacía; si se asigna vacía, se normaliza por defecto a `"Detective Misterio"`.
2. El `puntajeTotal` nunca puede ser negativo; si una penalización reduce el puntaje por debajo de cero, se satura en 0.
3. El rango se calcula automáticamente tras cada modificación de puntaje y casos resueltos:
   - `InvestigadorMaestro`: `casosResueltos >= 3 && puntajeTotal >= 3000`.
   - `SabuesoDeBaluartes`: `casosResueltos >= 2 && puntajeTotal >= 1800`.
   - `RastreadorDeCartagena`: `casosResueltos >= 1 && puntajeTotal >= 800`.
   - `NovatoDeMisterio`: en cualquier otro caso.
4. `historialDeMisiones` tiene un tamaño máximo de 20 entradas (FIFO o truncamiento) para evitar crecimiento descontrolado de memoria.

## 6. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `sumarPuntaje`
1. Si `puntos > 0`: `puntajeTotal += puntos`.
2. Invoca `actualizarRangoSegunProgreso()`.

### `decrementarPuntaje`
1. `puntajeTotal = std::max(0, puntajeTotal - puntos)`.
2. Invoca `actualizarRangoSegunProgreso()`.

### `agregarMisionAlHistorial`
1. Inserta la descripción al final de `historialDeMisiones`.
2. Si `historialDeMisiones.size() > 20`: elimina el primer elemento más antiguo (`historialDeMisiones.erase(historialDeMisiones.begin())`).
