# ESPECIFICACION TECNICA: controlador_del_juego.h / controlador_del_juego.cpp

## Archivo de Cabecera: `controlador_del_juego.h`

### Guardas de inclusion
```cpp
#ifndef CONTROLADOR_DEL_JUEGO_H
#define CONTROLADOR_DEL_JUEGO_H
```

### Inclusiones requeridas
```cpp
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>
#include <QString>
#include <QHash>
#include <QJsonObject>

#include "juego/teclado.h"
#include "juego/dificultad.h"
#include "juego/recursos_de_la_partida.h"
#include "juego/reloj_dia_noche.h"
#include "mapa/mapa.h"
#include "mapa/cargador_de_niveles.h"
#include "personajes/scooby.h"
#include "personajes/personaje_no_jugable.h"
#include "personajes/objeto_del_escenario.h"
#include "personajes/trampa.h"
#include "investigacion/caso.h"
#include "investigacion/acusacion.h"
#include "inteligencia/avisos_entre_agentes.h"
#include "herramientas/azar_con_semilla.h"
#include "guardado/perfil.h"
#include "guardado/gestor_de_guardado.h"
```

### Enumeraciones de Estados y Eventos Visuales
```cpp
enum class EtapaDelJuego {
    MenuPrincipal,
    TutorialNivelUno,
    ExploracionNivelDos,
    ClimaxTrampaArmada,
    Pausa,
    Derrota,
    Victoria
};

enum class TipoDeEventoVisual {
    ObjetoRecogido,
    PistaDescubierta,
    MisionCompletada,
    ScoobyRalentizado,
    AlertaGlobalActivada,
    CanonDisparado,
    CanonImpactoCercano,
    TrampaArmada,
    CambioDeMomentoDelDia,
    AcusacionAcertada,
    AcusacionFallida,
    ScoobyCapturado,
    VillanoAtrapado
};
```

### Declaracion de Clase `ControladorDelJuego`
```cpp
class ControladorDelJuego : public QObject {
    Q_OBJECT

private:
    EtapaDelJuego etapaActual;
    EtapaDelJuego etapaAntesDeLaPausa;
    QTimer* relojDeCuadros;
    QElapsedTimer cronometroDeCuadros;
    EstadoDeTeclado ultimoEstadoDeTeclado;

    // Punteros dueños de la logica
    Mapa* mapa;
    Scooby* scooby;
    std::vector<PersonajeNoJugable*> personajes;
    std::vector<ObjetoDelEscenario*> objetos;
    Trampa* trampa;
    Caso* casoActivo;
    Acusacion* acusacion;
    RelojDiaNoche* relojDiaNoche;
    RecursosDeLaPartida* recursos;

    // Instancias directas
    BuzonCompartidoDeAvisos buzonDeAvisos;
    AzarConSemilla azar;
    ParametrosDeDificultad parametros;
    PerfilDeUsuario perfil;
    GestorDeGuardado gestorDeGuardado;
    DatosInicialesDelNivel datosDelNivel;
    QHash<QString, QJsonObject> dialogosPorPersonaje;
    int derrotasSeguidas;
    double segundosParaElProximoComodin;
    double segundosParaElProximoDisparoDeCanon;

    void liberarMemoriaDeNivel();
    void cargarNivel(int numeroDeNivel);
    void resolverColisiones();
    void resolverInteracciones();
    void aplicarEfectoDeMision(const EfectoDeMisionEnElMundo& efecto);
    void generarComodinCercanoAScooby();
    void dispararCanonSiCorresponde(double segundosTranscurridos);
    void iniciarClimax();
    void pasarDelNivelUnoAlNivelDos();
    RectanguloVision armarVistaDeScooby() const;
    void evaluarReglasDeFinDePartida();
    void terminarPartida(bool fueVictoria, const QString& motivo);
    void cambiarEtapa(EtapaDelJuego nuevaEtapa);

private slots:
    void ejecutarBuclePrincipal();

public:
    explicit ControladorDelJuego(QObject* padre = nullptr);
    ~ControladorDelJuego() override;

    void iniciarPartida(int numeroDeNivel, NivelDeDificultad dificultad, unsigned int semillaManual = 0);
    void procesarEntradaTeclado(const EstadoDeTeclado& teclado);
    void pausarPartida();
    void reanudarPartida();
    void acusarASospechoso(int indiceDelSospechoso);
    void responderAcertijo(int indiceDeLaOpcion);
    void gastarGemaEnAcertijo();
    void cerrarConversacion();
    void guardarPerfilAlSalir();

    // Consultas const para la interfaz
    const Mapa* obtenerMapa() const;
    const Scooby* obtenerScooby() const;
    const std::vector<PersonajeNoJugable*>& obtenerPersonajes() const;
    const std::vector<ObjetoDelEscenario*>& obtenerObjetos() const;
    const Trampa* obtenerTrampa() const;
    const Caso* obtenerCaso() const;
    const RelojDiaNoche* obtenerReloj() const;
    const RecursosDeLaPartida* obtenerRecursos() const;
    const PerfilDeUsuario& obtenerPerfil() const;
    EtapaDelJuego obtenerEtapaActual() const;

signals:
    void alCambiarEtapa(EtapaDelJuego etapaNueva);
    void alActualizarRecursos();
    void alOcurrirEventoVisual(TipoDeEventoVisual evento, int numeroDeSerie);
    void alCambiarMomentoDelDia(bool esDeNoche);
    void alIniciarConversacion(QString nombre, QString claveDeRetrato, QString mensaje);
    void alIniciarAcertijo(int codigoDeLaMision);
    void alAnotarEnLibreta(QString texto, bool esRumorSinConfirmar);
    void alTacharSospechoso(int indiceDelSospechoso);
    void alTerminarPartida(bool fueVictoria, QString motivo);
};

#endif
```

---

## Archivo de Implementacion: `controlador_del_juego.cpp`

### Inclusiones requeridas
```cpp
#include "juego/controlador_del_juego.h"
#include "inteligencia/cerebro_ciudadano_y_amigo.h"
#include "inteligencia/cerebro_secuaz.h"
#include "inteligencia/cerebro_villano.h"
#include "fisicas/movimiento_circular.h"
#include "fisicas/pendulo_amortiguado.h"
#include "fisicas/tiro_parabolico.h"
#include "herramientas/calculos_espaciales.h"
```

### Implementacion de Metodos

#### 1. Constructor
- Inicializa punteros en `nullptr`, `etapaActual = EtapaDelJuego::MenuPrincipal`, `derrotasSeguidas = 0`.
- Crea temporizador de cuadros:
  - `relojDeCuadros = new QTimer(this);`
  - `connect(relojDeCuadros, &QTimer::timeout, this, &ControladorDelJuego::ejecutarBuclePrincipal);`
- Carga perfil con `gestorDeGuardado.cargarPerfilDesdeDisco(perfil);`

#### 2. Destructor
- Detiene `relojDeCuadros`.
- Llama a `liberarMemoriaDeNivel()`.

#### 3. `liberarMemoriaDeNivel`
- Libera con `delete`: `mapa`, `scooby`, `trampa`, `casoActivo`, `acusacion`, `relojDiaNoche`, `recursos`.
- Libera vectores:
  - `for (auto p : personajes) delete p; personajes.clear();`
  - `for (auto o : objetos) delete o; objetos.clear();`

#### 4. `iniciarPartida`
- Llama a `liberarMemoriaDeNivel()`.
- Inicializa semilla en `azar` (manual si `semillaManual != 0`, sino del sistema).
- Obtiene parametros de `Dificultad::obtenerParametros(dificultad)`.
- Aplica asistencia por derrotas `Dificultad::aplicarAsistenciaPorDerrotas(parametros, derrotasSeguidas)`.
- Instancia `mapa`, `relojDiaNoche`, `recursos`, `acusacion`, `casoActivo`.
- Llama a `cargarNivel(numeroDeNivel)`.
- Inicia reloj a 16 ms: `cronometroDeCuadros.start(); relojDeCuadros->start(16);`
- Cambia etapa a `TutorialNivelUno` si nivel 1, o `ExploracionNivelDos` si nivel 2.

#### 5. `ejecutarBuclePrincipal`
- Mide tiempo real con `cronometroDeCuadros.restart() / 1000.0`.
- Acota tiempo a maximo 0.05 s: `double dt = CalculosEspaciales::mantenerValorDentroDelRango(segundos, 0.0, 0.05);`
- Si la etapa es `MenuPrincipal, Pausa, Derrota, Victoria`: retorna sin calcular logica.
- 1. Mueve a Scooby con `scooby->moverSegunTeclado(ultimoEstadoDeTeclado, dt, *mapa);`
- 2. Avanza reloj: `bool cambioDiaNoche = relojDiaNoche->avanzarTiempo(dt);` Si cambio, emite `alCambiarMomentoDelDia(relojDiaNoche->esDeNoche())`.
- 3. En Nivel 2: avanza consumo de provisiones en `recursos` (aplica robo si es de noche y villano activo).
- 4. Descuenta vigencia de avisos en `buzonDeAvisos.descontarTiempoDeVigencia(dt);`
- 5. Para cada personaje activo: llama a `pensarYActuar`, luego `avanzarUnCuadro`.
- 6. Para cada objeto activo: llama a `avanzarUnCuadro`.
- 7. Resuelve colisiones: `resolverColisiones();`
- 8. Resuelve interacciones: `resolverInteracciones();`
- 9. Evalua misiones:
  - `auto efectos = casoActivo->evaluarProgresoDeMisionesEnCuadro(situacion);`
  - Ejecuta cada efecto devuelto con `aplicarEfectoDeMision`.
- 10. Si Nivel 1 y misiones completadas: llama a `pasarDelNivelUnoAlNivelDos()`.
- 11. Generador de comodines (cada 40 a 120 s) y cañonazos nocturnos.
- 12. Revisa trampa: `trampa->revisarSiScoobyOElVillanoPisanLaZona(*scooby, *villano);`
- 13. Evalua reglas de fin: `evaluarReglasDeFinDePartida();`
- 14. Limpia `ultimoEstadoDeTeclado.limpiarPulsacionReciente();`

#### 6. `evaluarReglasDeFinDePartida`
- Derrota si:
  - `relojDiaNoche->seAcaboElTiempo()` -> motivo: "Se agoto el tiempo de la investigacion."
  - `recursos->estanProvisionesAgotadas()` -> motivo: "Las provisiones del castillo se han agotado."
  - `trampa->obtenerScoobyCayoEnLaTrampa()` -> motivo: "Scooby cayo en su propia trampa."
  - Toque letal de villano o secuaz en modo alerta -> motivo: "Scooby fue capturado por los guardias del asedio."
- Victoria si:
  - `trampa->obtenerVillanoCayoEnLaTrampa()` -> motivo: "¡El fantasma ha caido en la trampa!"

#### 7. `terminarPartida`
- Detiene temporizadores.
- Si fue victoria:
  - `derrotasSeguidas = 0;`
  - `perfil.casosResueltosTotales++;`
  - Marca villano como desenmascarado.
  - Actualiza mejor tiempo.
- Si fue derrota:
  - `derrotasSeguidas++;`
- `gestorDeGuardado.guardarPerfilEnDisco(perfil);`
- Cambia etapa a Victoria o Derrota y emite `alTerminarPartida(fueVictoria, motivo)`.
