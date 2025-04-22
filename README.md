# Studios Slab
## A.K.A. "the anything bagel"
### Intro 

This repo is where I've amalgamated most of my physics PhD code. Discrete time-advancement of (classic) Klein-Gordon-like field theories (with non-differentiable potentials) and also path integrals of their quantum counterparts in a stochastic formalism. Also, as studies, montecarlo of magnetic models and Langevin dynamics of soft matter.

It is mostly C++ but, because jupyter notebooks get so big, github thinks it is mostly Jupiter.

It also contains most of my coding projects of all sorts and own libraries of common functionality.

The root folder contains the base CMakeFiles. It automatically includes all C++ targets.

### Installation
There are two ways to install this repo: via git or via Docker.

#### Via git

Assuming you're using Ubuntu, you'll need git to pull this repo:

`sudo apt-get update && sudo apt-get install git`

Choose a suitable location (_e.g._ `~/Dev/`), then 

`git clone https://github.com/jstreibel/StudioSlab.git`

Install dependencies. On most Linux systems, you can do this by running (notice this script has only been tested on Ubuntu):

`chmod +x StudioSlab/Scripts/install-deps.sh`

`./StudioSlab/Scripts/install-deps.sh`

If your Linux system is not contemplated here, or you're on Windows, you'll need to install the dependencies manually.

Here's a comprehensive list of what you need (remember to choose the development versions):

cmake \
SFML \
FreeType \
GLEW \
Fontconfig \
Cairomm \
Pangomm \
FreeGLUT \
GLFW \
cxxopts \
FFTW3 \
TinyXML2 \
GLM \
FreeImage \
OpenGL and EGL \
Boost::Locale \
Boost::Random \
Boost::Timer

Then, build the project:

`cd StudioSlab/ && mkdir build && cd build && cmake .. && make`

#### Via Docker

...to be written.

### Screenshots

#### Two-point function (low-left) of quantum 1+1 dimensional signum-Gordon field, plus some of its Fourier space data:

![Two-point function analysis](https://github.com/user-attachments/assets/4cb6300c-596c-4fe0-a8cd-d747d061831c)

#### Field momentum: 

![Momentum](https://github.com/user-attachments/assets/a770e7e7-1305-4c8c-8a57-6e00715ae4c2)

#### Histograms:

![Histograms](https://github.com/user-attachments/assets/c9aff204-0976-4819-bdc0-6f19eb51f0fa)

