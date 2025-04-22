#!/usr/bin/env bash

# -------------------------------------------------------------------
# install-deps.sh
#
# Usage: ./install-deps.sh [--dev|--deploy]
#
#   --dev    : install development deps (build-essential, -dev headers, CMake, etc.)
#   --deploy : install only runtime libraries
# -------------------------------------------------------------------

set -euo pipefail
#set -euxo pipefail # DEBUG MODE

# ——————————————————————————
# ensure script is run as root
if [[ "$(id -u)" -ne 0 ]]; then
  echo "❌ This script must be run as root. Please re‑run with sudo or as root:"
  echo "    sudo $0 $*"
  exit 1
fi

# 1) parse arguments
if [[ $# -ne 1 ]]; then
  echo "Usage: $0 [--dev|--deploy]"
  exit 1
fi

MODE="$1"
if [[ "$MODE" != "--dev" && "$MODE" != "--deploy" ]]; then
  echo "Error: unknown mode '$MODE'"
  echo "Usage: $0 [--dev|--deploy]"
  exit 1
fi

# 2) detect package manager
if   command -v apt-get &> /dev/null; then PM=apt
elif command -v dnf     &> /dev/null; then PM=dnf
elif command -v yum     &> /dev/null; then PM=yum
elif command -v pacman  &> /dev/null; then PM=pacman
elif command -v apk     &> /dev/null; then PM=apk
elif command -v zypper  &> /dev/null; then PM=zypper
else
  echo "Unsupported package manager. Exiting."
  exit 1
fi

# 3) define packages per manager
declare -a DEV_PKGS DEPLOY_PKGS

case "$PM" in

  apt)
    DEV_PKGS=(
      build-essential cmake git
      libsfml-dev
      libgl1-mesa-dev libegl1-mesa-dev
      libgl-dev freeglut3-dev libglfw3-dev
      libfreetype6-dev libglew-dev libglm-dev
      libfontconfig1-dev libcairomm-1.0-dev libpangomm-1.4-dev
      libboost-locale1.83-dev libboost-random1.83-dev libboost-timer1.83-dev
      libfftw3-dev libtinyxml2-dev
      libfreeimage-dev libfreeimageplus-dev
    )
    DEPLOY_PKGS=(
      libsfml-dev
      #libglx-mesa0
      libfreetype6
      libglut3.12
      libglu1-mesa
      #freeglut3
      libglew2.2 libglfw3 libglm-dev
      libfontconfig1
      libcairomm-1.0-dev libpangomm-1.4-dev
      libboost-locale1.83.0 libboost-random1.83.0 libboost-timer1.83.0
      libfftw3-bin libtinyxml2-10
      libfreeimage3 libfreeimageplus3
    )
    ;;

  dnf|yum)
    DEV_PKGS=(
      gcc gcc-c++ cmake git
      SFML-devel mesa-libGL-devel mesa-libEGL-devel
      freetype-devel glew-devel fontconfig-devel
      cairomm-devel pangomm-devel
      boost-locale-devel boost-random-devel boost-timer-devel
      freeglut-devel glfw-devel
      fftw-devel tinyxml2-devel glm-devel freeimageplus-devel
    )
    DEPLOY_PKGS=(
      SFML mesa-libGL mesa-libEGL
      freetype glew fontconfig
      cairomm pangomm
      boost-locale boost-random boost-timer
      freeglut glfw fftw tinyxml2 glm freeimageplus
    )
    ;;

  pacman)
    DEV_PKGS=(
      base-devel cmake git
      sfml mesa freetype2 glew fontconfig cairomm pangomm
      boost freeglut glfw fftw tinyxml2 glm freeimageplus
    )
    DEPLOY_PKGS=(
      sfml mesa freetype2 glew fontconfig cairomm pangomm
      boost freeglut glfw fftw tinyxml2 glm freeimageplus
    )
    ;;

  apk)
    DEV_PKGS=(
      build-base cmake git
      sfml-dev mesa-dev freetype-dev glew-dev fontconfig-dev
      cairomm-dev pangomm-dev
      boost-dev freeglut-dev glfw-dev fftw-dev tinyxml2-dev glm-dev freeimageplus-dev
    )
    DEPLOY_PKGS=(
      sfml mesa freetype glew fontconfig
      cairomm pangomm boost freeglut glfw fftw tinyxml2 glm freeimageplus
    )
    ;;

  zypper)
    DEV_PKGS=(
      gcc gcc-c++ cmake git
      libsfml-devel libGL1-devel libEGL-devel
      freetype2-devel libGLEW-devel fontconfig-devel
      cairomm-devel pangomm-devel
      libboost_locale-devel libboost_random-devel libboost_timer-devel
      freeglut-devel libglfw3-devel
      fftw3-devel tinyxml2-devel glm-devel libfreeimageplus-devel
    )
    DEPLOY_PKGS=(
      libsfml6 libGL1 libEGL1 freetype2 libGLEW2
      fontconfig1 cairomm pangomm
      libboost_locale1_74_0 libboost_random1_74_0 libboost_timer1_74_0
      freeglut3 libglfw3 fftw3 libtinyxml2-6 glm libfreeimageplus3
    )
    ;;
esac

# choose which to install
if [[ "$MODE" == "--dev" ]]; then
  TO_INSTALL=( "${DEV_PKGS[@]}" )
else
  TO_INSTALL=( "${DEPLOY_PKGS[@]}" )
fi

echo "Detected package manager: $PM"
echo "Installing packages (${MODE#--}): ${TO_INSTALL[*]}"

# 4) install
case "$PM" in

  apt)
    apt-get update
    DEBIAN_FRONTEND=noninteractive \
      apt-get install -y "${TO_INSTALL[@]}"
    rm -rf /var/lib/apt/lists/*
    ;;

  dnf)
    dnf install -y "${TO_INSTALL[@]}"
    ;;

  yum)
    yum install -y "${TO_INSTALL[@]}"
    ;;

  pacman)
    pacman -Sy --noconfirm "${TO_INSTALL[@]}"
    ;;

  apk)
    apk update
    apk add --no-cache "${TO_INSTALL[@]}"
    ;;

  zypper)
    zypper --non-interactive install "${TO_INSTALL[@]}"
    ;;
esac

echo "✔️  All packages installed successfully."
