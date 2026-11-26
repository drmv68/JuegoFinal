# ESPECIFICACIÓN TÉCNICA DE BAJO NIVEL: RECURSOS DE LA PARTIDA

## 1. PROPÓSITO ARQUITECTÓNICO Y ROL EN EL SISTEMA
La clase `RecursosDeLaPartida` encapsula los contadores numéricos volátiles de la sesión de juego en curso: galletas Scooby recolectadas, gemas coloniales, llaves de accesos, trampas disponibles en el inventario rápido y modificadores de puntaje temporal. Provee métodos con control de límites para evitar subdesbordamientos o incrementos ilimitados.

## 2. INCLUSIONES Y DEPENDENCIAS ESTRICTAS
```cpp
#pragma once

#include <cstdint>
```

## 3. DECLARACIÓN COMPLETA DE LA CLASE
```cpp
class RecursosDeLaPartida {
public:
    RecursosDeLaPartida();
    explicit RecursosDeLaPartida(int vidasIniciales);
    ~RecursosDeLaPartida() = default;

    int obtenerVidasRestantes() const;
    int obtenerGalletasScooby() const;
    int obtenerGemasColoniales() const;
    int obtenerLlavesDeAcceso() const;
    int obtenerTrampasDisponibles() const;
    int obtenerPuntajeDePartida() const;

    void reiniciarConVidas(int vidasIniciales);

    void sumarGalletas(int cantidad);
    bool consumirGalletas(int cantidad);

    void sumarGemas(int cantidad);
    void sumarLlaves(int cantidad);
    bool consumirLlave();

    void sumarTrampas(int cantidad);
    bool consumirTrampa();

    void sumarPuntaje(int puntos);
    void penalizarPuntaje(int puntos);

    void decrementarVida();
    void incrementarVida();
    bool estaSinVidas() const;

private:
    int vidasRestantes;
    int galletasScooby;
    int gemasColoniales;
    int llavesDeAcceso;
    int trampasDisponibles;
    int puntajeDePartida;
};
```

## 4. CONSTANTES DE RECURSOS
```cpp
namespace ConstantesRecursos {
    inline constexpr int MAXIMO_GALLETAS_PERMITIDAS = 999;
    inline constexpr int MAXIMO_GEMAS_PERMITIDAS = 999;
    inline constexpr int MAXIMO_LLAVES_PERMITIDAS = 10;
    inline constexpr int MAXIMO_TRAMPAS_PERMITIDAS = 5;
    inline constexpr int MAXIMO_VIDAS_PERMITIDAS = 9;
}
```

## 5. INVARIANTES DE CLASE Y REGLAS DE NEGOCIO
1. Ningún recurso puede ser negativo. Cualquier intento de consumo que exceda la cantidad disponible debe retornar `false` y no modificar el contador.
2. Si `vidasRestantes` llega a 0, `estaSinVidas()` retorna `true`, lo que activa la condición de Game Over en el controlador.
3. El puntaje se satura en 0 ante penalizaciones severas.

## 6. LÓGICA PASO A PASO DE IMPLEMENTACIÓN
### `consumirGalletas`
1. Si `galletasScooby >= cantidad`:
   - `galletasScooby -= cantidad`.
   - Retorna verdadero.
2. Retorna falso.

### `consumirLlave`
1. Si `llavesDeAcceso > 0`:
   - `llavesDeAcceso--`.
   - Retorna verdadero.
2. Retorna falso.

### `consumirTrampa`
1. Si `trampasDisponibles > 0`:
   - `trampasDisponibles--`.
   - Retorna verdadero.
2. Retorna falso.

### `decrementarVida`
1. Si `vidasRestantes > 0`:
   - `vidasRestantes--`.
