# Chronos
Pequeña aplicación con cronómetro, temporizador y alarma.

Es un proyecto en C que me he propuesto con fines académicos para aprender. Quería ponerme a prueba y ver que podía lograr con lo que he aprendido hasta ahora. He abordado las mismas tareas de distintas formas, así que el código es algo repetitivo y no está optimizado. El proyecto empezó como algo un poco menos ambicioso, pero según iba logrando mis objetivos, se me ocurrían detalles que podía añadir y que quería probar. Así que cuando me di cuenta que no estaría mal incluir anotaciones, ya era demasiado tarde. Pero bueno ya lo sé para la próxima. Aunque sea un proyecto pequeño, haré anotaciones. No es gran cosa, una aplicación de terminal, pero al haberlo logrado es algo de lo que me siento orgulloso.

Importante: Para sistemas Windows.

Para compilar yo he usado MSYS2 y Dev-C++. MSYS2 para compilar el archivo de recursos myresource.rc y luego compilar el código con Dev-C++. No soy ningún experto así que daré los detalles de como lo he compilado yo por si alguien desea seguir mis pasos directamente.

En el terminal de MSYS2, navegamos hasta el directorio donde se encuentre el archivo de recursos "myresource.rc" y los archivos a los que hace referencia e introducimos:

windres -i myresource.rc -o myresource.o --target=pe-x86-64

El archivo necesitamos compilar en 64 bits.

Luego en Dev-C++ abrimos un nuevo proyecto, añadimos el código en C, añadimos el archivo myresource.o y es importante que el archivo resource.h esté en el mismo directorio que el código. Vamos a las opciones del compilador y en los comandos del linker ponemos:

-lwinmm myresource.o

Luego en opciones del entorno deseleccionamos "Pause console programs after return" compilamos y listo. Así además de ver el código que sin anotaciones puede ser un poco lioso y resultar algo tedioso, también podrán probar como funciona y darme consejos de cara al futuro. Gracias a todos por vuestro tiempo.
