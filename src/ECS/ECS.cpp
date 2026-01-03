#include "ECS.h"
#include "../Logger/Logger.h"

#include <algorithm>

/// @brief Initialize the static component ID counter
int IComponent::nextId = 0;

/// @brief Gets the unique identifier of this entity
/// @return The entity's ID
/// @details Simple getter that provides access to the private id field
int Entity::GetId() const {
    return id;
}

void Entity::Kill() {
    registry->KillEntity(*this);
}

void Entity::Tag(const std::string &tag) {
    registry->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string &tag) const {
    return registry->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string &group) {
    registry->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const std::string &group) const {
    return registry->EntityBelongsToGroup(*this, group);
}

/// @brief Adds an entity to this system's processing list
/// @param entity Pointer to the entity to be added
/// @details Adds the entity to the vector of entities this system processes.
/// The entity should already have been verified to have the required components.
void System::AddEntityToSystem(Entity entity) {
    entities.push_back(entity);
}

/// @brief Removes an entity from this system's processing list
/// @param entity Pointer to the entity to be removed
/// @details Uses the erase-remove idiom to efficiently remove the entity from
/// the vector. The lambda comparison ensures we remove the correct entity by ID.
void System::RemoveEntityFromSystem(Entity entity) {
    entities.erase(
        std::remove_if(
            entities.begin(),
            entities.end(),
            [&entity](Entity other) { return entity == other; }),
        entities.end());
}

/// @brief Gets all entities currently being processed by this system
/// @return Vector containing all entities in this system
/// @details Returns a copy of the entities vector to prevent external modification
std::vector<Entity> System::GetSystemEntities() const {
    return entities;
}

/// @brief Gets this system's component requirements
/// @return Const reference to the system's component signature
/// @details Returns the signature that specifies which components an entity
/// must have for this system to process it
const Signature &System::GetComponentSignature() const {
    return componentSignature;
}

/// @brief Creates a new entity in the ECS
/// @return The newly created entity
/// @details
/// - Generates a new unique ID for the entity
/// - Creates an Entity object with this ID
/// - Adds the entity to the pending entities list
/// - Logs the creation for debugging
/// - Returns the new entity to the caller
Entity Registry::CreateEntity() {
    long unsigned int entityId;

    if (freeIds.empty()) {
        // If there are no free ids waiting to be reused
        entityId = numEntities++;

        // Make sure the entityComponentSignature vector can accommodate the new entity
        if (entityId >= entityComponentSignatures.size()) {
            entityComponentSignatures.resize(entityId + 1);
        }
    } else {
        // Reuse an id from the list of previously removed entities
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);

    entity.registry = this;

    entitiesToBeAdd.insert(entity);

    Logger::Log("Entity created with id = " + std::to_string(entityId));

    return entity;
}

void Registry::KillEntity(Entity entity) {
    entitiesToBeKilled.insert(entity);
}

/// @brief Updates which systems should process an entity
/// @param entity Pointer to the entity to check
/// @details
/// - Gets the entity's current component signature
/// - Iterates through all registered systems
/// - For each system:
///   - Compares the entity's components with the system's requirements
///   - If the entity has all required components, adds it to that system
/// - The bitwise AND operation checks if all required components are present
void Registry::AddEntityToSystems(Entity entity) {
    const auto entityId = entity.GetId();

    const auto &entityComponentSignature = entityComponentSignatures[entityId];

    // Loop all the systems
    for (auto &system: systems) {
        const auto &systemComponentSignature = system.second->GetComponentSignature();

        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

        if (isInterested) {
            system.second->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity) {
    for (auto system: systems) {
        system.second->RemoveEntityFromSystem(entity);
    }
}

void Registry::TagEntity(Entity entity, const std::string &tag) {
    entityPerTag.emplace(tag, entity);
    tagPerEntity.emplace(entity.GetId(), tag);
}

bool Registry::EntityHasTag(Entity entity, const std::string &tag) const {
    if (tagPerEntity.find(entity.GetId()) == tagPerEntity.end()) {
        return false;
    }
    return entityPerTag.find(tag)->second == entity;
}

Entity Registry::GetEntityByTag(const std::string &tag) const {
    return entityPerTag.at(tag);
}

void Registry::RemoveEntityTag(Entity entity) {
    auto taggedEntity = tagPerEntity.find(entity.GetId());
    if (taggedEntity != tagPerEntity.end()) {
        auto tag = taggedEntity->second;
        entityPerTag.erase(tag);
        tagPerEntity.erase(taggedEntity);
    }
}

void Registry::GroupEntity(Entity entity, const std::string &group) {
    entitiesPerGroup.emplace(group, std::set<Entity>());
    entitiesPerGroup[group].emplace(entity);
    groupPerEntity.emplace(entity.GetId(), group);
}

bool Registry::EntityBelongsToGroup(Entity entity, const std::string &group) const {
    auto groupEntities = entitiesPerGroup.at(group);
    return groupEntities.find(entity.GetId()) != groupEntities.end();
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string &group) const {
    auto &setOfEntities = entitiesPerGroup.at(group);
    return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());
}

void Registry::RemoveEntityGroup(Entity entity) {
    auto groupedEntity = groupPerEntity.find(entity.GetId());
    if (groupedEntity != groupPerEntity.end()) {
        auto group = entitiesPerGroup.find(groupedEntity->second);
        if (group != entitiesPerGroup.end()) {
            auto entityInGroup = group->second.find(entity);
            if (entityInGroup != group->second.end()) {
                group->second.erase(entityInGroup);
            }
        }
        groupPerEntity.erase(groupedEntity);
    }
}

/// @brief Processes pending entity changes
/// @details This method should:
/// 1. Process entities in entitiesToBeAdd:
///    - Initialize their data structures
///    - Add them to relevant systems
/// 2. Process entities in entitiesToBeKilled:
///    - Remove them from all systems
///    - Clean up their components
///    - Free their IDs for reuse
/// @note Currently unimplemented - marked as TODO
void Registry::Update() {
    // Processing the entities that are waiting to be created to the active Systems
    for (auto entity: entitiesToBeAdd) {
        AddEntityToSystems(entity);
    }

    entitiesToBeAdd.clear();

    // Processing the entities that are waiting to be killed from the active Systems
    for (auto entity: entitiesToBeKilled) {
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetId()].reset();

        // Make the entity id avaliable to reused
        freeIds.push_back(entity.GetId());

        RemoveEntityTag(entity);
        RemoveEntityGroup(entity);
    }
    entitiesToBeKilled.clear();
}
