// -----------------------------------------------------------------------------
// Copyright (c) 2009-2016 Nicolas P. Rougier. All rights reserved.
// Distributed under the (new) BSD License.
// -----------------------------------------------------------------------------
// From Fluid demo by Philip Rideout
// Originals sources and explanation on http://prideout.net/blog/?p=58
// -----------------------------------------------------------------------------
attribute vec2 Position;
attribute vec2 SpaceCoord;

varying vec2 v_spaceCoord;
varying vec2 v_position;

void main()
{
    v_spaceCoord = SpaceCoord;
    v_position = Position;
    gl_Position = vec4(v_position, 0.0, 1.0);
}
