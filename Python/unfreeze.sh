#!/bin/bash

mkdir StudioSlab

python3 -m venv ./StudioSlab

source ./StudioSlab/bin/activate

pip install -r requirements.txt
