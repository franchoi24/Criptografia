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

### Encriptar

```
./ss d secretImagePath k shadesPath
```


* **d** es el parametro indicado para encriptar el secreto
* **secretImagePath** es el path (absoluto o relativo) de la imagen a esconder
* **k** es la mínima cantidad de sombras requeridas para recuperar un secreto en un esquema (k, n).
* **shadesPath** es el directorio del cual se obtienen las imagenes originales para utilizar como sombras

Se generarán k sombras en el directorio **shadesPath-shades** numeradas de la forma **shade-x-nombreOriginal.bmp** que corresponden a las sombras con el secreto distribuido.

##### Ejemplo de uso

```
./ss d res/Albert.bmp 6 ./res/someImages
```

### Desencriptar

```
./ss s secretImagePath k shadesPath
```

Donde:
* **s** es el parametro indicado para recuperar el secreto
* **secretImagePath** es el path + nombre de la imagen a recuperar
* **k** es la cantidad de sombras utilizadas para esconder la imagen
* **shadesPath** es el directorio en el cual estan las sombras ordenadas alfabeticamente segun fueron encriptadas

##### Ejemplo de uso
```
./ss r ./secret.bmp 6 ./res/someImages-shades
```
