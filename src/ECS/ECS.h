#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <vector>
#include <algorithm>

/// @brief Constant defining the maximum number of supported components
const unsigned int MAX_COMPONENTS = 32;

/// @brief Signature, we use a bitset (1s and 0s) to keep track of which components an entity has,
/// and also helps keep track of which entities a system is interested in.
typedef std::bitset<MAX_COMPONENTS> Signature;

/// @brief Abstract base class for components
/// Provides a way to manage unique IDs for component types
struct IComponent
{
protected:
    static int nextId;
};

/// @brief Template class to create components with unique IDs
/// @tparam T Component type
template <typename T>
class Component : public IComponent
{
    /// @brief Gets a unique ID for component type T
    /// @return Unique component ID
    static int GetId()
    {
        static auto id = nextId++;
        return id;
    }
};

/// @brief Represents an entity in the ECS system
/// An entity is basically an ID that can have associated components
class Entity
{
private:
    int id;

public:
    Entity(int id) : id(id) {};
    Entity(const Entity &entity) = default;

    int GetId() const;

    Entity &operator=(const Entity &other) = default;
    bool operator==(const Entity &other) const { return id == other.id; }
    bool operator!=(const Entity &other) const { return id != other.id; }
    bool operator>(const Entity &other) const { return id > other.id; }
    bool operator<(const Entity &other) const { return id < other.id; }
};

/// @brief System that processes entities with a specific component signature
/// Systems define which components are required to process entities
class System
{
private:
    Signature componentSignature;
    std::vector<Entity> entities;

public:
    System() = default;
    virtual ~System() = default;

    /// @brief Adds an entity to the system
    /// @param entity Pointer to the entity to be added
    void AddEntityToSystem(Entity entity);

    /// @brief Removes an entity from the system
    /// @param entity Pointer to the entity to be removed
    void RemoveEntityFromSystem(Entity entity);

    /// @brief Gets all entities managed by this system
    /// @return Vector of entities in the system
    std::vector<Entity> GetSystemEntities() const;

    /// @brief Gets the component signature of the system
    /// @return Reference to the component signature
    const Signature &GetComponentSignature() const;

    /// @brief Defines which component types are required for an entity
    /// to be processed by this system
    /// @tparam TComponent Type of the required component
    template <typename TComponent>
    void RequireComponent();
};

/// @brief Central registry that manages entities, components, and systems
/// Will be responsible for creating, destroying, and managing the lifecycle of entities and components
class Registry
{
};

/// @brief Implementation of the RequireComponent method
/// Adds the component ID to the system's signature
/// @tparam TComponent Component type
template <typename TComponent>
void System::RequireComponent()
{
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

#endif /** ECS_H */