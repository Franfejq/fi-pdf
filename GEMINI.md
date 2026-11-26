# Reglas del proyecto

- **Idioma:** Responde, explica tus razonamientos, documenta funciones y redacta mensajes de commit siempre en **español técnico**, manteniendo en inglés únicamente los identificadores de código y términos estándar de C/CMake.
- **Estándar y compilación:** El proyecto está escrito en **C17** y se compila con **CMake**. No sugieras características de C++ ni dependencias externas no declaradas en `CMakeLists.txt`.
- **Control de memoria y errores:** Toda reserva dinámica debe tener su liberación explícita verificada y toda llamada al sistema o librería externa debe validar su código de retorno.
- **Estilo de trabajo:** Antes de proponer cambios grandes, explica brevemente qué archivos vas a tocar y por qué.
