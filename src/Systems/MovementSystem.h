#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"

class MovementSystem : public System {

    public:
    MovementSystem(){
        // Todo: 
        // RequireComponent<TransformComponent>();
        // RequireComponent<...>();
    }

    void Update(){
        // Todo:
        // Loop all entities that the system is interested in

        // for(auto entity : GetEntities()){
        //     // Todo:
        //     // Update Entity position based on its velocity 
        // }

    }

};

#endif /** MOVEMENTSYSTEM_H  */
