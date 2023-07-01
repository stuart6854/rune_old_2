CPMAddPackage("gh:jarro2783/cxxopts#v3.1.1")
CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage(
        NAME assimp
        GITHUB_REPOSITORY assimp/assimp
        GIT_TAG v5.2.5
        OPTIONS
        "BUILD_SHARED_LIBS OFF"
        "ASSIMP_NO_EXPORT ON"
        "ASSIMP_BUILD_TESTS OFF"
        "ASSIMP_INSTALL OFF"
        "ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF"
        "ASSIMP_BUILD_OBJ_IMPORTER ON"
        "ASSIMP_BUILD_GLTF_IMPORTER ON"
        "ASSIMP_BUILD_FBX_IMPORTER ON"
)