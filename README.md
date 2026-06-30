# Dusk

Filtro de luz azul para Haiku OS, estilo Night Shift.

Dusk superpone un tinte cálido sobre la pantalla para reducir la luz azul y
hacer más cómodo el uso de la compu de noche.

## Qué hace

- Pone un overlay semi-transparente con tinte naranja/cálido sobre toda la pantalla
- Podés elegir la temperatura de color (de 1900K bien cálido a 6500K neutro)
- Se activa y desactiva con un botón, o desde el icono en el Deskbar
- El overlay no interfiere con el mouse ni el teclado (click-through)
- Al cerrar la app se destruye el overlay

## Compilar

```bash
make
```

Necesitás tener el entorno de desarrollo de Haiku configurado (gcc, makefile-engine).

## Instalar

```bash
make install
```

## Uso

Abrí Dusk desde el menú de aplicaciones o desde la terminal. Aparece una ventanita
con un slider para la temperatura y un botón para activar/desactivar. También se
agrega un icono al tray del Deskbar: click izquierdo togglea, click derecho abre
un menú con opciones.

## Cómo funciona por dentro

Crea una ventana `BWindow` fullscreen sin bordes, con `B_FLOATING_ALL_WINDOW_FEEL`
(queda encima de todo) y `B_AVOID_FOCUS` (no roba el foco, deja pasar los clicks).
Dibuja un rect del tamaño de la pantalla con un color cálido y alpha blending.
La intensidad del tinte se calcula a partir de la temperatura en Kelvin usando
la aproximación de Tanner Helland para radiación de cuerpo negro.

## Licencia

MIT
