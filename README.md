# Quick start

Don't forget to `git clone --recursive-submodules ...` or `git submodule update --init` to get raylib

Build everything with `cmake`

No installation is introduced, so the main executable is located in the binary folder the project.

# Scenes

You can switch between scenes using `1`, `2`, `3`, `4`, `5` keys.

## 1. Drawing polygons

Draw polygons' vertexes with `left mouse button`. Press `Draw/Finish` button to control which polygon you are drawing

Use `space` to pause/unpause the scene. While paused you can drag the vertexes with `right mouse button`

Use `R` to reset the scene without resetting its parameters

Use `Ctrl + R` to reset the scene and the parameters 

Use `Delete` to erase the whole scene

You can move the scene with `arrow keys`

```C
// .gif will be here
```

## 2. Ellipses

Use `space` to pause/unpause the scene

```C
// .gif will be here
```


## 3. Is point in the triangle?

Drag points with `right mouse button`

Use `space` to switch between modes

```C
// .gif will be here
```

## 4. Elementary Bezier curves

Choose `order` of bezier curve (is can be from 1 to 10)

Drag points with `right mouse button`

```C
// .gif will be here
```

## 5. Bezier curves

Draw control points with `left mouse button`. Press `Enter` to draw next curve

Drag points with `right mouse button`

Use `space` to hide/show control points

Use `L` to align last curve

You can move the scene with `arrow keys` and scale with `mouse wheel`

```C
// .gif will be here
```