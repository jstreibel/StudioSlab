//
// Created by joao on 16/09/23.
//

#include <cstring>
#include <cassert>
#include <string>
#include <cstdlib>
#include <utility>

#include "Utils/Utils.h"

#include "rougier/vec234.h"
#include "VertexBuffer.h"
#include "Core/Tools/Log.h"

/**
 * Buffer status
 */
#define CLEAN  (0)
#define DIRTY  (1)
#define FROZEN (2)

namespace Slab::Graphics::OpenGL {

    FVertexBuffer::FVertexBuffer(Str formatStr) {
        size_t i, index = 0, stride = 0;
        const char *start = nullptr, *end = nullptr;
        GLchar *pointer = nullptr;

        format = std::move(formatStr);

        for( i=0; i<MAX_VERTEX_ATTRIBUTE; ++i )
        {
            attributes[i] = nullptr;
        }

        start = format.c_str();
        do
        {
            char *desc = nullptr;
            ftgl::vertex_attribute_t *attribute;
            GLuint attribute_size = 0;
            end = (char *) (strchr(start+1, ','));

            if (end == nullptr)
            {
                desc = strdup( start );
            }
            else
            {
                desc = strndup( start, end-start );
            }
            attribute = ftgl::vertex_attribute_parse( desc );
            start = end+1;
            free(desc);
            attribute->pointer = pointer;

            switch( attribute->type )
            {
                case GL_BOOL:           attribute_size = sizeof(GLboolean); break;
                case GL_BYTE:           attribute_size = sizeof(GLbyte); break;
                case GL_UNSIGNED_BYTE:  attribute_size = sizeof(GLubyte); break;
                case GL_SHORT:          attribute_size = sizeof(GLshort); break;
                case GL_UNSIGNED_SHORT: attribute_size = sizeof(GLushort); break;
                case GL_INT:            attribute_size = sizeof(GLint); break;
                case GL_UNSIGNED_INT:   attribute_size = sizeof(GLuint); break;
                case GL_FLOAT:          attribute_size = sizeof(GLfloat); break;
                default:                attribute_size = 0;
            }
            stride  += attribute->size*attribute_size;
            pointer += attribute->size*attribute_size;
            attributes[index] = attribute;
            index++;
        } while ( end && (index < MAX_VERTEX_ATTRIBUTE) );

        for( i=0; i<index; ++i )
        {
            attributes[i]->stride = (GLsizei) stride;
        }

        VAO_id = 0;

        vertices = ftgl::vector_new( stride );
        vertices_id  = 0;
        GPU_vsize = 0;

        indices = ftgl::vector_new( sizeof(GLuint) );
        indices_id  = 0;
        GPU_isize = 0;

        items = ftgl::vector_new( sizeof(ftgl::ivec4) );
        state = DIRTY;
        primitive = GL_TRIANGLES;

        Core::Log::Debug() << "Instantiated new VertexBuffer" << Core::Log::Flush;
    }

    FVertexBuffer::~FVertexBuffer() {
        size_t i;

        for( i=0; i<MAX_VERTEX_ATTRIBUTE; ++i )
        {
            if( attributes[i] )
            {
                vertex_attribute_delete( attributes[i] );
            }
        }


        if( VAO_id )
        {
            glDeleteVertexArrays( 1, &VAO_id );
        }
        VAO_id = 0;


        vector_delete( vertices );
        vertices = nullptr;
        if( vertices_id )
        {
            glDeleteBuffers( 1, &vertices_id );
        }
        vertices_id = 0;

        vector_delete( indices );
        indices = nullptr;
        if( indices_id )
        {
            glDeleteBuffers( 1, &indices_id );
        }
        indices_id = 0;

        vector_delete( items );

        format.clear();
        state = 0;
    }

    CountType FVertexBuffer::getSize() const {
        return (CountType)ftgl::vector_size( items );
    }

    Str FVertexBuffer::ToString() const {
        int i = 0;
        static Str gltypes[9] = {
                "GL_BOOL",
                "GL_BYTE",
                "GL_UNSIGNED_BYTE",
                "GL_SHORT",
                "GL_UNSIGNED_SHORT",
                "GL_INT",
                "GL_UNSIGNED_INT",
                "GL_FLOAT",
                "GL_VOID"
        };

        Str str = ToStr(vector_size( vertices )) + " vertices, " + ToStr(vector_size( indices )) + " indices\n";

        while( attributes[i] )
        {
            int j;
            switch( attributes[i]->type )
            {
                case GL_BOOL:           j=0; break;
                case GL_BYTE:           j=1; break;
                case GL_UNSIGNED_BYTE:  j=2; break;
                case GL_SHORT:          j=3; break;
                case GL_UNSIGNED_SHORT: j=4; break;
                case GL_INT:            j=5; break;
                case GL_UNSIGNED_INT:   j=6; break;
                case GL_FLOAT:          j=7; break;
                default:                j=8; break;
            }

            str += Str(attributes[i]->name) + " : " + ToStr(attributes[i]->size) + "x" + gltypes[j]
                    + " (+" + ToStr(reinterpret_cast<uintptr_t>(attributes[i]->pointer)) + ")\n";

            i += 1;
        }

        return str;
    }

    void FVertexBuffer::upload() {
        if( state == FROZEN ) return;

        if( !vertices_id ) glGenBuffers( 1, &vertices_id );
        if( !indices_id )  glGenBuffers( 1, &indices_id );

        fix vsize = vertices->size*vertices->item_size;
        fix isize = indices->size*indices->item_size;


        // Always upload vertices first such that indices do not point to non-existing
        // data (if we get interrupted in between for example).

        // Upload vertices
        glBindBuffer( GL_ARRAY_BUFFER, vertices_id );
        if( vsize != GPU_vsize )
        {
            glBufferData( GL_ARRAY_BUFFER, (GLsizeiptr)vsize, vertices->items, GL_DYNAMIC_DRAW );
            GPU_vsize = vsize;
        }
        else
        {
            glBufferSubData( GL_ARRAY_BUFFER, 0, (GLsizeiptr)vsize, vertices->items );
        }
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

        // Upload indices
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indices_id );
        if( isize != GPU_isize )
        {
            glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                          (GLsizeiptr)isize, indices->items, GL_DYNAMIC_DRAW );
            GPU_isize = isize;
        }
        else
        {
            glBufferSubData( GL_ELEMENT_ARRAY_BUFFER,
                             0, (GLsizeiptr)isize, indices->items );
        }
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

    void FVertexBuffer::clear() {
        state = FROZEN;
        vector_clear( indices );
        vector_clear( vertices );
        vector_clear( items );
        state = DIRTY;
    }

    void FVertexBuffer::renderSetup(GLenum primitive) {
        // Unbind so no existing VAO-state is overwritten,
        // (e.g. the GL_ELEMENT_ARRAY_BUFFER-binding).
        glBindVertexArray( 0 );

        if( state != CLEAN )
        {
            upload();
            state = CLEAN;
        }

        if( VAO_id == 0 )
        {
            // Generate and set up VAO

            glGenVertexArrays( 1, &VAO_id );
            glBindVertexArray( VAO_id );

            glBindBuffer( GL_ARRAY_BUFFER, vertices_id );

            for(auto attribute : attributes)
            {
                if( attribute == nullptr ) continue;
                else vertex_attribute_enable( attribute );
            }

            glBindBuffer( GL_ARRAY_BUFFER, 0 );

            if( indices->size )
                glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indices_id );
        }

        // Bind VAO for drawing
        glBindVertexArray( VAO_id );

        this->primitive = primitive;
    }

    void FVertexBuffer::renderFinish() { glBindVertexArray( 0 ); }

    void FVertexBuffer::renderItem(size_t index) {
        auto * item = (ftgl::ivec4 *) vector_get( items, index );
        assert( index < vector_size( items ) );

        if( indices->size )
        {
            size_t start = item->istart;
            size_t count = item->icount;
            glDrawElements(primitive, (GLsizei)count, GL_UNSIGNED_INT, (void *)(start * sizeof(GLuint)) );
        }
        else if( vertices->size )
        {
            size_t start = item->vstart;
            size_t count = item->vcount;
            glDrawArrays(primitive, (GLint)(start * vertices->item_size), (GLsizei)count);
        }
    }

    void FVertexBuffer::Render(GLenum primitive) {
        size_t vcount = vertices->size;
        size_t icount = indices->size;

        renderSetup( primitive );
        if( icount )
        {
            glDrawElements( primitive, (GLsizei)icount, GL_UNSIGNED_INT, 0 );
        }
        else
        {
            glDrawArrays( primitive, 0, (GLsizei)vcount );
        }
        renderFinish();
    }

    void FVertexBuffer::pushBackIndices(const GLuint *_indices, const size_t icount) {
        state |= DIRTY;
        ftgl::vector_push_back_data( indices, _indices, icount );
    }

    void FVertexBuffer::pushBackVertices(const void *_vertices, const size_t vcount) {
        state |= DIRTY;
        ftgl::vector_push_back_data( vertices, _vertices, vcount );
    }

    void FVertexBuffer::insertIndices(const size_t _index, const GLuint *_indices, const size_t count) {
        assert( indices );
        assert( _index < indices->size+1 );

        state |= DIRTY;
        vector_insert_data( indices, _index, _indices, count );
    }

    void FVertexBuffer::insertVertices(const size_t index, const void *_vertices, const size_t vcount) {
        size_t i;
        assert( this->vertices );
        assert( index < this->vertices->size+1 );

        this->state |= DIRTY;

        for( i=0; i<this->indices->size; ++i )
        {
            if( *(GLuint *)(vector_get( this->indices, i )) > index )
            {
                *(GLuint *)(vector_get( this->indices, i )) += index;
            }
        }

        vector_insert_data( this->vertices, index, _vertices, vcount );
    }

    void FVertexBuffer::eraseIndices(const size_t first, const size_t last) {
        assert( this->indices );
        assert( first < this->indices->size );
        assert( (last) <= this->indices->size );

        this->state |= DIRTY;
        vector_erase_range( this->indices, first, last );
    }

    void FVertexBuffer::eraseVertices(const size_t first, const size_t last) {
        size_t i;
        assert( this->vertices );
        assert( first < this->vertices->size );
        assert( last <= this->vertices->size );
        assert( last > first );

        this->state |= DIRTY;
        for( i=0; i<this->indices->size; ++i )
        {
            if( *(GLuint *)(vector_get( this->indices, i )) > first )
            {
                *(GLuint *)(vector_get( this->indices, i )) -= (last-first);
            }
        }
        vector_erase_range( this->vertices, first, last );
    }

    size_t FVertexBuffer::insert(const size_t index, const void *_vertices,
                                const size_t vcount, const GLuint *_indices,
                                const size_t icount) {
        size_t vstart, istart, i;
        ftgl::ivec4 item;
        assert( this->vertices );
        assert( this->indices );

        this->state = FROZEN;

        // Push back vertices
        vstart = vector_size( this->vertices );
        pushBackVertices( _vertices, vcount );

        // Push back indices
        istart = vector_size( this->indices );
        pushBackIndices( _indices, icount );

        // Update indices within the vertex buffer
        for( i=0; i<icount; ++i )
        {
            *(GLuint *)(vector_get( this->indices, istart+i )) += vstart;
        }

        // Insert item
        item.x = (int)vstart;
        item.y = (int)vcount;
        item.z = (int)istart;
        item.w = (int)icount;
        vector_insert( this->items, index, &item );

        this->state = DIRTY;
        return index;
    }

    size_t FVertexBuffer::pushBack(const void *_vertices, const size_t vcount,
                                const GLuint *_indices, const size_t icount) {

        return insert( vector_size( items ), _vertices, vcount, _indices, icount );
    }

    void FVertexBuffer::erase(const size_t index) {
        ftgl::ivec4 * item;
        int vstart;
        size_t vcount, istart, icount, i;

        assert( index < vector_size( this->items ) );

        item = (ftgl::ivec4 *) vector_get( this->items, index );
        vstart = item->vstart;
        vcount = item->vcount;
        istart = item->istart;
        icount = item->icount;

        // Update items
        for( i=0; i<vector_size(this->items); ++i )
        {
            auto * currItem = (ftgl::ivec4 *) vector_get( this->items, i );
            if( currItem->vstart > vstart)
            {
                currItem->vstart -= (int)vcount;
                currItem->istart -= (int)icount;
            }
        }

        this->state = FROZEN;
        eraseIndices( istart, istart+icount );
        eraseVertices( vstart, vstart+vcount );
        vector_erase( this->items, index );
        this->state = DIRTY;
    }


} // OpenGL