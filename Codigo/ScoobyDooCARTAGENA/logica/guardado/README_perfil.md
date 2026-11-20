# ESPECIFICACION TECNICA: perfil.h

## Archivo de Cabecera: `perfil.h` (Solo .h)

### Guardas de inclusion
```cpp
#ifndef PERFIL_H
#define PERFIL_H
```

### Inclusiones requeridas
```cpp
#include <QString>
#include <vector>
#include <array>
#include "juego/dificultad.h"
```

### Declaraciones Adelantadas
```cpp
enum class NivelDeDificultad;
```

### Estructura `PerfilDeUsuario`
```cpp
struct PerfilDeUsuario {
    QString nombreDelDetective;
    bool nivelUnoTutorialSuperado;
    NivelDeDificultad ultimaDificultadSeleccionada;
    int mejorTiempoEnSegundos;
    int casosResueltosTotales;
    std::array<bool, 5> villanosDesenmascarados;
    std::vector<int> ultimasMisionesJugadas;
    double volumenGuardado;

    PerfilDeUsuario()
        : nombreDelDetective("Scooby")
        , nivelUnoTutorialSuperado(false)
        , ultimaDificultadSeleccionada(static_cast<NivelDeDificultad>(0))
        , mejorTiempoEnSegundos(9999)
        , casosResueltosTotales(0)
        , villanosDesenmascarados{false, false, false, false, false}
        , volumenGuardado(0.8) {}

    inline QString obtenerRangoActualDelDetective() const {
        if (casosResueltosTotales <= 2) {
            return "Aprendiz";
        } else if (casosResueltosTotales <= 7) {
            return "Detective";
        } else if (casosResueltosTotales <= 15) {
            return "Sabueso";
        }
        return "Leyenda de Cartagena";
    }

    inline int contarVillanosDesenmascarados() const {
        int contador = 0;
        for (bool atrapado : villanosDesenmascarados) {
            if (atrapado) {
                contador++;
            }
        }
        return contador;
    }
};

#endif
```
