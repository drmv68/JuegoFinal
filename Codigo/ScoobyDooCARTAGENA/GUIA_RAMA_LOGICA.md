# GUIA DE LA RAMA LOGICA

Guia completa para escribir todo el codigo de la carpeta `logica/`.
Cada seccion corresponde a un modulo y detalla sus clases, metodos, atributos,
constantes, flujos y reglas. Los nombres ya estan aprobados: usar exactamente
estos nombres en el codigo.

---

## Reglas generales de escritura

- **Cero comentarios.** El nombre de cada clase, metodo y variable debe bastar
  para entender que hace. Si un nombre pide comentario, el nombre esta mal.
- **Español de Colombia, completo, sin siglas ni abreviaturas.** Solo se
  conservan en ingles las palabras propias de C++ y de Qt (`class`, `virtual`,
  `QTimer`, `QPainter`, `emit`).
- **Convenciones de nombres:**
  - Archivo: `cerebro_villano.h` / `.cpp`
  - Clase: `CerebroVillano` (misma idea que el archivo, con mayuscula inicial
    en cada palabra).
  - Clases madre abstractas: llevan el sufijo `Base` (`CerebroBase` vive en
    `cerebro.h`).
  - Metodos y atributos: `camelCase` en español (`avanzarUnCuadro`,
    `posicionX`, `segundosRestantesTotales`).
  - Constantes: `constexpr` con nombre descriptivo al inicio del `.h`
    (`velocidadDeScoobyEnCasillasPorSegundo`).
- **Unidades de medida:**
  - Tiempo: segundos con decimales (`double`).
  - Posiciones y distancias: casillas con decimales (`double`). La posicion 3.5
    es el centro de la casilla 3.
  - Velocidades: casillas por segundo.
  - Angulos: radianes.
- **Cada cuadro del juego** recibe `segundosTranscurridos`, el tiempo desde el
  cuadro anterior, limitado a un maximo de 0.05 s.
- **Numeros fijos** van como `constexpr` con nombre. Los que cambian con la
  dificultad van en `ParametrosDeDificultad`. **Cero numeros sueltos** dentro
  de los metodos.
- **Memoria dinamica:** quien crea un objeto con `new` es su dueño y lo libera
  con `delete` en su destructor.
- **Separacion estricta:** `logica/` nunca incluye nada de dibujo (`QPainter`,
  `QWidget`, `QGraphicsView`).

---

## Orden de construccion (ramas)

1. `logica-herramientas` → azar_con_semilla, calculos_espaciales
2. `logica-mapa` → mapa, cargador_de_niveles
3. `logica-fisicas` → movimiento, pendulo_amortiguado, movimiento_circular,
   tiro_parabolico
4. `logica-elementos` → elemento_del_mapa, scooby, objeto_del_escenario,
   trampa, personaje_no_jugable
5. `logica-inteligencia` → memoria_del_agente, avisos_entre_agentes, cerebro,
   cerebro_ciudadano_y_amigo, cerebro_secuaz, cerebro_villano
6. `logica-investigacion` → mision, mision_hablar, mision_traer,
   mision_examinar, mision_pendulo, caso, acusacion
7. `logica-guardado` → perfil, gestor_de_guardado
8. `logica-juego` → recursos_de_la_partida, reloj_dia_noche,
   controlador_del_juego

Cada rama se une a `principal` antes de empezar la siguiente.

---

## Modulo herramientas/

### azar_con_semilla.h / .cpp
- **Clase:** `AzarConSemilla`
- **Atributos:** `std::mt19937 motorDeNumerosAleatorios`,
  `unsigned int numeroDeSemillaActiva`
- **Metodos:**
  - `inicializarConSemillaDelSistema()`
  - `inicializarConSemillaFija(unsigned int semillaManual)`
  - `obtenerNumeroDeSemillaActiva() const`
  - `generarEnteroEnRango(int minimoIncluido, int maximoIncluido)`
  - `generarDecimalEnRango(double minimoIncluido, double maximoIncluido)`
  - `evaluarProbabilidadPorcentual(double porcentajeDeExito)`
  - `barajarListaDeCasillas(std::vector<CoordenadaCasilla>& listaDeCasillas)`
- **Regla:** una sola instancia en todo el juego, creada por el controlador y
  pasada por referencia. Para ubicar algo en una casilla libre se baraja la
  lista y se toman las primeras; nunca `while` hasta acertar.

### calculos_espaciales.h / .cpp
- **Namespace:** `CalculosEspaciales` (funciones libres, sin atributos)
- **Struct:** `RectanguloVision { double izquierda; double arriba;
  double derecha; double abajo; }`
- **Funciones:**
  - `mantenerValorDentroDelRango(double valorActual,
    double valorMinimoPermitido, double valorMaximoPermitido)`
  - `obtenerValorSinSigno(double valor)`
  - `calcularDistanciaAlCuadrado(double origenX, double origenY,
    double destinoX, double destinoY)`
  - `calcularDistanciaEnUnEje(double coordenadaA, double coordenadaB)`
  - `convertirPosicionACasilla(double posicionX, double posicionY)`
  - `calcularAnguloEntrePuntosEnRadianes(double origenX, double origenY,
    double destinoX, double destinoY)`
  - `estaPuntoDentroDelConoDeVision(double origenX, double origenY,
    DireccionMirada direccionMirada, double objetivoX, double objetivoY,
    double aperturaDelConoEnRadianes)`
  - `estaPuntoDentroDelRectangulo(double puntoX, double puntoY,
    const RectanguloVision& rectangulo)`
- **Regla:** funciones cortas definidas `inline` dentro del `.h`. Todo se
  calcula sobre la cuadricula plana, nunca sobre la proyeccion isometrica.
  Comparar `distanciaAlCuadrado <= rango * rango` para evitar raices cuadradas.

---

## Modulo mapa/

### mapa.h / .cpp
- **Structs:**
  - `CoordenadaCasilla { int columna; int fila; }`
  - `enum class TipoDeTerreno { SueloTransitable, CaminoDeAdoquin,
    ParedSolida, AguaMuralla }`
  - `CasillaDelMapa` con `TipoDeTerreno terreno`, `int numeroDeZona`,
    banderas: `esTransitable`, `esZonaRestringida`, `tienePasoBloqueado`,
    `esArbustoParaOcultarse`, `esAptaParaTrampa`, `esAlcanzableDesdeElInicio`
  - `SectorDelMapa { QString nombre; CoordenadaCasilla esquinaA, esquinaB; }`
  - `ReferenciaDelMapa { QString nombre; CoordenadaCasilla casilla; }`
- **Clase:** `Mapa`
- **Atributos:** `int anchoEnCasillas`, `int altoEnCasillas`,
  `std::vector<CasillaDelMapa> cuadriculaLineal`,
  `std::vector<CoordenadaCasilla> listaDeArbustos`,
  `std::vector<CoordenadaCasilla> casillasAptasParaTrampa`,
  `std::vector<SectorDelMapa> sectores`,
  `std::vector<ReferenciaDelMapa> referencias`
- **Acceso a cuadricula:** `fila * anchoEnCasillas + columna`
- **Metodos:**
  - `inicializarDimensiones(int ancho, int alto)`
  - `obtenerCasilla(int columna, int fila) const`
  - `modificarCasilla(int columna, int fila)`
  - `esPosicionTransitable(double posicionX, double posicionY,
    bool puedeEntrarARestringidas) const`
  - `tieneLineaDeVista(CoordenadaCasilla origen,
    CoordenadaCasilla destino) const` — algoritmo de Bresenham
  - `calcularSiguientePasoHacia(CoordenadaCasilla origen,
    CoordenadaCasilla destino, bool puedeEntrarARestringidas) const` —
    A* limitado a 300 casillas exploradas
  - `desbloquearZonaCompleta(int numeroDeZona)`
  - `cambiarBloqueoDeCasilla(int columna, int fila, bool bloquearPaso)`
  - `obtenerSectorEnCasilla(CoordenadaCasilla casilla) const`
  - `marcarCasillasAlcanzablesDesde(CoordenadaCasilla casillaDeInicio)`
- **Regla:** el movimiento se valida primero en X y luego en Y para que el
  personaje se deslice al rozar una pared. Linea de vista cada 0.15 s.

### cargador_de_niveles.h / .cpp
- **Struct:** `DatosInicialesDelNivel` (casilla de Scooby, amigos con rol y
  casilla, ciudadanos con clave de dialogo, secuaces con tipo de patrulla y
  puntos de control, centro y radio del centinela, casilla del almacen,
  casilla de la lampara, casillas de los canones)
- **Clase:** `CargadorDeNiveles`
- **Metodos:**
  - `cargarNivelDesdeArchivos(const QString& rutaTerreno,
    const QString& rutaDetalles, Mapa& mapaDestino,
    DatosInicialesDelNivel& datosSalida)`
  - `leerBancoDeDatos(const QString& rutaDelBanco)` — devuelve `QJsonObject`
  - Privados: `procesarArchivoTerreno(...)`,
    `procesarArchivoDetalles(...)`,
    `comprobarBordesCerradosYDimensiones(...)`,
    `comprobarQueNadieNazcaSobreObstaculos(...)`
- **Formato terreno.txt:** `#` pared, `.` suelo, `+` adoquin, `R` restringido,
  `A` arbusto, `B` muralla/agua
- **Formato detalles.txt:** una orden por linea (`SCOOBY col fila`,
  `AMIGO VELMA col fila`, `SECUAZ RUTA_FIJA c1 f1 c2 f2...`, etc.)
- **Excepciones:** lanza `std::runtime_error` con archivo y numero de linea
  ante cualquier error.

---

## Modulo fisicas/

### movimiento.h (solo .h)
- **Struct:** `CoordenadaFisicaTresEjes { double x; double y;
  double alturaZ; }`
- **Clase madre abstracta:** `MovimientoBase`
- **Atributos:** `double tiempoAcumuladoEnSegundos`,
  `bool estaSimulacionActiva`
- **Metodos:** destructor virtual, `reiniciarSimulacion() = 0`,
  `avanzarYCalcularPosicion(double segundosDesdeElCuadroAnterior) = 0`,
  `estaEnMomentoClave() const` (por defecto false)

### pendulo_amortiguado.h / .cpp
- **Clase:** `PenduloAmortiguado : public MovimientoBase`
- **Atributos:** `puntoAnclajeX`, `puntoAnclajeY`,
  `longitudCuerdaEnCasillas`, `amplitudMaximaEnRadianes`,
  `frecuenciaOscilacionEnHertz`, `factorDeAmortiguamiento`,
  `anguloActualEnRadianes`
- **Ecuacion:** `angulo(t) = amplitud * exp(-amortiguamiento * t) *
  cos(2 * PI * frecuencia * t)`
- **Momento clave:** angulo sin signo supera 90% de la amplitud restante
- **Reinicio:** cuando amplitud restante cae bajo 40% de la inicial

### movimiento_circular.h / .cpp
- **Clase:** `MovimientoCircular : public MovimientoBase`
- **Atributos:** `centroDeGiroX`, `centroDeGiroY`,
  `radioActualEnCasillas`, `velocidadAngularEnRadianesPorSegundo`,
  `faseInicialEnRadianes`, `sentidoDeGiro` (1 o -1),
  `segundosParaElProximoCambio`, `segundosRestantesDePausa`,
  `estaEnPausaDeCambio`
- **Ecuaciones:** `x = centroX + radio * cos(fase + sentido * vel * t)`,
  `y = centroY + radio * sin(fase + sentido * vel * t)`
- **Variacion:** cada 6-12 s (sorteados) se detiene 0.8 s, luego
  `variarUnSoloParametroDePatrulla` cambia UNA variable

### tiro_parabolico.h / .cpp
- **Clase:** `TiroParabolico : public MovimientoBase`
- **Atributos:** `posicionDeDisparoX`, `posicionDeDisparoY`,
  `alturaInicialDelCanonZ`, `velocidadInicialDeDisparo`,
  `anguloDeElevacionEnRadianes`, `anguloDeDireccionEnMapaEnRadianes`,
  `gravedadSimulada`, `tiempoTotalDeVueloCalculado`,
  `puntoDeImpactoX`, `puntoDeImpactoY`, `yaImpactoContraElSuelo`
- **Metodos:** `configurarDisparoDesdeMuralla(...)`,
  `configurarDisparoHaciaUnPunto(...)`, `precalcularTiempoYPuntoDeImpacto()`,
  `obtenerCoordenadaDeImpactoFinal() const`,
  `obtenerProgresoDeVueloNormalizado() const`

---

## Modulo personajes/

### elemento_del_mapa.h / .cpp
- **Clase madre abstracta:** `ElementoDelMapaBase`
- **Atributos:** `static int ultimoNumeroDeSerieAsignado`,
  `int numeroDeSerie`, `double posicionX`, `double posicionY`,
  `double anchoDeColision`, `double altoDeColision`,
  `DireccionMirada direccionMirada`, `bool estaActivo`, `bool esVisible`
- **Enum en este .h:** `enum class DireccionMirada { Norte, Sur, Este, Oeste }`
- **Metodos:** destructor virtual,
  `avanzarUnCuadro(double segundosTranscurridos) = 0`,
  `obtenerCasillaActual() const`,
  `colisionaCon(const ElementoDelMapaBase& otroElemento) const`,
  `calcularOrdenDeDibujo() const` — devuelve `posicionX + posicionY`

### scooby.h / .cpp
- **Clase:** `Scooby : public ElementoDelMapaBase`
- **Constantes `constexpr`:**
  - `velocidadDeScoobyEnCasillasPorSegundo = 3.3`
  - `alientoMaximoEnSegundos = 3.0`
  - `multiplicadorDeSprint = 1.6`
  - `segundosDeSprintInfinito = 20.0`
  - `factorDeRalentizacion = 0.67`
  - `segundosParaOlvidarToques = 20.0`
  - `segundosDeInmunidadTrasToque = 1.5`
  - `segundosMaximosDeCamuflajeEnArbusto = 4.5`
  - `opacidadOculto = 0.45`
  - `cantidadMaximaDeProvisionesCargadas = 2`
- **Enum resultado:** `ConsecuenciaDelToque { SoloRalentizado,
  ActivarAlertaGlobal, Captura }`
- **Metodos principales:**
  - `moverSegunTeclado(const EstadoDeTeclado& teclado,
    double segundosTranscurridos, const Mapa& mapa)` — primero X luego Y
  - `actualizarTemporizadoresDeEstado(double segundosTranscurridos)`
  - `recibirToqueDeSecuaz(bool partidaEnPanico)` — devuelve ConsecuenciaDelToque
  - `aplicarEmpujeDeFantasma(double origenX, double origenY)`
  - `activarEsconditeDeDaphne(double segundos)`
  - `obtenerOpacidadVisual() const`
  - `estaVisibleParaLosEnemigos() const`

### objeto_del_escenario.h / .cpp
- **Enum en este .h:** `enum class CategoriaDeObjeto { PistaDeLugar,
  EvidenciaDeSospechoso, MarcaEnSuelo, ProvisionDeSuministro,
  FrutaSprintInfinito, GemaDeIntercambio, RelojTiempoExtra,
  LamparaOscilante, BalaDeCanon }`
- **Clase:** `ObjetoDelEscenario : public ElementoDelMapaBase`
- **Constantes:** `segundosDeVidaDelRelojExtra = 6.5`,
  `segundosDeVidaDeLaFruta = 12.0`, `segundosDeVidaDeLaProvision = 20.0`,
  `distanciaParaRevelarMarcaEnCasillas = 2.0`
- **Puntero dueño:** `MovimientoBase* movimientoMatematico`

### trampa.h / .cpp
- **Enum:** `EstadoDeLaTrampa { OcultaSinArmar, ArmadaInvisible,
  DisparadaConCaptura }`
- **Clase:** `Trampa : public ElementoDelMapaBase`
- **Constantes:** `mitadDelLadoDeLaZonaEnCasillas = 2.5`,
  `segundosNecesariosParaArmar = 3.5`
- **Metodos:**
  - `armarSiScoobyMantieneLaInteraccion(...)` — devuelve true al armar
  - `revisarSiScoobyOElVillanoPisanLaZona(...)` — usa
    `calcularDistanciaEnUnEje` en X y en Y

### personaje_no_jugable.h / .cpp
- **Enums en este .h:**
  - `TipoRolDelPersonaje { CiudadanoInformativo, AmigoVelma, AmigoFred,
    AmigoShaggy, AmigoDaphne, SecuazVigilante, CentinelaAlmacen,
    VillanoFantasma }`
  - `IconoSobreLaCabeza { Ninguno, SignoDeInterrogacion,
    SignoDeExclamacion, AuraDeFuria }`
  - `ServicioDeAmigo { Ninguno, SoloConversacion,
    CambiarGemaPorProvisiones, RecibirEvidenciasDeVelma, DarIndicioSutil,
    OcultarAScooby }`
- **Clase:** `PersonajeNoJugable : public ElementoDelMapaBase`
- **Punteros dueño:** `CerebroBase* cerebro`,
  `MovimientoBase* movimientoDeRecorrido`

---

## Modulo inteligencia/

### memoria_del_agente.h (solo .h)
- **Struct:** `MemoriaDelAgente` con:
  - Patrulla: `listaDePuntosDeRutaFija`, `indiceDelPuntoObjetivoActual`,
    `usaPatrullaLibreAleatoria`, `destinoAleatorioActual`
  - Rastreo: `recuerdaUltimaPosicionDeScooby`,
    `ultimaCasillaDondeVioAScooby`, `ultimoArbustoDondeSeEscondio`,
    `vecesEscondidoEnElMismoArbusto` (captura al llegar a 3)
  - Villano: `contadorDeAvistamientosEnLaZonaActual`,
    `segundosParaOlvidarLaSospecha`
  - Amigos: `yaEntregoLaPistaPrincipal`,
    `segundosDeEnfriamientoDelServicio`

### avisos_entre_agentes.h (solo .h)
- **Struct:** `AvisoDeVigilancia { int numeroDeSerieDelEmisor;
  CoordenadaCasilla casillaReportada;
  double segundosRestantesDeVigencia;
  bool esOrdenDirectaDelVillano; }`
- **Struct:** `BuzonCompartidoDeAvisos` con
  `AvisoDeVigilancia listaDeAvisosActivos[16]`,
  `int cantidadDeAvisosActivos`,
  `double segundosDeAlertaGlobalRestantes`
- **Metodos inline:** `publicarNuevoAviso(...)`,
  `descontarTiempoDeVigencia(double segundosTranscurridos)`,
  `buscarAvisoCercanoParaElAgente(...)`,
  `activarAlertaGlobal(double duracionEnSegundos)`
- Arreglo fijo de 16: nunca pide `new` dentro del bucle.

### cerebro.h / .cpp
- **Enums:**
  - `EstadoMentalDelAgente { InactivoDurmiendo, PatrullandoSector,
    PausaDeVigilancia, SospechandoConDuda, PersiguiendoObjetivo,
    BuscandoEnUltimaPosicion, HuyendoEnPanico, PreparandoFuria,
    FuriaDeContraataque }`
  - `EstimuloPercibido { veAScooby, scoobyEstaEnZonaRestringida,
    distanciaAlCuadradoHaciaScooby, casillaDondeLoVio,
    recibioAvisoDeAlerta, casillaDelAvisoRecibido }`
- **Clase madre abstracta:** `CerebroBase`
- **Atributos:** `estadoMentalActual`, `MemoriaDelAgente memoriaPropia`,
  `intervaloEntrePensamientos = 0.15`,
  `rangoDeVisionEnCasillas`, `velocidadNormalDePatrulla = 2.4` (80% de 3.0),
  `velocidadDePersecucion = 3.0`
- **Ciclo:** percibir → razonar → actuar → aprender (cada 0.15 s)
- **Embudo de percepcion:** distancia al cuadrado → cono de vision →
  linea de vista (de lo mas barato a lo mas caro)
- Cada personaje nace con un desfase en su acumulador para que no piensen
  todos en el mismo instante.

### cerebro_ciudadano_y_amigo.h / .cpp
- `CerebroCiudadanoYAmigo : public CerebroBase`
- Omite linea de vista y busqueda de caminos.
- Servicios: Fred (25 provisiones por gema, 45 s espera), Velma (recibe
  evidencias), Shaggy (indicio sutil), Daphne (oculta 6 s, 60 s espera).

### cerebro_secuaz.h / .cpp
- `CerebroSecuaz : public CerebroBase`
- Sospecha 0.4 s antes de perseguir.
- Patrulla al 80%, persecucion al 100%.
- Pausas de 1.0-1.5 s en esquinas.
- Tras perder a Scooby: va a ultima casilla, revisa adyacentes, vuelve a
  patrulla.
- Arbusto: 3 escondidas = captura.

### cerebro_villano.h / .cpp
- `CerebroVillano : public CerebroBase`
- De dia: `InactivoDurmiendo`. De noche: aparece con sabana.
- Bloquea pasillos secundarios segun avistamientos.
- Climax: `HuyendoEnPanico` → pausas de furia → contraataque → panico.
- `alternarEntrePanicoYFuria(double segundosTranscurridos)`

---

## Modulo investigacion/

### mision.h / .cpp
- **Enums:**
  - `TipoDeEfectoDeMision { AbrirPuerta, CerrarPuerta,
    CrearObjetoEnElMapa, MostrarMarcaEnSuelo, CambiarDialogoDePersonaje,
    EntregarProvisiones, EntregarGemas, AlertarZona, TacharSospechoso,
    EscribirEnLibreta }`
  - `EstadoDeLaMision { EnEsperaPorPasoAnterior, ActivaEnCurso,
    CompletadaConExito }`
- **Structs:**
  - `EfectoDeMisionEnElMundo` (tipo, casilla, categoria, cantidad, texto,
    clave de personaje)
  - `SituacionActualDelMundo` (referencias de solo lectura: scooby, mapa,
    teclado, personajes, objetos, esDeNoche,
    numeroDeSerieDelPersonajeInteractuado)
- **Clase madre abstracta:** `MisionBase`
- **Metodos virtuales puros:** `prepararMision(...)`,
  `evaluarCumplimientoEnCuadro(...)`

### mision_hablar.h / .cpp — conversaciones y acertijos
### mision_traer.h / .cpp — recoger y entregar objetos
### mision_examinar.h / .cpp — marcas, huellas, evidencias
### mision_pendulo.h / .cpp — lampara oscilante + fisica del pendulo

### caso.h / .cpp
- **Structs en este .h:**
  - `FichaDeSospechoso { nombre, oficio, tres codigos de rasgo,
    clave del retrato, fragmento de carta, frase al ser descubierto }`
  - `enum class TipoDeGiroSorpresa { CambioDeGuardia, NieblaDelPuerto,
    BombardeoIngles, RumorFalso }`
- **Clase:** `Caso`
- **Metodos:** `armarCasoFijoNivelUno(...)`,
  `sortearCasoAleatorioNivelDos(...)`,
  `evaluarProgresoDeMisionesEnCuadro(...)`,
  `registrarEvidenciaYDescartarSospechosos(...)`,
  `estaTrampaListaParaSerArmada() const`

### acusacion.h / .cpp
- **Enum:** `ResultadoDeAcusacion { AciertoVillanoExpuesto,
  ErrorInocenteTachado, ErrorSinOportunidadesDerrota }`
- **Clase:** `Acusacion`
- Acierto: +30 s, +1 gema, desbloquea retrato, inicia climax.
- Error: -1 oportunidad, -20% tiempo (min 20 s), tacha inocente,
  alerta 15 s. Con 0 oportunidades: derrota inmediata.

---

## Modulo guardado/

### perfil.h (solo .h)
- **Struct:** `PerfilDeUsuario`
- **Campos:** `nombreDelDetective`, `nivelUnoTutorialSuperado`,
  `ultimaDificultadSeleccionada`, `mejorTiempoEnSegundos`,
  `casosResueltosTotales`, `std::array<bool, 5> villanosDesenmascarados`,
  `std::vector<int> ultimasMisionesJugadas`, `double volumenGuardado`
- **Metodos inline:** `obtenerRangoActualDelDetective() const` (Aprendiz
  0-2, Detective 3-7, Sabueso 8-15, Leyenda 16+),
  `contarVillanosDesenmascarados() const`

### gestor_de_guardado.h / .cpp
- **Clase:** `GestorDeGuardado`
- Ruta segura: `QStandardPaths::AppDataLocation`
- Formato: clave=valor (`NOMBRE=Daniel`, `NIVEL1_COMPLETADO=1`, etc.)
- Respaldo `.bak` antes de cada escritura.
- Excepciones controladas internamente: restaura desde `.bak` o crea
  perfil limpio.
- **Nunca toca disco dentro del bucle de 60 cuadros.**

---

## Modulo juego/

### teclado.h (solo .h)
- **Struct:** `EstadoDeTeclado` con `bool arriba`, `abajo`, `izquierda`,
  `derecha`, `correrActivo`, `interactuarPresionado`,
  `interactuarRecienPulsado`
- Tab no viaja a la logica: la ventana atiende la libreta sola.

### dificultad.h / .cpp
- **Enum:** `NivelDeDificultad { Facil, Medio, Dificil, Leyenda }`
- **Struct:** `ParametrosDeDificultad` (ver todos los campos en la guia
  del documento original)
- **Clase:** `Dificultad`
- **Metodos estaticos:**
  - `obtenerParametros(NivelDeDificultad nivel)`
  - `aplicarAsistenciaPorDerrotas(ParametrosDeDificultad& parametros,
    int derrotasSeguidas)` — reduce 10% vision con 2+ derrotas seguidas

### recursos_de_la_partida.h / .cpp
- **Clase:** `RecursosDeLaPartida`
- Provisiones bajan solo en Nivel 2. De noche: `multiplicadorDeRoboNocturno`.
- Toda suma/resta se acota con `mantenerValorDentroDelRango`.
- Solo emite actualizacion cuando cambia el valor entero visible.

### reloj_dia_noche.h / .cpp
- **Enum:** `MomentoDelDia { Dia, Noche }`
- **Clase:** `RelojDiaNoche`
- `factorDeOscuridad` se interpola linealmente en los ultimos 5 s de
  cada transicion.
- Bonos y descuentos actuan sobre la cuenta regresiva.

### controlador_del_juego.h / .cpp
- **Clase:** `ControladorDelJuego : public QObject` — unico QObject de logica
- **Senales (ya acordadas):**
  - `alCambiarEtapa(EtapaDelJuego etapaNueva)`
  - `alActualizarRecursos()`
  - `alOcurrirEventoVisual(TipoDeEventoVisual evento, int numeroDeSerie)`
  - `alCambiarMomentoDelDia(bool esDeNoche)`
  - `alIniciarConversacion(QString nombre, QString claveDeRetrato,
    QString mensaje)`
  - `alIniciarAcertijo(int codigoDeLaMision)`
  - `alAnotarEnLibreta(QString texto, bool esRumorSinConfirmar)`
  - `alTacharSospechoso(int indiceDelSospechoso)`
  - `alTerminarPartida(bool fueVictoria, QString motivo)`
- **Enums en este .h:**
  - `EtapaDelJuego { MenuPrincipal, TutorialNivelUno,
    ExploracionNivelDos, ClimaxTrampaArmada, Pausa, Derrota, Victoria }`
  - `TipoDeEventoVisual { ObjetoRecogido, PistaDescubierta,
    MisionCompletada, ScoobyRalentizado, AlertaGlobalActivada,
    CanonDisparado, CanonImpactoCercano, TrampaArmada,
    CambioDeMomentoDelDia, AcusacionAcertada, AcusacionFallida,
    ScoobyCapturado, VillanoAtrapado }`
- **Flujo de cada cuadro:** medir tiempo → acotar a 0.05 s → mover Scooby →
  avanzar reloj y provisiones → pensar y mover personajes → avanzar objetos →
  resolver colisiones e interacciones → evaluar misiones → comodines y
  canones → revisar trampa → evaluar reglas de fin → limpiar pulsacion
  reciente → emitir senales.
- **Regla de derrota:** tiempo = 0, provisiones = 0, captura, trampa propia,
  canon en climax, oportunidades = 0.
- **Regla de victoria:** villano cae en la trampa.
