#ifndef _SCENE_H_
#define _SCENE_H_

struct Scene {
    /* this contains the data structure of different scenes */
    void* scene;
    /* constructor */
    void* (*Constructor)(void);
    /* destructor */
    void (*Destructor)(void*);
    /* handle key */
    void (*HandleKey)(void* _scene, void* _event);
    /* update */
    void (*Update)(void*);
    /* render */
    void (*Render)(void*);
};
typedef struct Scene Scene;

#define SceneSetConstructor(_scene, func) ((_scene)->Constructor = (func))
#define SceneSetDestructor(_scene, func) ((_scene)->Destructor = (func))
#define SceneSetHandleKey(_scene, func) ((_scene)->HandleKey = (func))
#define SceneSetUpdate(_scene, func) ((_scene)->Update = (func))
#define SceneSetRender(_scene, func) ((_scene)->Render = (func))

#define SceneConstruct(_scene) ((_scene)->scene = (_scene)->Constructor())
#define SceneDestruct(_scene) ((_scene)->Destructor((_scene)->scene))
#define SceneHandleKey(_scene, _event) ((_scene)->HandleKey((_scene)->scene, (_event)))
#define SceneUpdate(_scene) ((_scene)->Update((_scene)->scene))
#define SceneRender(_scene) ((_scene)->Render((_scene)->scene))

void InitScene(
    Scene* scene,
    void* (*cons)(void),
    void (*dest)(void*),
    void (*hand)(void*, void*),
    void (*update)(void*),
    void (*rend)(void*)
);

#endif