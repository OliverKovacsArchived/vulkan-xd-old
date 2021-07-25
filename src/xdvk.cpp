// Oliver Kovacs 2021 - xdvk - MIT

#include <iostream>
#include <math.h>
#include "vertex.cpp"

namespace xdvk {

    template<uint32_t D>
    struct Transform {
        float buffer[2 * D + D * (D - 1) / 2];
        float *position = buffer;
        float *scale = &buffer[D];
        float *rotation = &buffer[2 * D];
        
        Transform() {
            std::fill_n(buffer, 2 * D + D * (D - 1) / 2, 0.0f);
            std::fill_n(scale, D, 1.0f);
        };
    };

    struct Geometry {

        std::vector<float> vertices;
        
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexBufferSize;
        bool drawIndexed = true;

        size_t vertexBufferIndex;
        size_t indexBufferIndex;
        size_t transformBufferIndex;
    };

    template<uint32_t D>
    struct Entity {
        uint64_t id;
        uint64_t components;
        Transform<D> transform;
        Geometry geometry;
    };

    struct Index {
        uint64_t id;
        uint32_t index;
        uint32_t next;
    };

    #define INDEX_MASK 0xffffffff
    #define NEW_OBJECT_ID_ADD 0x100000000
    template<uint32_t D>
    class Scene {
        public:
        
        std::vector<Index> indices;
        std::vector<Entity<D>> entities;
        
        Scene(size_t reserve) {
            indices.resize(reserve);
            entities.reserve(reserve);
            entity_count = 0;
            for (size_t i = 0; i < reserve; i++) {
                indices[i].id = i;
                indices[i].next = i + 1;
            }
            freelist = 0;
        };

        bool has(uint64_t id) {
            Index &index = indices[id & INDEX_MASK];
            return index.id == id && index.index != UINT32_MAX;
        }

        Entity<D> &get(uint64_t id) {
            return entities[indices[id & INDEX_MASK].index];
        }

        uint64_t add() {
            Index &index = indices[freelist];
            freelist = index.next;
            index.id += NEW_OBJECT_ID_ADD;
            index.index = entity_count++;
            Entity<D> &entity = entities.emplace_back();
            // Entity<D> &entity = entities[index.index];
            entity.id = index.id;
            return entity.id;
        }

        void remove(uint64_t id) {
            Index &index = indices[id & INDEX_MASK];
            Entity<D> &entity = entities[index.index];
            entity = entities[--entity_count];
            entities.pop_back();
            indices[entity.id & INDEX_MASK].index = index.index;
            index.index = UINT32_MAX;
            index.next = freelist;
            freelist = id & INDEX_MASK;
        }

        private:
        uint32_t entity_count;
        uint32_t freelist;
    };

    void hypercube(std::vector<float> &vertices, std::vector<uint32_t> &indices, uint32_t dimension);
    void hypercubeVertices(std::vector<float> &vertices, uint32_t dimension, float size, uint32_t stride, uint32_t offset);
    void hypercubeIndices(std::vector<uint32_t> &indices, uint32_t dimension, uint32_t stride, uint32_t offset);

    template<uint32_t D>
    void hypercubeTransform(std::vector<float> &buffer, Transform<D> transform, uint32_t index, uint32_t stride, uint32_t offset);

    uint32_t rotationSize(const uint32_t dimension);
    uint32_t transformSize(const uint32_t dimension);

    template<typename T>
    void printVector(std::string name, std::vector<T> vector);
}

namespace xdvk {

    void createHypercubeVertices(std::vector<float> &vertices, const uint32_t dimension, float size) {
        const uint32_t block = 1;
        const uint32_t n1 = pow(2, dimension);
        const uint32_t n2 = dimension;
        vertices.resize(n1 * n2 * block);
        for (size_t i = 0; i < n1; i++) {
            for (size_t j = 0; j < n2; j++) {
                const uint32_t index = (i * n2 + j) * block;
                vertices[index] = size * (1.0 - ((uint32_t)floor(i / pow(2.0, j)) % 2) * 2.0);
            }
        }
    }
    
    void hypercubeVertices(std::vector<float> &vertices, const uint32_t dimension, float size, uint32_t stride, uint32_t offset) {
        const uint32_t block = 1 + stride;
        const uint32_t n1 = pow(2, dimension);
        const uint32_t n2 = dimension;
        vertices.resize(offset + n1 * n2 * block);
        for (size_t i = 0; i < n1; i++) {
            for (size_t j = 0; j < n2; j++) {
                const uint32_t index = offset + (i * n2 + j) * block;
                vertices[index] = size * (1.0 - ((uint32_t)floor(i / pow(2.0, j)) % 2) * 2.0);
            }
        }
    }

    void hypercubeIndices(std::vector<uint32_t> &buffer, const uint32_t dimension, uint32_t stride, uint32_t offset) {
        const uint32_t block = 2 + stride;
        const uint32_t n1 = dimension;
        const uint32_t n2 = pow(2, (dimension - 1));
        buffer.resize(offset + n1 * n2 * block);
        for (size_t i = 0; i < n1; i++) {
            for (size_t j = 0; j < n2; j++) {
                const uint32_t base = (j % (uint32_t)pow(2, i)) + pow(2, (i + 1)) * floor(j / pow(2, i));
                const uint32_t index = offset + (i * n2 + j) * block;
                buffer[index]     = base;
                buffer[index + 1] = base + pow(2, i);
            }
        }
    }

    void hypercubeEdges(std::vector<float> &buffer, uint32_t dimension, uint32_t stride, uint32_t offset) {
        const uint32_t block = 8 + stride;
        const uint32_t n1 = dimension;
        const uint32_t n2 = pow(2, (dimension - 1));
        buffer.resize(offset + n1 * n2 * block);
        for (size_t i = 0; i < n1; i++) {
            for (size_t j = 0; j < n2; j++) {
                const uint32_t index = offset + (i * n2 + j) * block;
                buffer[index]     = index;
                buffer[index + 1] = i;
                buffer[index + 2] = j;
                buffer[index + 3] = 0;
                buffer[index + 4] = index;
                buffer[index + 5] = i;
                buffer[index + 6] = j;
                buffer[index + 7] = 1;
            }          
        }
    }

    void icositetrachoronVertices(std::vector<float> &buffer, float size, uint32_t stride, uint32_t offset) {
        hypercubeVertices(buffer, 4, 0.5 * size, stride, offset);
        const uint32_t block = 1 + stride;
        const uint32_t _offset = 64 * block;
        const uint32_t n1 = 8;
        const uint32_t n2 = 4;
        buffer.resize(offset + _offset + n1 * n2 * block);
        for (size_t i = 0; i < n1; i++) {
            for (size_t j = 0; j < n2; j++) {
                const uint32_t index = offset + _offset + (i * n2 + j) * block;
                buffer[index] = j == floor(i / 2) ? (i % 2 ? size : -size) : 0.0f;
            }
        }
    }

    void icositetrachoronIndices(std::vector<uint32_t> &buffer, uint32_t stride, uint32_t offset) {
        hypercubeIndices(buffer, 4, stride, offset);
        const uint32_t block = 2 + stride;
        const uint32_t _offset = 32 * block;
        const uint32_t n1 = 16;
        const uint32_t n2 = 8;
        buffer.resize(offset + _offset + n1 * n2 * block);
        for (size_t i = 0; i < n1; i++) {
            for (size_t j = 0; j < n2; j++) {
                const uint32_t index = offset + _offset + (i * n2 + j) * block;
                buffer[index]     = i;
                buffer[index + 1] = j + n2;
            }
        } 
    }

    template<uint32_t D>
    void hypercubeTransform(std::vector<float> &buffer, Transform<D> transform, uint32_t index, uint32_t stride, uint32_t offset) {
        uint32_t size = transformSize(D);
        const uint32_t block = size + stride;
        buffer.resize(offset + (index + 1) * block);
        std::copy_n(&transform.buffer[offset + index * stride], size, buffer.begin());
    }

    uint32_t rotationSize(const uint32_t dimension) {
        return dimension * (dimension - 1) / 2;
    }

    uint32_t transformSize(const uint32_t dimension) {
        return 2 * dimension + rotationSize(dimension);
    }

    template<typename T>
    void printVector(std::vector<T> vector, std::string name = "") {
        std::cout << name << "[" << vector.size() << "] = [ ";
        for (auto elem : vector) {
            std::cout << elem << " ";
        }
        std::cout << "]" << std::endl;
    }
}
