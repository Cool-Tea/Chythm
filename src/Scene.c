#include "Scene.h"

void InitScene(
    Scene* scene,
    void* (*cons)(void),
    void (*dest)(void*),
    void (*hand)(void*, void*),
    void (*update)(void*),
    void (*rend)(void*)
) {
    SceneSetConstructor(scene, cons);
    SceneSetDestructor(scene, dest);
    SceneSetHandleKey(scene, hand);
    SceneSetUpdate(scene, update);
    SceneSetRender(scene, rend);
    SceneConstruct(scene);
}