[![Build Status](https://github.com/temken/raytracer/workflows/Build%20Status/badge.svg)](https://github.com/temken/raytracer/actions)
[![codecov](https://codecov.io/gh/temken/raytracer/branch/main/graph/badge.svg)](https://codecov.io/gh/temken/raytracer)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

# RaytraCer++


## To do

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
- [ ] Add more pre-defined colors
- [ ] Create some nice images with the new class. Create some interesting images and videos using all the capabilities so far.
- [ ] Renderer with next event estimation: Create a renderer with deterministic shadows by checking for the closest light source or something.
  - [ ] Could this be a way to solve the black pixel issue?
- [ ] If still an issue? Improve the block blur algorithm to not smudge edges. -> Bilateral filter
- [ ] Print preview of image downscaled in terminal
- [ ] Test and fix the object orientation and rotations and spin (in particular rectangles and boxes)
- [ ] Mean free path inside the object with scattering color (could just be albedo color). Also useable for fog.
- [ ] Check out BSDF
- [ ] Get debugging to work.
- [ ] Implement depth of field capability
- [ ] Create camera based on aspect ratio and one pixel dimension
- [ ] New objects: Cones, Rings
- [ ] Normal textures especially for glass surfaces, but not just
- [ ] Image textures for other objects (Spheres!)


Image ideas:
- Light lanterns along a road in the night with nice ambient
- Create a series of material comparisons, e.g. roughness, metallicity, glas, ...

Known bugs and issues:
- [ ] Increasing the resolution sometimes seems to widen the field of view and vice versa
- [ ] Test and debug the image class, import/export a file. Ensure that the image has the same orientation.
- [ ] Debug the rectangle/ box classes. There is something off with the dimensions. This is related to the previous point potentially.
  - [ ] Check the brick scene. The texture orientation is different for each wall. It might have to do with the fact that the normals are aligned with an axis -> Swap x,y of the rectangle.

Other ideas:

- Define trajectories that the camera can follow (time dependence is already ).
- Enable background gradients/images.
- Preview images in the terminal
- Make a UI with qt or imgui to show the scene and camera, move stuff, render quick images. Simple UI to have some interactive fun.

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
- *data/*: Contains additional data necessary for the simulations, e.g. the solar model tables.
- *external/*: This folder will only be created and filled during the build with CMake.
- *include/*: All header files of RaytraCer++ can be found here.
- *results/*: Each run of RaytraCer++ generates result files in a dedicated sub-folder named after the run's simulation ID string, which is specified in the configuration file.
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

<details><summary>Citing SOFTWARENAME</summary>
<p>

If you decide to use this code, please cite the latest archived version,

> Emken, T., 20xx, SOFTWARENAME [Code, v0.1.0], Astrophysics Source Code Library, record [[ascl:xxxx.xxx]](https://ascl.net/xxxx.xxx), [[DOI:10.5281/zenodo.xxxxxxx]](https://doi.org/10.5281/zenodo.xxxxxxx)

Bibtex entry:

```
@software{SOFTWARENAME,
  author = {Emken, Timon},
  title = {{SOFTWARENAME [Code, v0.1.0]}},
  year         = {20xx},
  publisher    = {Zenodo},
  version      = {v0.1.0},
  doi          = {DOI:10.5281/zenodo.xxxxxxx},
  url          = {https://doi.org/10.5281/zenodo.xxxxxxx},
  howpublished={Astrophysics Source Code Library record \href{https://ascl.net/xxxx.xxx}{[ascl:xxxx.xxx]}. The code can be found under \url{https://github.com/temken/SOFTWARENAME}. Version 0.1.1 is archived as \href{https://doi.org/10.5281/zenodo.xxxxxxx}{DOI:10.5281/zenodo.5957388}}
}
```

<!-- As well as the original publications,

> authors , year,  **title**, [[arXiv:xxxx.xxxxx]](https://arxiv.org/abs/xxxx.xxxxx). -->

</p>
</details>

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