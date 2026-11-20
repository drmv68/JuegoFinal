# ESPECIFICACION TECNICA: caso.h / caso.cpp

## Archivo de Cabecera: `caso.h`

### Guardas de inclusion
```cpp
#ifndef CASO_H
#define CASO_H
```

### Inclusiones requeridas
```cpp
#include <QString>
#include <vector>
#include <QJsonObject>
#include "mapa/mapa.h"
#include "investigacion/misiones/mision.h"
#include "herramientas/azar_con_semilla.h"
#include "guardado/perfil.h"
```

### Estructuras y Enumeraciones
```cpp
struct FichaDeSospechoso {
    QString nombre;
    QString oficioColonial;
    int rasgosCaracteristicos[3];
    QString claveDelRetrato;
    QString fragmentoDeLaCarta;
    QString fraseAlSerDescubierto;
};

enum class TipoDeGiroSorpresa {
    CambioDeGuardia,
    NieblaDelPuerto,
    BombardeoIngles,
    RumorFalso
};
```

### Declaracion de Clase `Caso`
```cpp
class Caso {
private:
    FichaDeSospechoso culpableSorteado;
    std::vector<FichaDeSospechoso> listaDeCincoSospechosos;
    std::vector<bool> sospechososTachados;
    CoordenadaCasilla casillaCentralDeLaTrampa;
    TipoDeGiroSorpresa giroSorpresaDeLaPartida;
    std::vector<MisionBase*> listaDeMisionesActivas;
    int pasoActualDeUbicacion;
    std::vector<QString> anotacionesDeLaLibreta;

    QString textoDelSector;
    QString textoDeLaReferencia;
    QString textoDeLaMarca;

    MisionBase* crearMisionDesdeEntradaDelBanco(const QJsonObject& entrada, const Mapa& mapa, AzarConSemilla& azar);
    QString reemplazarEtiquetasDelTexto(const QString& textoOriginal) const;

public:
    Caso();
    ~Caso();

    void armarCasoFijoNivelUno(const Mapa& mapa, const ObjetoDelEscenario* lampara);
    void sortearCasoAleatorioNivelDos(AzarConSemilla& azar, const Mapa& mapa, const PerfilDeUsuario& perfil, const QJsonObject& bancoMisiones, const QJsonObject& bancoIdentidades, const QJsonObject& bancoTrampas, const QJsonObject& bancoGiros);

    std::vector<EfectoDeMisionEnElMundo> evaluarProgresoDeMisionesEnCuadro(const SituacionActualDelMundo& situacion);
    std::vector<int> registrarEvidenciaYDescartarSospechosos(int codigoDeEvidencia);
    bool estaTrampaListaParaSerArmada() const;

    const FichaDeSospechoso& obtenerCulpable() const;
    const std::vector<FichaDeSospechoso>& obtenerSospechosos() const;
    const std::vector<bool>& obtenerSospechososTachados() const;
    CoordenadaCasilla obtenerCasillaCentralDeLaTrampa() const;
    TipoDeGiroSorpresa obtenerGiroSorpresa() const;
    int obtenerPasoActualDeUbicacion() const;
    const std::vector<QString>& obtenerAnotacionesDeLaLibreta() const;
};

#endif
```

---

## Archivo de Implementacion: `caso.cpp`

### Inclusiones requeridas
```cpp
#include "investigacion/caso.h"
#include "investigacion/misiones/mision_hablar.h"
#include "investigacion/misiones/mision_traer.h"
#include "investigacion/misiones/mision_examinar.h"
#include "investigacion/misiones/mision_pendulo.h"
#include <QJsonArray>
```

### Implementacion de Metodos

#### 1. Constructor
- `pasoActualDeUbicacion = 0;`
- `sospechososTachados.assign(5, false);`

#### 2. Destructor
- Itera en `listaDeMisionesActivas`: `for (MisionBase* mision : listaDeMisionesActivas) { delete mision; }`
- `listaDeMisionesActivas.clear();`

#### 3. `armarCasoFijoNivelUno`
- Limpia misiones previas.
- Crea Mision 1: `MisionHablar(1, 0, "Parte del centinela", TipoRolDelPersonaje::CentinelaAlmacen)`. Agrega efecto al completar: entregar libreta y gema.
- Crea Mision 2: `MisionPendulo(2, 0, "La sombra que baila", lampara)`.
- Ambas se agregan a `listaDeMisionesActivas`.

#### 4. `sortearCasoAleatorioNivelDos`
- Parsea identidades desde `bancoIdentidades["identidades"].toArray()`.
- Sortea culpable prefiriendo villanos no desenmascarados en `perfil.villanosDesenmascarados`.
- Sortea `casillaCentralDeLaTrampa` entre `mapa.obtenerCasillasAptasParaTrampa()`.
- Busca sector y referencia mas cercana a la trampa; extrae `textoDelSector` y `textoDeLaReferencia`.
- Sortea variante de trampa de `bancoTrampas` y llena `textoDeLaMarca`.
- Sortea giro de `bancoGiros`.
- Sortea 1 mision por paso (sector, referencia, marca) de `bancoMisiones`. Aplica `reemplazarEtiquetasDelTexto` sobre los textos.
- Prepara cada mision con `prepararMision(azar, mapa)`.

#### 5. `reemplazarEtiquetasDelTexto`
- Reemplaza `{sector}` con `textoDelSector`.
- Reemplaza `{referencia}` con `textoDeLaReferencia`.
- Reemplaza `{marca}` con `textoDeLaMarca`.

#### 6. `evaluarProgresoDeMisionesEnCuadro`
- std::vector<EfectoDeMisionEnElMundo> efectosGenerados;
- Si `pasoActualDeUbicacion < static_cast<int>(listaDeMisionesActivas.size())`:
  - `MisionBase* misionActiva = listaDeMisionesActivas[pasoActualDeUbicacion];`
  - Si `misionActiva->evaluarCumplimientoEnCuadro(situacion)`:
    - Agrega sus `listaDeEfectosAlCompletar` a `efectosGenerados`.
    - Agrega `misionActiva->obtenerTextoDeLaPistaParaLaLibreta()` a `anotacionesDeLaLibreta`.
    - `pasoActualDeUbicacion++;`
    - Si `pasoActualDeUbicacion < static_cast<int>(listaDeMisionesActivas.size())`:
      - Agrega las `listaDeEfectosAlIniciar` de la nueva mision a `efectosGenerados`.
- Retorna `efectosGenerados;`

#### 7. `registrarEvidenciaYDescartarSospechosos`
- std::vector<int> indicesRecienTachados;
- Para cada sospechoso `i` en `listaDeCincoSospechosos`:
  - Si ya estaba tachado, continua.
  - Comprueba si `codigoDeEvidencia` esta en sus 3 rasgos.
  - Si NO esta en sus rasgos: `sospechososTachados[i] = true; indicesRecienTachados.push_back(i);`
- Retorna `indicesRecienTachados;`

#### 8. `estaTrampaListaParaSerArmada`
- Retorna `pasoActualDeUbicacion >= 3;`
