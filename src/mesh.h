#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
};

struct Material
{
    std::string name;
    glm::vec3 ambient;  // Ka
    glm::vec3 diffuse;  // Kd
    glm::vec3 specular; // Ks
    float shininess;    // Ns
};

struct SubMesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;
    unsigned int VAO, VBO, EBO;
};

class Mesh
{
public:
    std::vector<SubMesh> submeshes;

    Mesh(const char *filepath)
    {
        std::string objPath(filepath);
        std::string mtlPath = objPath.substr(0, objPath.find_last_of('.')) + ".mtl";

        loadMTL(mtlPath.c_str());
        loadOBJ(filepath);
        setupMeshes();
    }

    void Draw(class Shader &shader)
    {
        for (auto &submesh : submeshes)
        {
            // Aplicar material
            shader.setVec3("material.ambient", submesh.material.ambient);
            shader.setVec3("material.diffuse", submesh.material.diffuse);
            shader.setVec3("material.specular", submesh.material.specular);
            shader.setFloat("material.shininess", submesh.material.shininess);

            glBindVertexArray(submesh.VAO);
            glDrawElements(GL_TRIANGLES, submesh.indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

private:
    std::map<std::string, Material> materials;
    Material defaultMaterial;

    void loadMTL(const char *filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            std::cout << "WARNING: MTL file not found: " << filepath << std::endl;
            std::cout << "Using default materials..." << std::endl;
            defaultMaterial = {
                "default",
                glm::vec3(0.2f, 0.15f, 0.1f), // ambient
                glm::vec3(0.5f, 0.35f, 0.2f), // diffuse
                glm::vec3(0.3f, 0.3f, 0.3f),  // specular
                32.0f                         // shininess
            };
            return;
        }

        std::string line;
        Material currentMaterial;
        currentMaterial.shininess = 32.0f;
        currentMaterial.ambient = glm::vec3(0.2f);
        currentMaterial.diffuse = glm::vec3(0.8f);
        currentMaterial.specular = glm::vec3(0.5f);

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "newmtl")
            {
                if (!currentMaterial.name.empty())
                {
                    materials[currentMaterial.name] = currentMaterial;
                }
                iss >> currentMaterial.name;
            }
            else if (prefix == "Ka")
            {
                iss >> currentMaterial.ambient.r >> currentMaterial.ambient.g >> currentMaterial.ambient.b;
            }
            else if (prefix == "Kd")
            {
                iss >> currentMaterial.diffuse.r >> currentMaterial.diffuse.g >> currentMaterial.diffuse.b;
            }
            else if (prefix == "Ks")
            {
                iss >> currentMaterial.specular.r >> currentMaterial.specular.g >> currentMaterial.specular.b;
            }
            else if (prefix == "Ns")
            {
                iss >> currentMaterial.shininess;
            }
        }

        if (!currentMaterial.name.empty())
        {
            materials[currentMaterial.name] = currentMaterial;
        }

        file.close();
        std::cout << "Loaded " << materials.size() << " materials from MTL" << std::endl;
    }

    void loadOBJ(const char *filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            std::cout << "ERROR::MESH::FILE_NOT_FOUND: " << filepath << std::endl;
            return;
        }

        std::vector<glm::vec3> temp_positions;
        std::vector<glm::vec3> temp_normals;

        SubMesh currentSubmesh;
        currentSubmesh.material = defaultMaterial;
        std::string currentMaterialName = "";

        std::vector<unsigned int> position_indices;
        std::vector<unsigned int> normal_indices;

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty())
                continue;
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v")
            {
                glm::vec3 pos;
                iss >> pos.x >> pos.y >> pos.z;
                temp_positions.push_back(pos);
            }
            else if (prefix == "vn")
            {
                glm::vec3 n;
                iss >> n.x >> n.y >> n.z;
                temp_normals.push_back(glm::normalize(n));
            }
            else if (prefix == "usemtl")
            {
                // fecha o submesh anterior
                if (!position_indices.empty())
                {
                    buildSubmesh(currentSubmesh, temp_positions, temp_normals,
                                 position_indices, normal_indices);
                    submeshes.push_back(currentSubmesh);
                    currentSubmesh = SubMesh{};
                    currentSubmesh.material = defaultMaterial;
                    position_indices.clear();
                    normal_indices.clear();
                }
                iss >> currentMaterialName;
                auto it = materials.find(currentMaterialName);
                currentSubmesh.material = (it != materials.end()) ? it->second : defaultMaterial;
            }
            else if (prefix == "f")
            {
                // Lê todos os vértices dad face
                std::vector<std::string> vtokens;
                std::string tok;
                while (iss >> tok)
                    vtokens.push_back(tok);
                if (vtokens.size() < 3)
                    continue;

                // converter cada token "v/vt/vn" -> índices
                struct Idx
                {
                    unsigned int p, n;
                };
                std::vector<Idx> verts;
                verts.reserve(vtokens.size());
                for (auto &vs : vtokens)
                {
                    unsigned int p = 0, n = 0;
                    parseVertex(vs, p, n);
                    verts.push_back({p, n});
                }

                // Triangulação em Leque: (v0, v[i], v[i+1])
                for (size_t i = 1; i + 1 < verts.size(); ++i)
                {
                    position_indices.push_back(verts[0].p);
                    position_indices.push_back(verts[i].p);
                    position_indices.push_back(verts[i + 1].p);

                    normal_indices.push_back(verts[0].n);
                    normal_indices.push_back(verts[i].n);
                    normal_indices.push_back(verts[i + 1].n);
                }
            }
        }

        // guarda último submesh
        if (!position_indices.empty())
        {
            buildSubmesh(currentSubmesh, temp_positions, temp_normals,
                         position_indices, normal_indices);
            submeshes.push_back(currentSubmesh);
        }

        file.close();
    }

    void buildSubmesh(SubMesh &submesh,
                      const std::vector<glm::vec3> &temp_positions,
                      std::vector<glm::vec3> &temp_normals,
                      const std::vector<unsigned int> &position_indices,
                      const std::vector<unsigned int> &normal_indices)
    {

        if (temp_normals.empty())
        {
            std::cout << "Calculating smooth normals..." << std::endl;
            temp_normals.resize(temp_positions.size(), glm::vec3(0.0f));
            std::vector<int> normalCount(temp_positions.size(), 0);

            for (size_t i = 0; i < position_indices.size(); i += 3)
            {
                unsigned int idx0 = position_indices[i] - 1;
                unsigned int idx1 = position_indices[i + 1] - 1;
                unsigned int idx2 = position_indices[i + 2] - 1;

                glm::vec3 v0 = temp_positions[idx0];
                glm::vec3 v1 = temp_positions[idx1];
                glm::vec3 v2 = temp_positions[idx2];

                glm::vec3 edge1 = v1 - v0;
                glm::vec3 edge2 = v2 - v0;
                glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

                // Adicionar a normal com o peso baseado na área do triângulo
                float area = glm::length(glm::cross(edge1, edge2)) * 0.5f;

                temp_normals[idx0] += normal * area;
                temp_normals[idx1] += normal * area;
                temp_normals[idx2] += normal * area;

                normalCount[idx0]++;
                normalCount[idx1]++;
                normalCount[idx2]++;
            }

            // Normalizar com suavização
            for (size_t i = 0; i < temp_normals.size(); i++)
            {
                if (normalCount[i] > 0 && glm::length(temp_normals[i]) > 0.001f)
                {
                    temp_normals[i] = glm::normalize(temp_normals[i]);
                }
                else
                {
                    temp_normals[i] = glm::vec3(0.0f, 1.0f, 0.0f); // fallback
                }
            }
        }

        for (size_t i = 0; i < position_indices.size(); i++)
        {
            Vertex vertex;
            vertex.Position = temp_positions[position_indices[i] - 1];
            vertex.Normal = normal_indices[i] > 0 ? temp_normals[normal_indices[i] - 1] : temp_normals[position_indices[i] - 1];
            submesh.vertices.push_back(vertex);
            submesh.indices.push_back(i);
        }
    }

    void parseVertex(const std::string &vertexStr, unsigned int &posIdx, unsigned int &normIdx)
    {
        std::istringstream iss(vertexStr);
        std::string index;

        std::getline(iss, index, '/');
        posIdx = std::stoi(index);

        std::getline(iss, index, '/');

        if (std::getline(iss, index, '/'))
        {
            normIdx = index.empty() ? 0 : std::stoi(index);
        }
        else
        {
            normIdx = 0;
        }
    }

    void setupMeshes()
    {
        for (auto &submesh : submeshes)
        {
            glGenVertexArrays(1, &submesh.VAO);
            glGenBuffers(1, &submesh.VBO);
            glGenBuffers(1, &submesh.EBO);

            glBindVertexArray(submesh.VAO);

            glBindBuffer(GL_ARRAY_BUFFER, submesh.VBO);
            glBufferData(GL_ARRAY_BUFFER, submesh.vertices.size() * sizeof(Vertex),
                         &submesh.vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, submesh.indices.size() * sizeof(unsigned int),
                         &submesh.indices[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)offsetof(Vertex, Normal));

            glBindVertexArray(0);
        }
    }
};

#endif