#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>

const unsigned int MAX_COMPONENTS = 32;

/// @brief Signature, we use a bitset (1s and 0s) to keep track of which components an entity has,
/// and also helps keep track of which entities a system is interested in.
typedef std::bitset<MAX_COMPONENTS> Signature;

class Component{};

class Entity{
    private:
        int id;

    public:
        Entity(int id): id(id){};
        int GetId() const;
};

/// @brief The System processes entities that contain a specific signature
class System{
    private:
        Signature componentSignature;
        std::vector<Entity> entities;

    public:
        System() = default;
        virtual ~System() = default;

        void AddEntityToSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        std::vector<Entity> GetSystemEntities() const;
        Signature& GetComponentSignature() const;

        // Define the component type T that entities must have to be considered by the system
};

class Registry{};

#endif /** ECS_H */