# ESPECIFICACION TECNICA: azar_con_semilla.h / azar_con_semilla.cpp

## Archivo de Cabecera: `azar_con_semilla.h`

### Guardas de inclusion
```cpp
#ifndef AZAR_CON_SEMILLA_H
#define AZAR_CON_SEMILLA_H
```

### Inclusiones requeridas
```cpp
#include <random>
#include <vector>
#include <algorithm>
#include "mapa/mapa.h"
```

### Declaracion de Clase `AzarConSemilla`
```cpp
class AzarConSemilla {
private:
    std::mt19937 motorDeNumerosAleatorios;
    unsigned int numeroDeSemillaActiva;

public:
    AzarConSemilla();
    void inicializarConSemillaDelSistema();
    void inicializarConSemillaFija(unsigned int semillaManual);
    unsigned int obtenerNumeroDeSemillaActiva() const;
    int generarEnteroEnRango(int minimoIncluido, int maximoIncluido);
    double generarDecimalEnRango(double minimoIncluido, double maximoIncluido);
    bool evaluarProbabilidadPorcentual(double porcentajeDeExito);
    void barajarListaDeCasillas(std::vector<CoordenadaCasilla>& listaDeCasillas);
};

#endif
```

---

## Archivo de Implementacion: `azar_con_semilla.cpp`

### Inclusiones requeridas
```cpp
#include "herramientas/azar_con_semilla.h"
```

### Implementacion de Metodos

#### 1. Constructor
- **Firma:** `AzarConSemilla::AzarConSemilla()`
- **Logica:**
  - `inicializarConSemillaDelSistema();`

#### 2. `inicializarConSemillaDelSistema`
- **Firma:** `void AzarConSemilla::inicializarConSemillaDelSistema()`
- **Logica:**
  - `std::random_device dispositivoAleatorio;`
  - `numeroDeSemillaActiva = dispositivoAleatorio();`
  - `motorDeNumerosAleatorios.seed(numeroDeSemillaActiva);`

#### 3. `inicializarConSemillaFija`
- **Firma:** `void AzarConSemilla::inicializarConSemillaFija(unsigned int semillaManual)`
- **Logica:**
  - `numeroDeSemillaActiva = semillaManual;`
  - `motorDeNumerosAleatorios.seed(numeroDeSemillaActiva);`

#### 4. `obtenerNumeroDeSemillaActiva`
- **Firma:** `unsigned int AzarConSemilla::obtenerNumeroDeSemillaActiva() const`
- **Logica:**
  - Retorna `numeroDeSemillaActiva;`

#### 5. `generarEnteroEnRango`
- **Firma:** `int AzarConSemilla::generarEnteroEnRango(int minimoIncluido, int maximoIncluido)`
- **Logica:**
  - `std::uniform_int_distribution<int> distribucion(minimoIncluido, maximoIncluido);`
  - Retorna `distribucion(motorDeNumerosAleatorios);`

#### 6. `generarDecimalEnRango`
- **Firma:** `double AzarConSemilla::generarDecimalEnRango(double minimoIncluido, double maximoIncluido)`
- **Logica:**
  - `std::uniform_real_distribution<double> distribucion(minimoIncluido, maximoIncluido);`
  - Retorna `distribucion(motorDeNumerosAleatorios);`

#### 7. `evaluarProbabilidadPorcentual`
- **Firma:** `bool AzarConSemilla::evaluarProbabilidadPorcentual(double porcentajeDeExito)`
- **Logica:**
  - `std::uniform_real_distribution<double> distribucion(0.0, 100.0);`
  - Retorna `distribucion(motorDeNumerosAleatorios) <= porcentajeDeExito;`

#### 8. `barajarListaDeCasillas`
- **Firma:** `void AzarConSemilla::barajarListaDeCasillas(std::vector<CoordenadaCasilla>& listaDeCasillas)`
- **Logica:**
  - `std::shuffle(listaDeCasillas.begin(), listaDeCasillas.end(), motorDeNumerosAleatorios);`
