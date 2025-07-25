#pragma once

#include "Compare.hpp"
#include "Item.hpp"
#include <algorithm>
#include <type_traits>
#include <unordered_set>
#include <vector>

// template <>
// struct std::hash<Item> {
//     /**
//      * @brief Computes a hash value for an Item based on
//      * using the standard hash for strings on the Item's name
//      *
//      * @param i The Item to hash
//      * @return Hash value for the Item
//      */
//     size_t operator()(const Item& i) const;
// };

template <class Comparator, class Container = std::vector<Item>>
class Inventory {
private:
    Container items_;

protected:
    // A pointer to a dynamically allocated Item outside of the Player's bag
    Item* equipped_;

    // The total weight of all items in `inventory_grid_`
    float weight_;

public:
    /**
     * @brief Default constructor for the Inventory template class.
     *
     * Initializes an empty inventory with no items, no equipped item,
     * and zero total weight.
     *
     * @tparam Comparator The comparison class for querying items
     * @tparam Container The container type used to store inventory items
     */
    Inventory();

    /**
     * @brief Retrieves the value stored in `equipped_`
     * @return The Item pointer stored in `equipped_`
     */
    Item* getEquipped() const;

    /**
     * @brief Equips a new item.
     * @param itemToEquip A pointer to the item to equip.
     * @post Updates `equipped` to the specified item
     * without deallocating the original.
     */
    void equip(Item* itemToEquip);

    /**
     * @brief Discards the currently equipped item.
     * @post Deallocates the item pointed to by `equipped`
     * and sets `equipped` to nullptr, if `equipped` is not nullptr already.
     */
    void discardEquipped();

    /**
     * @brief Retrieves the value stored in `weight_`
     * @return The float value stored in `weight_`
     */
    float getWeight() const;

    /**
     * @brief Retrieves the value stored in `item_count_`
     * @return The size_t value stored in `item_count_`
     */
    size_t size() const;

    /**
     * @brief Retrieves a copy of the container holding inventory items.
     *
     * @return Container of items in the inventory
     */
    Container getItems() const;

    /**
     * @brief Attempts to add a new item to the inventory.
     *
     * @param target Item to be added to the inventory
     * @return true if the item was successfully added, false if an item
     *         with the same name already exists
     */
    bool pickup(const Item& target);

    /**
     * @brief Attempts to remove an item from the inventory by name.
     *
     * @param name Name of the item to be removed
     * @return true if the item was successfully removed, false if the
     *         item was not found in the inventory
     */
    bool discard(const std::string& itemName);

    /**
     * @brief Checks if an item with the given name exists in the inventory.
     *
     * @param name Name of the item to search for
     * @return true if the item exists in the inventory, false otherwise
     */
    bool contains(const std::string& itemName) const;

    /**
     * @brief Queries the inventory for items within a specified range.
     *
     * Returns a set of items that fall between the start and end items
     * according to the specified Comparator (inclusive on both ends)
     *
     * @param start An Item whose compared property is the lower bound of the query range
     * @param end An Item whose compared property is the upper bound of the query range
     * @return std::unordered_set of items within the specified range
     *
     * @note Returns an empty set if the end item is less than the start item
     * @example To select all Items with weights 0.4 to 10.9, we'd setup the class & parameters as such:
     *  - this Inventory object is of type Inventory<CompareItemWeight>
     *  - start = Item("some_name", 0.4, ItemType::None)
     *  - end = = Item("some_other_name", 10.9, ItemType::None)
     *
     */
    std::unordered_set<Item> query(const Item& start, const Item& end) const;

    /**
     * @brief Destructor for the Inventory class.
     * @post Deallocates any dynamically allocated resources.
     */
    ~Inventory();
};

#include "Inventory.cpp"