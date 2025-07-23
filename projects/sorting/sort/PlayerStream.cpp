#include "PlayerStream.hpp"

/**
* @brief The interface for a PlayerStream created using the contents of a vector.
*
* @example Let's cover a brief example:
* Given a vector of Player objects v = {
* Player("Rykard", 23),
* Player("Malenia", 99)
* }
*
* Our stream's behavior would be someting akin to:
*
* stream.remaining() -> 2
* stream.nextPlayer() -> Player("Rykard", 23)
* stream.nextPlayer() -> Player("Malenia", 99)
* stream.remaining() -> 0
* stream.nextPlayer() -> throws std::runtime_error()
*/

/**
 * @brief Constructs a VectorPlayerStream from a vector of Players.
 *
 * Initializes the stream with a sequence of Player objects matching the
 * contents of the given vector.
 *
 * @param players The vector of Player objects to stream.
 */
VectorPlayerStream::VectorPlayerStream(const std::vector<Player>& players) : players_(players), current_(0) {}

/**
* @brief Retrieves the next Player in the stream.
*
* @return The next Player object in the sequence.
* @post Updates members so a subsequent call to nextPlayer() yields the Player
* following that which is returned.

* @throws std::runtime_error If there are no more players remaining in the stream.
*/
Player VectorPlayerStream::nextPlayer() {
    if (remaining() > 0) {
        current_++;
        return players_[current_-1];
    }

    // if not more players left
    throw std::runtime_error("No more players left.");
}

/**
* @brief Returns the number of players remaining in the stream.
*
* @return The count of players left to be read.
*/
size_t VectorPlayerStream::remaining() const {
    return players_.size() - current_;
}