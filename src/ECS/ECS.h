/// @file ECS.h
/// @brief Entity Component System Core Implementation
/// @details Defines Entity, Component, System, and Registry classes

#ifndef ECS_H
#define ECS_H

#include "../Logger/Logger.h"
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <deque>
#include <memory>
#include <algorithm>

const unsigned int MAX_COMPONENTS = 32;

////////////////////////////////////////////////////////////////////////////////
// Signature
////////////////////////////////////////////////////////////////////////////////
// We use a bitset (1s and 0s) to keep track of which components an entity has,
// and also which components a system is interested in.
typedef std::bitset<MAX_COMPONENTS> Signature;

////////////////////////////////////////////////////////////////////////////////
// Entity
////////////////////////////////////////////////////////////////////////////////
class Entity {
private:
    int id;
public:
    Entity(int id) : id(id) {};
    Entity(const Entity& entity) = default;
    int GetId() const { return id; };

    // Operator Overloading
    bool operator ==(const Entity& other) const { return id == other.id; }
    bool operator !=(const Entity& other) const { return id != other.id; }
    bool operator >(const Entity& other) const { return id > other.id; }
    bool operator <(const Entity& other) const { return id < other.id; }

    // Forward declaration for template methods
    template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
    template <typename TComponent> void RemoveComponent();
    template <typename TComponent> bool HasComponent() const;
    template <typename TComponent> TComponent& GetComponent();

    // Group & Tag management
    void Kill();
    void Tag(const std::string& tag);
    bool HasTag(const std::string& tag) const;
    void Group(const std::string& group);
    bool BelongsToGroup(const std::string& group) const;

    class Registry* registry;
};

////////////////////////////////////////////////////////////////////////////////
// Component
////////////////////////////////////////////////////////////////////////////////
struct IComponent {
protected:
    static int nextId;
};

// Used to assign a unique ID to a component type
template <typename T>
class Component : public IComponent {
public:
    // Returns the unique ID of Component<T>
    static int GetId() {
        static auto id = nextId++;
        return id;
    }
};

////////////////////////////////////////////////////////////////////////////////
// System
////////////////////////////////////////////////////////////////////////////////
class System {
private:
    Signature componentSignature;
    std::vector<Entity> entities;
public:
    System() = default;
    ~System() = default;

    void AddEntityToSystem(Entity entity);
    void RemoveEntityFromSystem(Entity entity);
    std::vector<Entity> GetSystemEntities() const;
    const Signature& GetComponentSignature() const;

    // Defines the component type that entities must have to be added to the system
    template <typename TComponent> void RequireComponent();
};

////////////////////////////////////////////////////////////////////////////////
// Pool
////////////////////////////////////////////////////////////////////////////////
class IPool {
public:
    virtual ~IPool() = default;
    virtual void RemoveEntityFromPool(int entityId) = 0;
};

template <typename T>
class Pool : public IPool {
private:
    std::vector<T> data;
    int size;
    std::unordered_map<int, int> entityIdToIndex;
    std::unordered_map<int, int> indexToEntityId;

public:
    Pool(int capacity = 100) {
        size = 0;
        data.resize(capacity);
    }

    virtual ~Pool() = default;

    bool IsEmpty() const { return size == 0; }
    int GetSize() const { return size; }

    void Resize(int n) {
        data.resize(n);
    }

    void Clear() {
        data.clear();
        size = 0;
        entityIdToIndex.clear();
        indexToEntityId.clear();
    }

    void Add(T object) {
        data.push_back(object);
    }

    void Set(int entityId, T object) {
        if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
            // If the element already exists, simply replace the component object
            int index = entityIdToIndex[entityId];
            data[index] = object;
        } else {
            // When adding a new object, we keep track of the entity IDs and their vector index
            int index = size;
            entityIdToIndex.emplace(entityId, index);
            indexToEntityId.emplace(index, entityId);
            if (index >= data.capacity()) {
                data.resize(size * 2);
            }
            data[index] = object;
            size++;
        }
    }

    void Remove(int entityId) {
        // Copy the last element to the deleted position to keep the vector packed
        int indexOfRemoved = entityIdToIndex[entityId];
        int indexOfLast = size - 1;
        data[indexOfRemoved] = data[indexOfLast];

        // Update the index-entity maps to point to the correct elements
        int entityIdOfLastElement = indexToEntityId[indexOfLast];
        entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
        indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

        entityIdToIndex.erase(entityId);
        indexToEntityId.erase(indexOfLast);

        size--;
    }

    void RemoveEntityFromPool(int entityId) override {
        if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
            Remove(entityId);
        }
    }

    T& Get(int entityId) {
        int index = entityIdToIndex[entityId];
        return data[index];
    }

    T& operator [](unsigned int index) {
        return data[index];
    }
};

////////////////////////////////////////////////////////////////////////////////
// Registry
////////////////////////////////////////////////////////////////////////////////
class Registry {
private:
    int numEntities = 0;
    std::set<Entity> entitiesToBeAdded;
    std::set<Entity> entitiesToBeKilled;
    
    // MASTER LIST OF ENTITIES (Exposed to Editor)
    std::set<Entity> activeEntities;

    // Component pools
    std::vector<std::shared_ptr<IPool>> componentPools;
    std::vector<Signature> entityComponentSignatures;
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

    // Tags and Groups
    std::unordered_map<std::string, Entity> entityPerTag;
    std::unordered_map<int, std::string> tagPerEntity;
    std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
    std::unordered_map<int, std::string> groupPerEntity;

    std::deque<int> freeIds;

public:
    Registry() { Logger::Log("Registry constructor called!"); }
    ~Registry() { Logger::Log("Registry destructor called!"); }

    void Update();

    // Entity Management
    Entity CreateEntity();
    void KillEntity(Entity entity);
    
    // API Exposed to Editor
    const std::set<Entity>& GetEntities() const;
    const Signature& GetEntitySignature(Entity entity) const;

    // Tag & Group Management
    void TagEntity(Entity entity, const std::string& tag);
    bool EntityHasTag(Entity entity, const std::string& tag) const;
    Entity GetEntityByTag(const std::string& tag) const;
    void RemoveEntityTag(Entity entity);

    void GroupEntity(Entity entity, const std::string& group);
    bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
    std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
    void RemoveEntityGroup(Entity entity);

    // Component Management
    template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
    template <typename TComponent> void RemoveComponent(Entity entity);
    template <typename TComponent> bool HasComponent(Entity entity) const;
    template <typename TComponent> TComponent& GetComponent(Entity entity) const;

    // System Management
    template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
    template <typename TSystem> void RemoveSystem();
    template <typename TSystem> bool HasSystem() const;
    template <typename TSystem> TSystem& GetSystem() const;

    // Add entity to systems checks
    void AddEntityToSystems(Entity entity);
    void RemoveEntityFromSystems(Entity entity);
};

// TEMPLATE IMPLEMENTATIONS

template <typename TComponent>
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    if (componentId >= static_cast<int>(componentPools.size())) {
        componentPools.resize(componentId + 1, nullptr);
    }

    if (!componentPools[componentId]) {
        std::shared_ptr<Pool<TComponent>> newPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newPool;
    }

    std::shared_ptr<Pool<TComponent>> pool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    TComponent newComponent(std::forward<TArgs>(args)...);
    pool->Set(entityId, newComponent);

    entityComponentSignatures[entityId].set(componentId);

    //Logger::Log("Component added to entity id = " + std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    std::shared_ptr<Pool<TComponent>> pool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    pool->Remove(entityId);

    entityComponentSignatures[entityId].set(componentId, false);
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    std::shared_ptr<Pool<TComponent>> pool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return pool->Get(entityId);
}

template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args) {
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem() {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args) {
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent() {
    registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const {
    return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() {
    return registry->GetComponent<TComponent>(*this);
}

#endif // ECS_H