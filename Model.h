#ifndef GAMAGORAGL_MODEL_H
#define GAMAGORAGL_MODEL_H

#include <assimp/scene.h>
#include "Shader.h"
#include "Mesh.h"

class Model {
public:
    Model(const std::string &path) {
        loadModel(path);
    }

    void Draw(Shader shader);

private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> texturesLoaded;

    void loadModel(const std::string &path);

    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat,
                                              aiTextureType type,
                                              std::string typeName
    );

    static unsigned int TextureFromFile(
            const char *path, const std::string &directory
    );
};


#endif //GAMAGORAGL_MODEL_H
