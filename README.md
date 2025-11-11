[![Build Status](https://github.com/temken/raytracer/workflows/Build%20Status/badge.svg)](https://github.com/temken/raytracer/actions)
[![codecov](https://codecov.io/gh/temken/raytracer/branch/main/graph/badge.svg)](https://codecov.io/gh/temken/raytracer)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

# RaytraCer++


## To do

- [x] Test Translate/Rotate/Spin for simple objects
- [x] Test Translate/Rotate/Spin for composite shapes
- [x] Test and fix the object orientation and rotations and spin (in particular rectangles and boxes)
- [x] Make a nice dynamic scene (spinning octahedron, spinning box, rotating light source ball)
- [x] Image texture and gradients for the background (spherical background image)
- [x] Scene: Solar system
- [x] Add a cylinder shell class of finite size.
- [ ] New renderer: Path tracing with NNE.
- [ ] Fix the bug of bright pixels in the ray tracer (maybe also pathtracer). Look up fireflys (try without russian roulette)
- [ ] Render some nice example of the Fresnel effect. Cylinder on a table. Wet road with lights from an oncoming car. Stuff like that.
- [ ] Visualize the contributions of N=1,2,3,4,5 scatterings
- [ ] Figure out how to make composite objects work.
- [ ] Maybe textures should not get stretched. How to have unstretched textures on cylinders, boxes, etc.
  - [ ] Textures for cubes/other composite objects. (Use only some of the texture? Special textures (t shaped) for cubes for example) Or each object uses its own texture.
- [ ] New shapes:
  - [ ] Cone (for pen)
  - [ ] Torus/Ring
  - [ ] Half Ring? (for globe)
  - [ ] Thick tube (requires a 2D ring)
- [ ] Create composite objects (street lamps, desk lamps, pencil, spots, table, Chair, glass, filled glass)
- [ ] Scene: Table see below
- [ ] If black pixels or noise still an issue? Improve the block blur algorithm to not smudge edges. -> Bilateral filter Or look at other Denoising algorithms.
- [ ] Image textures for other objects
  - [x] Spheres
  - [ ] Boxes
  - [ ] Cylinder/Tube
- [ ] Add more pre-defined colors
- [ ] Find a better title and make an ascii art header
- [ ] Improve the deterministic renderer by modeling mixtures of refraction and reflection. Split the ray in two, weigh each by their probability and propagate them recurseively. This should even reproduce the Fresnel effect.
- [ ] Mean free path inside the object with scattering color (could just be albedo color). Also useable for fog.
- [ ] Create camera based on aspect ratio and one pixel dimension
- [ ] Implement depth of field capability
- [ ] Normal textures especially for glass surfaces, but not just (who owns the normal texture?)
- [ ] Check out texture resources.
- [ ] Allow to run ALL renderes and store the renderer type in the file name. (?)
- [ ] Print preview of image downscaled in terminal
- [ ] Look into ogb mesh files.
- [ ] Create AABB from two points
- [ ] Shape should return vertices and min/max values to easily create bounding boxes
- [ ] Split AABB into two boxes along the longest side
- [ ] Camera effects:
  - [ ] Motion blur
  - [ ] Depth/aperture
- [ ] Camera/object have dynamics property in common (velocity, Evolve(t), etc). Common Interface?
  - [ ] Camera should have an ONB that handles location and orientation or a 3d motion base class
- [ ] Profile to check for bottlenecks.
- [ ] Organize textures
  - [ ] Normal, basecolor, specular, normal

### Image ideas:
- [ ] Light lanterns along a road in the night with nice ambient. Wet road, oncoming car.
- [x] Create a series of material comparisons, e.g. roughness, metallicity, glas, ...
- [ ] Recreate that photo I took in the pub.
- [ ] Table: Badly lid wooden table with chair: table lamp, two glasses, one filled, with an icecube, one fell over, spilled reflective surfuce of beverage, a globe, a sheet of paper (the liquid can be spilled on parts of it), a pen, a glass ball, etc. Maybe add some fog

### Known bugs and issues:
- [x] Increasing the resolution sometimes seems to widen the field of view and vice versa
- [ ] Test and debug the image class, import/export a file. Ensure that the image has the same orientation.
- [x] Debug the rectangle/ box classes. There is something off with the dimensions. This is related to the previous point potentially.
  - [x] Check the brick scene. The texture orientation is different for each wall. It might have to do with the fact that the normals are aligned with an axis -> Swap x,y of the rectangle.

### Other ideas

- Replace the RGB colors by a physical spectrum (i.e. a PDF over visible wavelengths). Implement an abstract Color base class, and then RGB and physical spectra can inherit from that.
  - It should be possible to model rainbows, or split white light on a prism, etc. 
- Define trajectories that the camera can follow (time dependence is already ).
- Create compositve objects with a physics engine included. Simulate and render a double pendulum in an interesting light ambient.
- Make a UI with qt or imgui to show the scene and camera, move stuff, render quick images. Simple UI to have some interactive fun.
- Look into GPUs, in particular c++ + Vulkan.

### Done tasks

- [x] Vector class
- [x] Ray class
- [x] Color class
- [x] Image class
- [x] Object (abstract) class
- [x] Scene class
- [x] Sphere class
- [x] Renderer (simple) class
- [x] Camera class
- [x] Video class
- [x] Generate the first png from a code defined image
- [x] Create a virtual world with a few simple objects
- [x] Take an image of it with a very simple deterministic ray tracing algorithm
- [x] Fix the field of view issue.
- [x] Implement camera pointing to origin with the possibility to rotate.
- [x] Export a video of a camera flying around the z-axis.
- [x] Anti-aliasing
- [x] Sample N pixel and take the average.
- [x] Add progress bar for rendering images
- [x] Rectangular
- [x] Disk
- [x] Cube/box (composite of 6 rectangulars, each with their own color)
- [x] Add a nice color scheme.
- [x] Progressbar for videos
- [x] Simulate a perfect mirror
- [x] Configuration singleton from imported file
- [x] Add configuration json file that also defines the destination of the generated images/videos (<ID>/images/image_01.png, <ID>/videos/video_01.mp4)
- [x] Define and import a scene in json file.
- [x] Implement the MC path tracing approach
- [x] Add textures to objects. At least rectangles should not be too hard. (store an image file with the object, promote mColor to Color(), and just return the color of the image at the hit point)
- [x] Smooth final images and reduce noise
- [x] Improve the texture logic in the rectangle class
- [x] Move image/video to utilities
- [x] Image bluring should be a config parameter of the camera
- [x] Convergence videos. 
- [x] Timedependence of scene/object and camera. Automatically used by video recording. Should also cover the fly around
- [x] Fix cylinder
- [x] Introduce torus class (Intersect is still buggy)
- [x] Optimize the rendering process with openmp or cuda (openmp might require gnu compilers)
- [x] Improve the config file
  - [x] Colors in the config file should be possible to define as integer 0..255, not just floats 0..1
  - [x] Take image or video -> Decide in config file
  - [x] Set number of threads in config file
- [x] Introduce a material class
  - [x] Semi-reflective/shiny objects
  - [x] LIght source are either on/off. Should be possible to dim them. Maybe this has something to do with luminance?
  - [x] Implement metallic surfaces
  - [x] Add refractive/transparent media.
- [x] Use the material class in the renderer
  - [x] Add material to object and parse from config file
  - [x] The intersection class should store the material not the object. That should be sufficient
  - [x] Use material in the Simple renderer
  - [x] Use material inthe MC renderer
  - [x] Update my config files
  - [x] Test
- [x] Fix the texture so I can have a brick room for future testing.
- [x] Move everything into the src files
- [x] Have a run ID that is consistent across output files.
    - [x] Also save the config yaml files and mark the exact git commit hash in it
- [x] Camera orbit should not look towards the origin, but just to the z-axis on its given height.
- [x] Create some nice images with the new class. Create some interesting images and videos using all the capabilities so far.
- [x] Implement Fresnel effect and the approximation
- [x] Composite object class with std::vector<Object*>. Where Objects are all 2D objects.
- [x] Get debugging to work.
- [x] Improve the renderer step by step.
  - [x] Deterministic renderer with light sampling and ambient light.
  - [x] Ray tracing algorithm with correct weighing(with ambient light?)
  - [x] Path tracing with correct weighing
  - [x] Build the raytracer and test the different shaders.
- [x] Fix the key points for cylindric light sources (see brick wall.yaml)
- [x] Extend the Renderer base class
  - [x] CollectDirectLight should be implemented only once here. (Ensure that the ray tracer image does not change!!!)
  - [x] Implement an ambient factor that will give black pixels a slight color. At least for deterministic, maybe also raytracer
- [x] Create a class for an orthonomal basis.
- [x] I removed the 1/pdf scaling in the material class for roughness. Ensure that this was PBR correct.
- [x] Separate geometric and physical objects. There are two options, either have the PhysicalObject (that inherits from GeometricObject or rather its child classes) have a member mMaterial (like now) or have it inherit from Material directly.
    - This is also since some algorithms (like hierarchical bounding boxes) will only need geometric objects, no need for them to have a material.
    - The geometric objects should have a ONB handling its orientation in 3D space.
- [x] Rename and fix Box -> AxisAlignedBox. Ensure that rotations and translations work with the new ONB.
- [x] All objects should have their own basis together with mPosition. Then we can have a RecalculateObject positions function using that basis. In rotate functions, we only have to rotate the ONB and rebuild the object. It also gives orientation for textures on spheres. It shoudl probably also be possible to define the orientation in the config file, beyond the normal. Indicating the front (corresponding e.g. to the x axis of the ONB)
- [x] Create a half sphere class
- [x] Implement Rotate and Spin functions properly.
- [x] Ensure that the Material class does not ever get copied around.
- [x] Illustrate the different shaders on an interesting scene and make a post.
- [x] Implement a triangle
- [x] Implement tetrahedron
- [x] Implement octahedoron
- [x] Dynamic scenes and camers:
  - [x] Introduce a ÍsDynamic() for Objects.
  - [x] Introduce `bool Scene::IsDynamic()` and `std::vector<ShapePtr> mDynamicObjects` to the Scene class. Give output in PrintInfo, is a scene dynamic, if yes how many objects are dynamic.
  - [x] IsDynamic in the camera
  - [x] Introcue `double mTime = 0.0` amd `void SetTime` to the Class.
  - [x] Have a `time` parameter in the config file that sets the time of the scene. Careful not to screw up the video rendering afterwards, either video should start at t=0 or the set time.
  - [x] Similar functionality in the `Camera` class.


## General notes

- build with [CMake](https://cmake.org/)
- continuous integration with [Github Actions](https://github.com/actions)
- unit testing with [googletest](https://github.com/google/googletest)
- code coverage with [codecov](https://codecov.io/).
- imports the static library [libphysica](https://github.com/temken/libphysica)

<details><summary>Repository content</summary>
<p>

The included folders are:

- *bin/*: This folder contains the executable after successful installation together with the configuration files.
- *external/*: This folder will only be created and filled during the build with CMake.
- *output/*: Each run of RaytraCer++ generates result files in a dedicated sub-folder named after the run's simulation ID string, which is specified in the configuration file.
- *src/*: Here you find the source code of RaytraCer++.
- *tests/*: All code and executable files of the unit tests are stored here.

</p>
</details>


## Getting started

<details><summary>1. Dependencies</summary>
<p>

Before we can install SOFTWARENAME, we need to make sure that a few dependencies are taken care of.

- [CMake](https://cmake.org/): SOFTWARENAME as well as the library libphysica are built with CMake.
- [boost](https://www.boost.org/): For numerical integration (used by libphysica).
- [libconfig](https://github.com/hyperrealm/libconfig): For the configuration files, SOFTWARENAME uses the libconfig library (required version at least 1.6). This will be installed by libphysica, if it is not already installed.
- [libphysica](https://github.com/temken/libphysica): Automatically downloaded to */external/*, compiled, and linked by CMake.


<details><summary>Installation of boost</summary>
<p>

```
>brew install boost
```

or alternatively with APT:

```
>sudo apt-get install libboost-all-dev
```

</p>
</details>

<details><summary>Installation of libconfig</summary>
<p>
This installation is optional, since `libphysica` will install it automatically, if it is not available.

On Macs, it can be on installed using [homebrew](https://brew.sh/)

```
>brew install libconfig
```

or using APT on Linux machines

```
>sudo apt-get update -y
>sudo apt-get install -y libconfig-dev
```

Alternatively, it can be built from the source files via

```
>wget https://hyperrealm.github.io/libconfig/dist/libconfig-1.7.2.tar.gz
>tar -xvzf libconfig-1.7.2.tar.gz
>pushd libconfig-1.7.2
>./configure
>make
>sudo make install
>popd
```

</p>
</details>

</p>
</details>

<details><summary>2. Downlad & Installation</summary>
<p>
The SOFTWARENAME source code can be downloaded by cloning this git repository:

```
>git clone https://github.com/temken/SOFTWARENAME.git 
>cd SOFTWARENAME
```

The code is compiled and the executable is created using CMake.

```
>cmake -E make_directory build
>cd build
>cmake -DCMAKE_BUILD_TYPE=Release -DCODE_COVERAGE=OFF ..
>cmake --build . --config Release
>cmake --install .
```

If everything worked well, there should be the executable *SOFTWARENAME* in the */bin/* folder.

</p>
</details>

<details><summary>3. Usage</summary>
<p>
Once SOFTWARENAME is installed, it can run by running the following command from the */bin/* folder:

```
>./SOFTWARENAME config.cfg
```

</p>
</details>

## Version History

- xx.xx.20xx: Release of version 0.1.0

## Everything else

<details><summary>Author & Contact</summary>
<p>

The author of RaytraCer++ is Timon Emken Christensen.

For questions, bug reports or other suggestions please open an [issue](https://github.com/temken/raytracer/issues).
</p>
</details>

<details><summary>License</summary>
<p>

This project is licensed under the MIT License - see the LICENSE file.

</p>
</details>