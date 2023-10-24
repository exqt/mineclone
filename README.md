# Mineclone
![](screenshots/1.png)


![](https://img.youtube.com/vi/OdZ4iz9_71A/sddefault.jpg)

[demo video](https://www.youtube.com/watch?v=OdZ4iz9_71A)

Minecraft clone from scratch.

Work in progress

# Control
- WASD: move
- Space: jump
- F: toggle flying
- Left Shift: sprint
- Left Mouse Click: break block
- Right Mouse Click: place block
- Mouse Wheel: change block type
- Tab: toggle relative mouse mode
- Esc: exit

# TODO List
- [x] Chunk Rendering
- [x] AABB Collision
- [ ] Terrain Generation
- [ ] Chunk Loading/Saving
- [ ] Optimizations
- [ ] Lighting
- [x] Multiplayer
- [ ] Refactoring
- [ ] CI/CD

# Bugs
- [ ] floating point precision error in collision detection

# Requirements
- gcc (C++17)
- cmake
- Ninja
- glew
- SDL2

# Third Party Libraries
- GLM
- FastNoise
- stb
- imgui
- enet

# Build
```bash
cmake -B build -G Ninja
cmake --build build
```