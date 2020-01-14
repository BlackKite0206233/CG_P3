# Computer Graphic Project 3

## Usage

### Keyboard

C : Add ControlPoint  
P : Add Path  
T : Add Train   
Q : Add Car   
W : Remove Car   
D : Remove ControlPoint / Path / Train  
N : Switch Train (when in Train Camera Mode)
R : Rotate ControlPoint  

1 : World Camera Mode  
2 : Top Camera Mode  
3 : Train Camera Mode  

4 : Linear Track Mode  
5 : Cardinal Track Mode  
6 : Cubic Track Mode  

7 : Line Mode  
8 : Track Mode  
9 : Road Mode

0 : Trun back to original oriention (when in Train Camera Mode)

F1 : Normal Render Mode  
F2 : SSAO Mode  
F3 : Toon Shader Mode  

F4 : Show / Hide Train (when in Train Camera Mode)

Space : Start / Stop train move


Alt (hold) : Alt Mode  
Shift (hold) : Shift Mode  
Ctrl (hold) : Ctrl Mode

\- : decrease Train Speed  
\+ : increase Train Speed  
\- (Shift Mode) : decrease Motion Blur Intensity  
\+ (Shift Mode) : increase  Motion Blur Intensity  
\- (Ctrl Mode) : decrease Bloom Intensity  
\+ (Ctrl Mode) : increase  Bloom Intensity  

### Mouse

move : Rotate Camera  
move (Alt Mode) : Pan Camera  
wheel : Zoom In / Out

## Tech

### Track

- Subdivsion
- Track Graph (branch)
- ArcBall on ControlPoint

### Water

- Normal Map
- dudv Map
- Reflection
- Refraction

### Terrain

- Generated Terrain
    - Normal Distribution
    - Gaussian Filter
    - Cosine Interpolation
- Useing different texture according to the height of terrain
    - Grass
    - Mud
    - Under Water
- Terrain Collision

### SkyBox

- Fog
- Switch Day / Night
- Rotated Skybox

### Shader

- Blinn Phong Shading
- Toon Shading
- SSAO
- Motion Blur
- Bloom

### Others
