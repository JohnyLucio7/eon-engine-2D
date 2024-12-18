#ifndef ECS_H
#define ECS_H

#include "../Logger/Logger.h"

#include <bitset>
#include <vector>
#include <set>
#include <deque>
#include <unordered_map>
#include <typeindex>
#include <memory>

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
public:
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
    /// Constructor that initializes an entity with an ID
    /// @param id Unique identifier for the entity
    Entity(int id) : id(id) {};

    /// Default copy constructor
    /// @param entity Entity to be copied
    Entity(const Entity &entity) = default;

    void Kill();

    /// Gets the entity's ID
    /// @return Entity's ID
    int GetId() const;

    /// Default assignment operator
    /// @param other Entity to be assigned
    /// @return Reference to the current entity
    Entity &operator=(const Entity &other) = default;

    /// Compares if two entities are equal based on their IDs
    /// @param other Entity to compare with
    /// @return true if IDs are equal, false otherwise
    bool operator==(const Entity &other) const { return id == other.id; }

    /// Compares if two entities are different based on their IDs
    /// @param other Entity to compare with
    /// @return true if IDs are different, false otherwise
    bool operator!=(const Entity &other) const { return id != other.id; }

    /// Compares if current entity's ID is greater than another's
    /// @param other Entity to compare with
    /// @return true if current ID is greater, false otherwise
    bool operator>(const Entity &other) const { return id > other.id; }

    /// Compares if current entity's ID is less than another's
    /// @param other Entity to compare with
    /// @return true if current ID is less, false otherwise
    bool operator<(const Entity &other) const { return id < other.id; }

    /// Adds a component to the entity
    /// @tparam TComponent Type of component to be added
    /// @tparam TArgs Types of arguments for component construction
    /// @param args Arguments for component construction
    template <typename TComponent, typename... TArgs>
    void AddComponent(TArgs &&...args);

    /// Removes a component from the entity
    /// @tparam TComponent Type of component to be removed
    template <typename TComponent>
    void RemoveComponent();

    /// Checks if the entity has a specific component
    /// @tparam TComponent Type of component to check for
    /// @return true if component exists, false otherwise
    template <typename TComponent>
    bool HasComponent() const;

    /// Gets a component from the entity
    /// @tparam TComponent Type of component to get
    /// @return Reference to the component
    template <typename TComponent>
    TComponent &GetComponent() const;

    /// Pointer to the registry that manages entities
    class Registry *registry;
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

/// @brief Interface for component pools
/// Allows for type-safe component storage and retrieval
class IPool
{
public:
    virtual ~IPool() {}
};

/// @brief Type-safe pool for storing components of a specific type
/// Provides contiguous memory storage for better cache performance
/// @tparam T Component type stored in this pool
template <typename T>
class Pool : public IPool
{
private:
    /// @brief Vector storing the actual component data
    std::vector<T> data;

public:
    /// @brief Constructor that pre-allocates space for components
    /// @param size Initial size of the pool
    Pool(int size = 100)
    {
        data.resize(size);
    }

    virtual ~Pool() = default;

    /// @brief Checks if the pool is empty
    /// @return True if pool contains no elements
    bool isEmpty() const
    {
        return data.empty();
    }

    /// @brief Gets the current size of the pool
    /// @return Number of elements the pool can hold
    int GetSize() const
    {
        return data.size();
    }

    /// @brief Resizes the pool to hold a specific number of elements
    /// @param n New size for the pool
    void Resize(int n)
    {
        data.resize(n);
    }

    /// @brief Removes all elements from the pool
    void Clear()
    {
        data.clear();
    }

    /// @brief Adds a new object to the pool
    /// @param object Object to be added
    void Add(T object)
    {
        data.push_back(object);
    }

    /// @brief Sets an object at a specific index in the pool
    /// @param index Position where to set the object
    /// @param object Object to be set
    void Set(int index, T object)
    {
        data[index] = object;
    }

    /// @brief Gets an object from a specific index in the pool
    /// @param index Position of the object to retrieve
    /// @return Reference to the requested object
    T &Get(int index)
    {
        return static_cast<T &>(data[index]);
    }

    /// @brief Array access operator
    /// @param index Position of the object to retrieve
    /// @return Reference to the requested object
    T &operator[](unsigned int index)
    {
        return data[index];
    }
};

/// @brief Central registry that manages entities, components, and systems
/// Will be responsible for creating, destroying, and managing the lifecycle of entities and components
class Registry
{
private:
    /// @brief Counter for the number of entities created
    int numEntities = 0;

    // Vector of component pools, each pool contains all the data for a certain component type
    // [Vector index = component type id]
    // [Pool index = entity id]
    std::vector<std::shared_ptr<IPool>> componentPools;

    // Vector of component signatures per entity, saying which component is turned "on" for a given entity
    // [Vector index = entity id]
    std::vector<Signature> entityComponentSignatures;

    /// @brief Map of systems indexed by their type
    /// Stores all registered systems in the ECS
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

    /// @brief Sets of entities pending addition or removal
    /// Used to manage entity lifecycle during updates
    std::set<Entity> entitiesToBeAdd;
    std::set<Entity> entitiesToBeKilled;

    /// @brief List of free entity ids that were previously removed
    std::deque<int> freeIds;

public:
    Registry()
    {
        Logger::Log("Registry constructor called!");
    }

    ~Registry()
    {
        Logger::Log("Registry destructor called!");
    }

    /// @brief Updates the registry state
    /// Processes pending entity additions and removals
    void Update();

    /// @brief Creates a new entity in the system
    /// @return Newly created entity
    Entity CreateEntity();

    void KillEntity(Entity entity);

    /// @brief Check the component signature of a entity and add the entity to the systems
    /// that are interested in it
    /// @param entity Entity to be processed
    void AddEntityToSystems(Entity entity);

    void RemoveEntityFromSystems(Entity entity);

    /// @brief Adds a component to an entity
    /// @tparam TComponent Type of component to add
    /// @tparam TArgs Types of arguments for component construction
    /// @param entity Entity to receive the component
    /// @param args Arguments for component construction
    template <typename TComponent, typename... TArgs>
    void AddComponent(Entity entity, TArgs &&...args);

    /// @brief Removes a component from an entity
    /// @tparam TComponent Type of component to be removed from the entity
    /// @param entity Pointer to the entity from which to remove the component
    /// @details This method updates the entity's component signature by setting the corresponding
    /// component bit to false, effectively marking that component as no longer present on the entity.
    /// Note that this only updates the signature and does not actually delete the component data
    /// from the pool - the data slot is simply marked as available for reuse.
    template <typename TComponent>
    void RemoveComponent(Entity entity);

    /// @brief Checks if an entity has a specific component
    /// @tparam TComponent Type of component to check for
    /// @param entity Pointer to the entity to check
    /// @return true if the entity has the component, false otherwise
    /// @details Uses the entity's component signature to efficiently check for the presence
    /// of a component without actually accessing the component pool. This is done by testing
    /// the bit corresponding to the component type in the entity's signature.
    template <typename TComponent>
    bool HasComponent(Entity entity) const;

    template <typename TComponent>
    TComponent &GetComponent(Entity entity) const;

    template <typename TSystem, typename... TArgs>
    void AddSystem(TArgs &&...args);

    template <typename TSystem>
    void RemoveSystem();

    template <typename TSystem>
    bool HasSystem() const;

    template <typename TSystem>
    TSystem &GetSystem() const;

    // TODO: Implement the following methods:
    // - KillEntity: Remove an entity from the system
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

/// @brief Implementation of AddComponent method
/// Adds a component of type TComponent to the specified entity
/// @tparam TComponent Type of component to add
/// @tparam TArgs Types of arguments for component construction
/// @param entity Entity to receive the component
/// @param args Arguments for component construction
template <typename TComponent, typename... TArgs>
void Registry::AddComponent(Entity entity, TArgs &&...args)
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    // Resize component pools if necessary
    if ((long unsigned int)componentId >= componentPools.size())
    {
        componentPools.resize(componentId + 1, nullptr);
    }

    // Create new pool for this component type if it doesn't exist
    if (!componentPools[componentId])
    {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    // Resize the pool if necessary to accommodate the new entity
    if (entityId >= componentPool->GetSize())
    {
        componentPool->Resize(numEntities);
    }

    // Create and add the new component
    TComponent newComponent(std::forward<TArgs>(args)...);
    componentPool->Set(entityId, newComponent);

    // Update the entity's component signature
    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

/// @brief Implementation of RemoveComponent
/// @tparam TComponent The type of component to remove
/// @param entity Pointer to the entity from which to remove the component
template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    // Update the entity's signature to indicate it no longer has this component
    // The second parameter 'false' explicitly sets the bit to 0
    entityComponentSignatures[entityId].set(componentId, false);

    Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
}

/// @brief Implementation of HasComponent
/// @tparam TComponent The type of component to check for
/// @param entity Pointer to the entity to check
/// @return Boolean indicating whether the entity has the component
template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    // Test if the bit corresponding to this component type is set in the entity's signature
    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent &Registry::GetComponent(Entity entity) const
{
    const auto componentId = Component<TComponent>::GetId();

    const auto entityId = entity.GetId();

    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    return componentPool->Get(entityId);
}

/// @brief Adds a system to the registry
/// @tparam TSystem Type of system to add
/// @tparam TArgs System constructor argument types
/// @param args System constructor arguments
template <typename TSystem, typename... TArgs>
void Registry::AddSystem(TArgs &&...args)
{
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);

    systems.insert(
        std::make_pair(
            std::type_index(typeid(TSystem)),
            newSystem));
}

/// @brief Removes a system from the registry
/// @tparam TSystem Type of system to remove
template <typename TSystem>
void Registry::RemoveSystem()
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

/// @brief Checks if a system exists in the registry
/// @tparam TSystem Type of system to check for
/// @return True if system exists
template <typename TSystem>
bool Registry::HasSystem() const
{
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

/// @brief Gets a reference to a system
/// @tparam TSystem Type of system to retrieve
/// @return Reference to the requested system
template <typename TSystem>
TSystem &Registry::GetSystem() const
{
    auto system = systems.find(std::type_index(typeid(TSystem)));

    return *(std::static_pointer_cast<TSystem>(system->second));
}

/// @brief Implementation of component addition
/// Delegates component addition to the registry
template <typename TComponent, typename... TArgs>
void Entity::AddComponent(TArgs &&...args)
{
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

/// @brief Implementation of component removal
/// Delegates component removal to the registry
template <typename TComponent>
void Entity::RemoveComponent()
{
    registry->RemoveComponent<TComponent>(*this);
}

/// @brief Implementation of component verification
/// Delegates verification to the registry
template <typename TComponent>
bool Entity::HasComponent() const
{
    return registry->HasComponent<TComponent>(*this);
}

/// @brief Implementation of component retrieval
/// Delegates component retrieval to the registry
template <typename TComponent>
TComponent &Entity::GetComponent() const
{
    return registry->GetComponent<TComponent>(*this);
}

#endif /** ECS_H */