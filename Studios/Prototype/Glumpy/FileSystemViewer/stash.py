import numpy as np
from glumpy import app, gl, glm, gloo, data
from glumpy.geometry import primitives
from glumpy.app import Viewport




vertex = """
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform float tx, ty;

attribute vec3 normal;
attribute vec3 position;
attribute vec2 texcoord;

varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;
void main()
{
    v_texcoord = texcoord;
    v_normal = normal;
    vec2 x = position.xy + vec2(tx, ty);
    v_position = vec3(x, position.z);
    //gl_Position = projection * view * model * vec4(v_position, 1.0);
    gl_Position = <transform>;
}
"""

fragment = """
uniform mat4 view;
uniform mat4 model;
uniform mat4 normal;
uniform mat4 projection;

uniform vec3 light1_position;
uniform vec3 light2_position;
uniform vec3 light3_position;
uniform vec3 light1_color;
uniform vec3 light2_color;
uniform vec3 light3_color;

uniform sampler2D texture;

varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;

float lighting(vec3 light_position)
{
    // Calculate normal in world coordinates
    vec3 n = normalize(normal * vec4(v_normal,1.0)).xyz;

    // Calculate the location of this fragment (pixel) in world coordinates
    vec3 position = vec3(view * model * vec4(v_position, 1));

    // Calculate the vector from this pixels surface to the light source
    vec3 surface_to_light = light_position - position;

    // Calculate the cosine of the angle of incidence (brightness)
    float brightness = dot(n, surface_to_light) /
                      (length(surface_to_light) * length(n));
    brightness = max(min(brightness,1.0),0.0);
    return brightness;
}

void main()
{
    vec4 l1 = vec4(light1_color * lighting(light1_position), 1);
    vec4 l2 = vec4(light2_color * lighting(light2_position), 1);
    vec4 l3 = vec4(light3_color * lighting(light3_position), 1);

    float r = texture2D(texture, v_texcoord).r;
    vec4 color = vec4(r,r,r,1);

    // Calculate final color of the pixel, based on:
    // 1. The angle of incidence: brightness
    // 2. The color/intensities of the light: light.intensities
    // 3. The texture and texture coord: texture(tex, fragTexCoord)
    gl_FragColor = color *( 0.25 + 0.75*(l1+l2+l3));
}
"""

app.use("glfw_imgui")
window = app.Window(width=1600, height=900, color=(1,1,1,1))

vp = Viewport((1600, 900))
window.attach(vp)


@window.event
def on_draw(dt):
    global phi, theta, duration, index, shapes
    window.clear()

    change_primitive(index)
    program['tx'] = 1
    program['ty'] = 0
    program.draw(gl.GL_TRIANGLES, indicesPlane)


    change_primitive((index + 1) % len(shapes))
    program['tx'] = 0
    program['ty'] = 0
    program.draw(gl.GL_TRIANGLES, indicesPlane)


    theta = 15
    phi = -60
    model = np.eye(4, dtype=np.float32)
    glm.rotate(model, phi, 1, 0, 0)
    glm.rotate(model, theta, 0, 1, 0)
    program['model'] = model
    program['normal'] = np.array(np.matrix(np.dot(view, model)).I.T)


@window.event
def on_resize(width, height):
    program['projection'] = glm.perspective(45.0, width / float(height), 2.0, 100.0)

@window.event
def on_init():
    gl.glEnable(gl.GL_DEPTH_TEST)

@window.event
def on_key_press(key, modifiers):
    global index

    if key == ord(' '):
        index = (index + 1) % len(shapes)
        change_primitive(index)

def change_primitive(new_index):
    global verticesPlane, indicesPlane

    verticesPlane, indicesPlane = shapes[new_index]
    program.bind(verticesPlane)


index = 0
shapes = [ primitives.plane(0.8),
           primitives.cube(0.9),
           primitives.sphere(),
           primitives.cubesphere(),
           primitives.cylinder(),
           primitives.torus(),
           primitives.cone(),
           primitives.pyramid(),
           primitives.teapot() ]
verticesPlane, indicesPlane = shapes[index]

program = gloo.Program(vertex, fragment)
program.bind(verticesPlane)
view = np.eye(4, dtype=np.float32)
model = np.eye(4, dtype=np.float32)
projection = np.eye(4, dtype=np.float32)
glm.translate(view, 0, 0, -5)
program['model'] = model
program['view'] = view
program['normal'] = np.array(np.matrix(np.dot(view, model)).I.T)
program['texture'] = data.checkerboard()
program['tx'] = 0
program['ty'] = 0

trackball = TrackballPan(Position('v_position'), znear=0.1, zfar=500, distance=5)
program['transform'] = trackball

program["light1_position"] = 3, 0, 0+5
program["light2_position"] = 0, 3, 0+5
program["light3_position"] = -3, -3, +5
program["light1_color"]    = 1, 0, 0
program["light2_color"]    = 0, 1, 0
program["light3_color"]    = 0, 0, 1
phi, theta = 0, 0

app.run()


