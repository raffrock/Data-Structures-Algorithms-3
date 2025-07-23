/** 
*@file Player.cpp 
* Rafael Chetata
* 02/10/24
*/

/**
 * Addendum 1:
 * In Inventory.hpp, line 38 uses the constructor, std::vector(10, std::vector<Item>(10, Item{}))
 * By the C++ standard, this is a valid constructor, and it works on Gradescope and on other C++17 compilers we've tested.
 * If it's generating an error for you anyway, you may edit the file to explicitly declare the value type as follows:
std::vector<std::vector<Item>>(10, std::vector<Item>(10, Item{}))
 * 
 * Note 2: Code that attempts to dereference a null pointer will compile just fine, 
 * but may get runtime errors or simply give the wrong output. 
 * And it will certainly fail any well-written unit tests.
 */

#include "Inventory.hpp"
#include <stdexcept>
#include <vector> 

/**
* @brief Constructor with optional parameters for initialization.
* @param items A const reference to a 2D vector of items. 
*  Defaults to a 10x10 grid of default-constructed items, if none provided.
* @param equipped A pointer to an Item object. 
*  Defaults to nullptr, if none provided.
* 
* @post Initializes members in the following way:
* 1) Initializes `weight_` as the total weight of all items in `items` (excluding NONE type) 
* 2) Initialies `item_count_` as the count of non-NONE items. 
* 
* NOTE: The `equipped` item is excluded from these calculations.
*/
Inventory::Inventory(const std::vector<std::vector<Item>>& items, Item* equipped) : inventory_grid_(items) {
    if (equipped != nullptr && equipped->type_ != NONE) {
        equipped_ = equipped;
    } else {
        equipped_ = nullptr;
    }

    weight_ = 0;
    item_count_ = 0;

    // iterates through items to update total weight_ and item_count
    for (auto item_row : items) {
        for (auto item : item_row) {
            if (item.type_ != NONE) {
                weight_ += item.weight_;
                item_count_++;
            } 
        }
    }
}

/** 
 * @brief Retrieves the value stored in `equipped_`
 * @return The Item pointer stored in `equipped_`
 */
Item* Inventory::getEquipped() const {
    return equipped_;
}

/**
 * @brief Equips a new item.
 * @param itemToEquip A pointer to the item to equip.
 * @post Updates `equipped` to the specified item 
 * without deallocating the original.
 */
void Inventory::equip(Item* itemToEquip) {
    // if (equipped_ != nullptr) {
    //     delete equipped_;
    // }
    equipped_ = new Item;
    *equipped_ = *itemToEquip;
}

/**
 * @brief Discards the currently equipped item.
 * @post Deallocates the item pointed to by `equipped` 
 * and sets `equipped` to nullptr, if `equipped` is not nullptr already.
 */
void Inventory::discardEquipped() {
    if (equipped_ != nullptr) {
        delete equipped_;
        equipped_ = nullptr;        
    }
}

/** 
 * @brief Retrieves the value stored in `inventory_grid_`
 * @return The vector<vector<Item>> value stored in `inventory_grid_`
 */
std::vector<std::vector<Item>> Inventory::getItems() const {
    return inventory_grid_;
}

/** 
 * @brief Retrieves the value stored in `weight_`
 * @return The float value stored in `weight_`
 */
float Inventory::getWeight() const {
    return weight_;
}

/**
 * @brief Retrieves the value stored in `item_count_`
 * @return The size_t value stored in `item_count_`
 */
size_t Inventory::getCount() const {
    return item_count_;
}

/**
 * @brief Retrieves the item located at the specified row and column
 * in the inventory grid.
 *
 * @param row A size_t parameter for the row index in the inventory grid.
 * @param col A size_t parameter for the column index in the inventory grid.
 * @return The item at the specified row and column.
 * @throws std::out_of_range If the row or column is out of bounds.
 */
Item Inventory::at(const size_t& row, const size_t& col) const {
    // if invalid number for row or col
    if ((row >= inventory_grid_.size() || row < 0) || ((col >= inventory_grid_[row].size()) || col < 0)) {
        throw std::out_of_range("Row or Column out of range");
    }
    return inventory_grid_[row][col];
}

/**
 * @brief Stores an item at the specified row and column in the inventory grid.
 *
 * @param row A size_t parameter for the row index in the inventory grid.
 * @param col  A size_t parameter for the column index in the inventory grid.
 * @param pickup A const ref. to the item to store at the specified location.
 * @return True if the item was successfully stored, false if the cell is already occupied.
 * 
 * @post Updates `item_count_` and `weight_` if the Item is sucessfully added
 * @throws std::out_of_range If the row or column is out of bounds.
 */
bool Inventory::store(const size_t& row, const size_t& col, const Item& pickup) {
    // check row, then col.
    // only throw exception
    if ((row >= inventory_grid_.size() || row < 0) || ((col >= inventory_grid_[row].size()) || col < 0)) {
        throw std::out_of_range("Row or Column out of range");
    }

    if (inventory_grid_[row][col].type_ == NONE) {
        // throw std::out_of_range("Row or Column out of range");
        inventory_grid_[row][col] = pickup; 
        item_count_++;
        weight_ += pickup.weight_;
        return true;
    }
    return false;
}

// Big Five

/**
 * @brief Copy constructor for the Inventory class.
 * @param rhs A const l-value ref. to the Inventory object to copy.
 * @post Creates a deep copy of `rhs`, 
 *  including duplicating the dynamically 
 *  allocated item in `equipped`.
 */
Inventory::Inventory(const Inventory& rhs) {
    if (rhs.equipped_ != nullptr) {
        // equipped_ = new Item(*rhs.equipped_);
        equipped_ = new Item;
        *equipped_ = *rhs.equipped_;
    } else {
        equipped_ = nullptr;
    }

    inventory_grid_ = rhs.inventory_grid_;
    weight_ = rhs.weight_;
    item_count_ = rhs.item_count_;
}

/**
 * @brief Move constructor for the Inventory class.
 * @param rhs An r-value ref. to the Inventory object to move from.
 * @post Transfers ownership of resources from `rhs` 
 * to the newly constructed Inventory object. 
 * 
 * Sets `rhs` to a valid but empty state.
 * - All pointers are set to nullptr
 * - All numerical values are set to 0
 * - All containers are cleared to have size 0
 * 
 * 
 * Pointers to dynamically allocated memory (i.e. resources in the
 * heap) are copied then deleted from the source, effectively
 * transferring ownership of the resource it points to.
 * 
 */
Inventory::Inventory(Inventory && rhs) {
    inventory_grid_ = std::move(rhs.inventory_grid_);

    if (rhs.equipped_ != nullptr) {
        equipped_ = rhs.equipped_;
        rhs.equipped_ = nullptr;
    } else {
        equipped_ = nullptr;
    }
    weight_ = std::move(rhs.weight_);
    item_count_ = std::move(rhs.item_count_);

    rhs.inventory_grid_.clear();
    rhs.weight_ = 0.0;
    rhs.item_count_ = 0;
}

/**
 * @brief Copy assignment operator for the Inventory class.
 * @param rhs A const l-value ref. to the Inventory object to copy.
 * @return A reference to the updated Inventory object.
 * @post Performs a deep copy of `rhs`, including 
 * re-allocating and copying the item in `equipped`.
 * 
 * NOTE: The resources of the overridden object
 * should be destroyed.
 */
Inventory& Inventory::operator=(const Inventory& rhs) {
    if (this != &rhs) {

        inventory_grid_ = rhs.inventory_grid_;

        discardEquipped();
        if (rhs.equipped_ != nullptr) {
            equipped_ = new Item;
            *equipped_ = *rhs.equipped_;
        }
        
        weight_ = rhs.weight_;
        item_count_ = rhs.item_count_;
    }
    return *this;
}

/**
 * @brief Move assignment operator for the Inventory class.
 * @param rhs An r-value ref. to the Inventory object to move from.
 * @return A reference to the updated Inventory object.
 * @post Transfers ownership of resources from `rhs` 
 * to the newly constructed Inventory object. 
 * 
 * Sets `rhs` to a valid but empty state.
 * - All pointers are set to nullptr
 * - All numerical values are set to 0
 * - All containers are cleared to have size 0
 * 
 * NOTE: The resources of the overridden object
 * should be destroyed.
 */
Inventory& Inventory::operator=(Inventory&& rhs) {

    if (this != &rhs) {
        inventory_grid_ = std::move(rhs.inventory_grid_);

        discardEquipped();
        if (rhs.equipped_ != nullptr) {
            equipped_ = rhs.equipped_;
            rhs.equipped_ = nullptr;
        }

        weight_ = rhs.weight_;
        item_count_ = rhs.item_count_;

        // reset rhs
        rhs.inventory_grid_.clear();
        rhs.weight_ = 0.0;
        rhs.item_count_ = 0;
    }

    // return ref to updated obj
    return *this;
}

/**
 * @brief Destructor for the Inventory class.
 * @post Deallocates any dynamically allocated resources.
 */
Inventory::~Inventory() {
    discardEquipped();
}
    
