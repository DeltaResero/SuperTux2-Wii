# Engine formats and build options

Notes on the file formats the engine reads and the build options that change
how it draws. Written for someone changing the engine rather than making a
level.

## Fonts

A font is a `.stf` file in `data/fonts/`. It describes where the letters sit on
one or more images in `data/images/engine/fonts/`, which are ordinary RGBA
pictures in any format SDL can load. The `.stf` file carries the measurements
and the character set; the image carries the shapes.

```
(supertux-font
  ; height of a glyph on the image
  (glyph-height 14)

  ; default width of a glyph, and the width used by a fixed-width surface
  (glyph-width 7)

  ; blank pixels around every glyph on the image, on all four sides.
  ; Optional, nothing means none.
  (glyph-border 1)

  ; text in this font reads right to left. Optional.
  (rtl #t)

  ; one or more images, each with its own character set
  (surface

    ; overrides the width given above, for this image only
    (glyph-width 7)

    ; every glyph on this image is the width given, rather than measured
    ; from the picture. Optional.
    (monospace #t)

    ; the picture holding the letters, relative to data/images/engine/fonts/
    (glyphs "andale12.png")

    ; a matching picture holding their shadows, laid out the same way
    (shadows "andale12-shadow.png")

    ; which letter sits in which cell. One string per row of the image,
    ; a space to leave a cell empty
    (chars "abcdef"
           "ghijkl")
  )
)
```

Glyphs are laid out in a grid. With a border of *b*, a glyph in row *r* and
column *c* is found at `x = c * (width + 2b) + b`, `y = r * (height + 2b) + b`,
so the border is space between the cells and not part of any letter.

`glyph-height` is required and loading fails without it. Everything else has a
default.

## Textures

Two build options decide what shape a texture is allowed to be, and they
compose. Neither is about a particular machine.

`ENABLE_NPOT_TEXTURES` says the hardware will take a texture of any size. When
it is off, each side is rounded up to a power of two, which is what old OpenGL
without `ARB_texture_non_power_of_two` needs. With an extension loader present
the answer is found at runtime instead, so the option only matters for a build
that has none.

`TEXTURE_ALIGNMENT` rounds each side up to a multiple of itself, and 1 leaves
sizes alone. This is not a console thing: block-compressed formats want four on
a desktop as well, and other tiled hardware wants its own number.

The two together give three useful arrangements: rounding to a power of two,
no rounding at all, or rounding to a small tile.

## The lightmap

Lights are drawn into a smaller picture of their own, which is then multiplied
over the frame. `ENABLE_LIGHTMAP_FBO` draws that picture straight into a
texture using a framebuffer object. With the option off it is drawn on screen
and copied back out afterwards, which is slower but asks less of the driver.
