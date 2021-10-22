#version 460

//out vec4 fragd_color;
//layout(location=0) out vec4 color;

varying vec2 v_spaceCoord;
uniform vec2 SpaceDim;

const float pi = 3.14159265359;

void main() {
    float ang = pi; //1*pi;

    const float WindingNumber = 1.0;

    mat2 Rot = mat2(cos(ang), sin(ang), -sin(ang), cos(ang));
    vec2 transl = vec2(0, 0);
    vec2 r = Rot*(v_spaceCoord*SpaceDim-transl);
    float R = length(r);
    float th = exp(-R/0.05)*atan(r.y, r.x) + 0.0*pi;
    //float th = 1*pi;
    //float th = -WindingNumber*atan(r.y, r.x) - 2.476*pi;
    float dthdt = 0;

    // BOMBA
    //float l=length(r-vec2(0.25,0));
    //float radius = 0.1;
    //if(l<radius){
    //    th += sin(2*pi*(l-radius));
    //    th = cos(2*pi*(l-radius));
    //    //dthdt = -sin(2*pi*(l-radius));
    //}


    gl_FragColor = vec4(th, dthdt, 0, 1); // dirac delta
}
