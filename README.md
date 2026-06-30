# Dusk

Filtro de luz azul para Haiku OS, estilo Night Shift.

Dusk ajusta la LUT (lookup table) del hardware gráfico para reducir la luz
azul y hacer más cómodo el uso de la compu de noche. Modifica directamente
los registros de paleta del CRTC a través del accelerant, lo que afecta toda
la salida de video sin importar el modo de color.

## Qué hace

- Modifica la gamma del display usando el hook `B_SET_INDEXED_COLORS` del accelerant
- Podés elegir la temperatura de color (de 1900K bien cálido a 6500K neutro)
- Se activa y desactiva con un botón, o desde el icono en el Deskbar
- Al cerrar la app restaura la paleta original (rampa lineal)

## Requisitos

- Haiku con driver gráfico que soporte `B_SET_INDEXED_COLORS`
- **Funciona:** intel_extreme, radeon_hd, radeon (escriben a la LUT del CRTC en todos los modos)
- **Parcial:** VESA (implementa el hook, pero la paleta VGA solo afecta modos 8-bit en la mayoría del hardware — en RGB32 puede no tener efecto visible)
- **No funciona (todavía):** nvidia-haiku de X547 (no implementa ese hook)

## Compilar

```bash
make
```

Necesitás tener el entorno de desarrollo de Haiku configurado y acceso a los
headers privados del app_server (vienen con haiku_devel).

## Uso

Abrí Dusk desde el menú de aplicaciones o desde la terminal. Aparece una ventanita
con un slider para la temperatura y un botón para activar/desactivar.

## Cómo funciona por dentro

1. Pide al app_server los paths del driver y del accelerant activos
2. Abre el device y clona el accelerant (se conecta al que ya usa el app_server)
3. Calcula multiplicadores RGB según la temperatura usando la aproximación de
   Tanner Helland para radiación de cuerpo negro
4. Arma una paleta de 256 entradas con la corrección aplicada
5. La escribe a la LUT del hardware con `set_indexed_colors`
6. Al desactivar, escribe la rampa de identidad (lineal)

## Licencia

MIT
