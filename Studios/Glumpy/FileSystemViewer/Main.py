import genericpath

import imgui
from glumpy import app, glm
from glumpy.app import Viewport as GLViewport
from glumpy.geometry import primitives

import numpy

from PySLib.Viewers3D import ObjectViewer3D
from PySLib.Viewers3D import ObjectInstance




import glob
import os


dir_name = '//'


def ListFiles(of_dir):
    list_of_files = filter( os.path.isfile,
                            glob.glob(dir_name + '*') )

    files_with_size = [ (file_path, os.stat(file_path).st_size)
                        for file_path in list_of_files ]

    for file_path, file_size in files_with_size:
        print(file_size, ' -->', file_path)


def ListDirectories(of_dir):
    dirs = os.listdir(of_dir)
    print(dirs)


def GetAllFilesAndFolders(in_dir):
    list_of_files = glob.glob(dir_name + '*', recursive=True)

    # files_with_size = [(file_path, os.stat(file_path).st_size)
    #                   for file_path in list_of_files]
    # for file_path, file_size in files_with_size:
    #    print(file_size, ' -->', file_path)

    return [(file_path, os.stat(file_path).st_size)
                       for file_path in list_of_files]

def ListAll(in_dir):
    files_with_size = GetAllFilesAndFolders(in_dir)

    for file, size in files_with_size:
        print(str(size/4096)+' 2^12 Bytes  ', file)








WindowWidth = int(1920*1.5)
WindowHeight = int(1080*1.5)


app.use("glfw_imgui")  # Required for ImGui integration
window = app.Window(width=WindowWidth, height=WindowHeight,
                    color=(0.30, 0.30, 0.35, 1.00))


vp = GLViewport((WindowWidth, WindowHeight))
window.attach(vp)



viewer3D = ObjectViewer3D.ObjectViewer3D(viewport=vp)

allFiles = GetAllFilesAndFolders(dir_name)

current_pos = .0
for file, size in allFiles:
    vol = size/4096

    modelview = numpy.eye(4, dtype=numpy.float32)

    prim = None
    side = None

    measure = None

    if os.path.isfile(file):
        prim = lambda diam: primitives.teapot(diam*.5)
        side = (0.75*vol/numpy.pi)**(1/3)

        measure = side

        glm.translate(modelview, current_pos + side * 0.5, 0, side * .5)
    else:
        prim = primitives.cube
        side = (vol**(1/3))

        #(xy/sz)(sz.z) = vol
        sz = 0.15
        sxy = sz**-0.5
        glm.scale(modelview, side*sxy, side*sxy, side*sz)
        side *= sxy
        glm.translate(modelview, current_pos+side*0.5, 0, sz*.5)


        measure = 1

    print("Adding object of volume {} and size/radius {}".format(vol, side))

    verts, ids = prim(measure)

    current_pos += side*1.1

    plane = ObjectInstance.ObjectInstance(verts, ids, modelview)
    viewer3D.addObject3D(plane)




@window.event
def on_draw(frame_dt):
    window.clear()

    viewer3D.draw()

    imgui.new_frame()
    viewer3D.draw_menu()
    imgui.end_frame()
    imgui.render()


if __name__ == '__main__':

    print('---------------')

    print("FILES")
    ListFiles(dir_name)

    print("")
    print("DIRECTORIES")
    ListDirectories(dir_name)

    print("")
    print("ALL")
    ListAll(dir_name)

    app.run()