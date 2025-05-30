# Studios Slab
## A.K.A. "the anything bagel"
### Intro

This is a well-organized collection of physics code and general use libraries.

It contains discrete time-advancement of (classic) Klein-Gordon-like field theories (with non-differentiable potentials) and also path integrals of their quantum counterparts in a stochastic formalism. Also, as studies, montecarlo of magnetic models and Langevin dynamics of soft matter.

It also contains random coding projects of all sorts.

### Installation
There are two ways to install this repo: via Git or via Docker.

#### Manually (for development), via Git

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

Install Docker, and NVidia Container Toolkit, then run, on project root, run:

`docker build -t studioslab:latest.`

If all goes well, you should be able to run the container's entrypoint (Fields-RtoR) with:

`docker run -it --gpus all studioslab:latest`

As a primer, you can run the container's entrypoint with:

`docker run -it --gpus studioslab:latest -og`

### Usage

... to be written.

### Screenshots

#### Two-point function (low-left) of quantum 1+1 dimensional signum-Gordon field, plus some of its Fourier space data:

![Two-point function analysis](https://github.com/user-attachments/assets/4cb6300c-596c-4fe0-a8cd-d747d061831c)

#### Field momentum: 

![Momentum](https://github.com/user-attachments/assets/a770e7e7-1305-4c8c-8a57-6e00715ae4c2)

#### Histograms:

![Histograms](https://github.com/user-attachments/assets/c9aff204-0976-4819-bdc0-6f19eb51f0fa)

### History

This repo is where I've amalgamated most of my physics PhD code.
