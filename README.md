# TPE Final | Programación Imperativa


## Descripción
Diseño e implementación un programa para el procesamiento de datos de series y películas.
Este proyecto se encuentra desarrollado principalmente para procesar información obtenida
del sitio iMDb mediante un archivo .csv indicado por el usuario.

## Generación de ejecutables
Ver archivo `Makefile`

Para generar el ejecutable, simplemente correr el comando `make all`
en la consola. El nombre del ejecutable por default será `imdb`. 
Si se desea limpiar el build, bastará con ejecutar el comando `make clean`

## Ejecución
Si se posee de un archivo .csv denominado `imdbv3.csv` en el mismo directorio que el ejecutable,
para correr el programa se debera utilizar el siguiente comando:

```bash
./imdb ./imdbv3.csv
```

Al terminar la ejecución, se crearán tres archivos `query1.csv`, `query2.csv`, `query3.csv`
los cuales contendrán respectivamente la salida obtenida luego de realizar las consultas.