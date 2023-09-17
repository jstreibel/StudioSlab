//
// Created by joao on 16/09/23.
//
/**
   Essentially an re-implementation of vertex-buffer.h from
   Nicolas Rougier (Nicolas.Rougier@inria.fr)
   April, 2012
 */


#ifndef STUDIOSLAB_VERTEXBUFFER_H
#define STUDIOSLAB_VERTEXBUFFER_H

#include "rougier/opengl.h"
#include "rougier/vector.h"
#include "rougier/vertex-attribute.h"
#include "Utils/Types.h"

namespace OpenGL {

    class VertexBuffer {
/** Format of the vertex buffer. */
        Str format;

        /** Vector of vertices. */
        ftgl::vector_t * vertices;

        // MODERN OPENGL
        /** GL identity of the Vertex Array Object */
        GLuint VAO_id;

        /** GL identity of the vertices buffer. */
        GLuint vertices_id;

        /** Vector of indices. */
        ftgl::vector_t * indices;

        /** GL identity of the indices buffer. */
        GLuint indices_id;

        /** Current size of the vertices buffer in GPU */
        size_t GPU_vsize;

        /** Current size of the indices buffer in GPU*/
        size_t GPU_isize;

        /** GL primitives to render. */
        GLenum primitive;

        /** Whether the vertex buffer needs to be uploaded to GPU memory. */
        char state;

        /** Individual items */
        ftgl::vector_t * items;

        /** Array of attributes. */
        ftgl::vertex_attribute_t *attributes[MAX_VERTEX_ATTRIBUTE]{};

        void renderSetup(GLenum mode);

        static void renderFinish();

        void renderItem(size_t index);

        void pushBackIndices(const GLuint * indices, size_t icount);

        void pushBackVertices(const void * vertices, size_t vcount);

        void insertIndices(size_t index, const GLuint *indices, size_t count);

        void insertVertices(size_t index, const void *vertices, size_t vcount);

        void eraseIndices(size_t first, size_t last);

        void eraseVertices(size_t first, size_t last);

        void upload();

    public:
        /**
         * Creates an empty vertex buffer.
         *
         * @param  format a string describing vertex format.
         * @return        an empty vertex buffer.
         */
        explicit VertexBuffer(Str format);

        /**
         * Deletes vertex buffer and releases GPU memory.
         *
         * @param  self  a vertex buffer
         */
        ~VertexBuffer();

        Count getSize() const;

        Str ToString() const;

        void clear();

        void render(GLenum mode);

        size_t insert(size_t index,
                      const void * vertices, size_t vcount,
                      const GLuint * indices, size_t icount);

        size_t pushBack(const void * vertices, size_t vcount, const GLuint * indices, size_t icount);

        void erase(size_t index);
    };

} // OpenGL

#endif //STUDIOSLAB_VERTEXBUFFER_H
