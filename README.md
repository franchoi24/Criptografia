# Criptografia

## Build

En el directorio raíz ejecutar el comando 

```
make
```

## Clean

En el directorio raíz ejecutar el comando 

```
make clean
```

## Ejecución

Uso:

```
./ss [d|r] secretImagePath k shadesPath
```

Donde d es para encriptar y distribuir el secreto, r es para desencriptar (recuperar) el secreto.
secretImagePath es el path (absoluto o relativo) de la imagen a esconder.
k es la mínima cantidad de sombras requeridas para recuperar un secreto en un esquema (k, n).
shadesPath es el directorio en el cual se crean o del cual se obtienen las sombras, dependiendo de si se elige el parámetro d o r, respectivamente.