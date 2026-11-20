# ESPECIFICACION TECNICA: gestor_de_guardado.h / gestor_de_guardado.cpp

## Archivo de Cabecera: `gestor_de_guardado.h`

### Guardas de inclusion
```cpp
#ifndef GESTOR_DE_GUARDADO_H
#define GESTOR_DE_GUARDADO_H
```

### Inclusiones requeridas
```cpp
#include <QString>
#include "guardado/perfil.h"
```

### Declaracion de Clase `GestorDeGuardado`
```cpp
class GestorDeGuardado {
private:
    QString rutaDelArchivoPrincipalDePerfiles;
    QString rutaDelArchivoDeRespaldo;

public:
    GestorDeGuardado();

    void inicializarRutasEnElDirectorioDelSistema();
    void cargarPerfilDesdeDisco(PerfilDeUsuario& perfilDestino);
    void guardarPerfilEnDisco(const PerfilDeUsuario& perfilActual);
    void recuperarPerfilDesdeElRespaldo(PerfilDeUsuario& perfilDestino);

    QString obtenerRutaArchivoPrincipal() const;
    QString obtenerRutaArchivoRespaldo() const;
};

#endif
```

---

## Archivo de Implementacion: `gestor_de_guardado.cpp`

### Inclusiones requeridas
```cpp
#include "guardado/gestor_de_guardado.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <stdexcept>
```

### Implementacion de Metodos

#### 1. Constructor
- Llama a `inicializarRutasEnElDirectorioDelSistema()`.

#### 2. `inicializarRutasEnElDirectorioDelSistema`
- Obtiene la carpeta segura:
  - `QString directorioSeguro = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);`
  - `QDir dir(directorioSeguro); if (!dir.exists()) dir.mkpath(".");`
  - `rutaDelArchivoPrincipalDePerfiles = directorioSeguro + "/perfiles.txt";`
  - `rutaDelArchivoDeRespaldo = directorioSeguro + "/perfiles.txt.bak";`

#### 3. `cargarPerfilDesdeDisco`
- Si no existe `rutaDelArchivoPrincipalDePerfiles`:
  - `perfilDestino = PerfilDeUsuario{};`
  - Retorna.
- Intenta leer archivo en bloque `try`:
  - `QFile archivo(rutaDelArchivoPrincipalDePerfiles);`
  - Si no abre, lanza `std::runtime_error("No se pudo abrir perfiles.txt");`
  - Lee linea por linea parseando `CLAVE=VALOR`:
    - `NOMBRE`: `perfilDestino.nombreDelDetective = valor;`
    - `NIVEL1_COMPLETADO`: `perfilDestino.nivelUnoTutorialSuperado = (valor.toInt() == 1);`
    - `DIFICULTAD`: `perfilDestino.ultimaDificultadSeleccionada = static_cast<NivelDeDificultad>(valor.toInt());`
    - `MEJOR_TIEMPO`: `perfilDestino.mejorTiempoEnSegundos = valor.toInt();`
    - `CASOS_RESUELTOS`: `perfilDestino.casosResueltosTotales = valor.toInt();`
    - `VILLANOS`: parsea 5 booleanos separados por comas.
    - `ULTIMAS_MISIONES`: parsea enteros separados por comas.
    - `VOLUMEN`: `perfilDestino.volumenGuardado = valor.toDouble();`
- En bloque `catch (const std::exception&)`:
  - Llama a `recuperarPerfilDesdeElRespaldo(perfilDestino);`

#### 4. `guardarPerfilEnDisco`
- Copia respaldo previo:
  - Si existe `rutaDelArchivoPrincipalDePerfiles`:
    - `QFile::remove(rutaDelArchivoDeRespaldo);`
    - `QFile::copy(rutaDelArchivoPrincipalDePerfiles, rutaDelArchivoDeRespaldo);`
- Abre archivo principal en modo escritura:
  - Escribe lineas en formato clave=valor ordenado.
  - Cierra archivo.

#### 5. `recuperarPerfilDesdeElRespaldo`
- Si existe `rutaDelArchivoDeRespaldo`:
  - Intenta cargar perfil desde respaldo.
- Si falla o no existe:
  - `perfilDestino = PerfilDeUsuario{};` (crea perfil nuevo limpio).
