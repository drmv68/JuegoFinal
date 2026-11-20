# ESPECIFICACION TECNICA: dificultad.h / dificultad.cpp

## Archivo de Cabecera: `dificultad.h`

### Guardas de inclusion
```cpp
#ifndef DIFICULTAD_H
#define DIFICULTAD_H
```

### Enumeracion de Niveles
```cpp
enum class NivelDeDificultad {
    Facil,
    Medio,
    Dificil,
    Leyenda
};
```

### Estructura `ParametrosDeDificultad`
```cpp
struct ParametrosDeDificultad {
    // Fisicas matematicas
    double frecuenciaDelPenduloEnHertz;
    double amplitudDelPenduloEnRadianes;
    double radioDelCentinelaEnCasillas;
    double velocidadAngularDelCentinela;
    double segundosEntreDisparosDeCanon;

    // Inteligencia artificial
    int cantidadDeSecuaces;
    double rangoDeVisionDeSecuacesEnCasillas;
    double segundosDeRalentizacion;
    int avistamientosParaAlertaDelVillano;
    double segundosDeEnfriamientoDelVillano;
    double segundosParaConfirmarSospecha;

    // Economia y dinamicas
    double segundosIniciales;
    double provisionesPerdidasPorSegundo;
    double multiplicadorDeRoboNocturno;
    double segundosDelDia;
    double segundosDeLaNoche;
    int oportunidadesDeAcusacionIniciales;
    bool mostrarPistaCercanaDeLaTrampa;
};
```

### Declaracion de Clase `Dificultad`
```cpp
class Dificultad {
public:
    static ParametrosDeDificultad obtenerParametros(NivelDeDificultad nivel);
    static void aplicarAsistenciaPorDerrotas(ParametrosDeDificultad& parametros, int derrotasSeguidas);
};

#endif
```

---

## Archivo de Implementacion: `dificultad.cpp`

### Inclusiones requeridas
```cpp
#include "juego/dificultad.h"
```

### Implementacion de Metodos

#### 1. `obtenerParametros`
- Retorna struct con valores segun nivel:

**Facil:**
- Fisicas: `frecuencia = 0.30 Hz`, `amplitud = 0.40 rad`, `radioCentinela = 3.0 cas`, `velAngular = 0.7 rad/s`, `canon = cada 14.0 s`.
- IA: `3 secuaces`, `vision = 4.0 cas`, `ralentizacion = 2.0 s`, `avistamientos = 4`, `enfriamientoVillano = 30.0 s`, `sospecha = 0.6 s`.
- Economia: `1080.0 s (18 min)`, `drenaje = 0.15 prov/s`, `roboNocturno = 1.5`, `dia = 270.0 s`, `noche = 135.0 s`, `oportunidades = 3`, `pistaCercana = true`.

**Medio:**
- Fisicas: `frecuencia = 0.40 Hz`, `amplitud = 0.50 rad`, `radioCentinela = 3.0 cas`, `velAngular = 0.9 rad/s`, `canon = cada 11.0 s`.
- IA: `5 secuaces`, `vision = 5.0 cas`, `ralentizacion = 3.0 s`, `avistamientos = 3`, `enfriamientoVillano = 25.0 s`, `sospecha = 0.4 s`.
- Economia: `900.0 s (15 min)`, `drenaje = 0.20 prov/s`, `roboNocturno = 2.0`, `dia = 270.0 s`, `noche = 135.0 s`, `oportunidades = 2`, `pistaCercana = false`.

**Dificil:**
- Fisicas: `frecuencia = 0.50 Hz`, `amplitud = 0.60 rad`, `radioCentinela = 3.5 cas`, `velAngular = 1.1 rad/s`, `canon = cada 8.0 s`.
- IA: `7 secuaces`, `vision = 6.0 cas`, `ralentizacion = 3.5 s`, `avistamientos = 2`, `enfriamientoVillano = 20.0 s`, `sospecha = 0.3 s`.
- Economia: `780.0 s (13 min)`, `drenaje = 0.25 prov/s`, `roboNocturno = 2.5`, `dia = 240.0 s`, `noche = 150.0 s`, `oportunidades = 2`, `pistaCercana = false`.

**Leyenda:**
- Fisicas: `frecuencia = 0.60 Hz`, `amplitud = 0.70 rad`, `radioCentinela = 4.0 cas`, `velAngular = 1.3 rad/s`, `canon = cada 6.0 s`.
- IA: `9 secuaces`, `vision = 7.0 cas`, `ralentizacion = 4.0 s`, `avistamientos = 2`, `enfriamientoVillano = 15.0 s`, `sospecha = 0.2 s`.
- Economia: `600.0 s (10 min)`, `drenaje = 0.30 prov/s`, `roboNocturno = 3.0`, `dia = 210.0 s`, `noche = 180.0 s`, `oportunidades = 2`, `pistaCercana = false`.

#### 2. `aplicarAsistenciaPorDerrotas`
- Si `derrotasSeguidas >= 2`:
  - `parametros.rangoDeVisionDeSecuacesEnCasillas *= 0.90;` (asistencia silenciosa reduciendo 10% la vision).
