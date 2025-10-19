## Graphics Component

TapiEngine utilizes Direct3D 11 to use hardware accceleration.  

<br>

The Graphics object per window encapsulates all direct3d functionality, and provides an extendable interface for individual 'objects' in the game to use (Component Architecture)

---
The overall architecture should look like the following diagram:
```
_____________               _____________________
| 3D Object |   <---------  |     Drawable      |
-------------               --------------------|      |----------------------------------------------------------- Transformation Constant Buffer
                            |     Update()      |  <---|    _____________                                                           |
                            | List of IBindable |  <------- | IBindable |  <------- InputLayout                                     |
                            ---------------------           -------------      |                                                    |
                                                            |   Bind()  |      |--- VertexBuffer                                    |
                                                            -------------      |                                                    |
                                                                               |--- VertexShader                                    |
                                                                               |                                                    |
                                                                               ...      ...                                         |
                                                                               |                                                    |
                                                                               |--- ConstantBuffer <---- Vertex Constant Buffer -----
                                                                                                     |
                                                                                                     |-- Pixel Constant Buffer 
```
Each 3D object would bind any needed feature within the graphics pipeline by calling Bind() on all IBindable inside Drawable's collection.  
This will be done at initialization or whenever changes are made (in the update loop) to reduce redundant operations.  
As for the constant buffer, since the transformation matrices will very likely be changed per frame, we keep a seperate reference to perform transformation.


### IBindable  
```
virtual void Bind()
```


### Drawable
