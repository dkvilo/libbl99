## Libbl99

Libbl99 is a simple abstraction for OpenGL 2.0 and GLSL 110 written in c99. This "library" was developed while I was traveling from Tbilisi to Batumi by train and was trying to show off myself to my seat neighbors.

So it's safe to say that, You don't want to use it.

```
--- Help
esc - Terminate program
r   - Reload Current Shader
h   - Remove Spacing
```

## Building Examples
Tested and run on Mac arm64/Intel

```bash
  $ git submodule update --init --recursive
  $ make glfw
  $ make build
  $ ./build/libbl
```

<img src="https://github.com/dkvilo/libbl99/blob/master/preview.gif" />


### TODO (Nice to have)
- [x] Shader hot reload
- [ ] SDF Font rendering
- [ ] Font Batching

