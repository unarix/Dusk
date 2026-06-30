# Dusk

Night Shift para Haiku OS (Filtro de luz Azul).

Dusk ajusta la gamma de la pantalla para reducir la luz azul y hacer más cómodo
el uso de noche. Nada de overlays ni hacks raros, usa directamente
`BScreen::SetGammaRamp()`.

## Qué hace

- Aplica un tinte cálido a la pantalla modificando la rampa de gamma por canal
- Podés elegir la temperatura de color (de 1900K bien cálido a 6500K neutro)
- Se activa y desactiva con un botón, o desde el icono en el Deskbar
- Al cerrar la app restaura la gamma original

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

Calcula multiplicadores RGB a partir de la temperatura en Kelvin usando la
aproximación de Tanner Helland para radiación de cuerpo negro. Con eso arma
una rampa de 256 valores por canal y la mete con `SetGammaRamp()`. Cuando
desactivás, vuelve a poner la rampa lineal (identidad).

## Licencia

MIT
