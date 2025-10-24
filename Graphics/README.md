## Graphics Component

TapiEngine utilizes Direct3D 11 to use hardware accceleration.  

<br>

The Graphics object per window encapsulates all direct3d functionality, and provides an extendable interface for individual 'objects' in the game to use (Component Architecture)

---
The overall architecture should look like the following diagram:
```
_____________               _____________________
| 3D Object |          ---  |     Drawable      |
-------------          |    --------------------|      |----------------------------------------------------------- Transformation Constant Buffer
      ^                |    |     Update()      |  <---|    _____________                                                           |
      |                |    | List of IBindable |  <------- | IBindable |  <------- InputLayout                                     |
      |                |    ---------------------           -------------      |                                                    |
___________________    |                                    |   Bind()  |      |--- VertexBuffer                                    |
| DrawableBase<T> | <--|                                    -------------      |                                                    |
-------------------                                                            |--- VertexShader                                    |
                                                                               |                                                    |
                                                                               ...      ...                                         |
                                                                               |                                                    |
                                                                               |--- ConstantBuffer <---- Vertex Constant Buffer -----
                                                                                                     |
                                                                                                     |-- Pixel Constant Buffer 
```
Each 3D object would bind any needed feature within the graphics pipeline by calling Bind() on all IBindable inside Drawable's collection.  
This will be done at initialization or whenever changes are made (in the update loop) to reduce redundant operations.  
As for the constant buffer, since the transformation matrices will change per frame, we keep a seperate reference to perform transformation.

A Child class Drawable Base from Drawable will be templated to account for different structural data needed per 3D object.
All instances of the same templated DrawableBase (ex. DrawableBase\<Cube> or DrawableBase\<Sphere>) shares a static buffer to operate on.

### IBindable  
```
virtual void Bind()
```
Performs any Direct3D pipeline-side function of the corresponding class. Constant Buffers are handled slightly differently to hold per-instance data.

### Drawable
```
virtual void Update()
```
Performs any update needed per frame.


## Geometric Primitives
All geometric primitive shapes such as cubes, planes, prisms, cones and spheres are essentially a wrapper for IndexedTriangleList.  

```Make()``` function transforms these primitives into IndexedTriangleList along with any divisions if any. This allows for tesselated textures or vertex manipulations.

## Others

I have used Microsoft's DirectXTK (DirectX Toolkit) for implementing sprite / text rendering.  

All font files must be of ```.spritefont``` extension and be included in ```Graphics/Fonts``` directory.  

To convert installed ```.ttf``` to ```.spritefont```, use the provided binary by Microsoft's DirectXTK library with the following command:  
```
MakeSpriteFont "My Font" myfont.spritefont /FontSize:16
```