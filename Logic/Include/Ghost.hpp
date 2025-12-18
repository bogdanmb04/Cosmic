/**
 * @file Ghost.hpp
 * @brief Concrete implementation of a Ghost entity.
 * @details This class acts as the "Context" in the Strategy Pattern, delegating the
 * movement logic to a specific IGhostStrategy implementation (Red, Pink, Blue, or Orange).
 */

#pragma once
#include "IGhost.hpp"
#include "IGhostStrategy.hpp"
#include "GameTypes/GhostType.hpp"
#include <memory>

namespace Pacman
{
	/**
	 * @class Ghost
	 * @brief Represents a single ghost entity in the game.
	 * @details Inherits from IGhost interface and manages the ghost's identity (Type) and AI Strategy.
	 */
	class Ghost : public IGhost
	{
	public:
		/**
		 * @brief Constructor used to inject dependencies.
		 * @param ghostStrategy The specific AI strategy (movement logic) for this ghost.
		 * @param type The visual identity (color) of the ghost.
		 */
		Ghost(std::unique_ptr<IGhostStrategy> ghostStrategy, GhostType type);

		/**
		 * @brief Calculates the next target tile during Chase mode.
		 * @details Delegates the calculation to the injected strategy.
		 */
		virtual Vector2 CalculateChaseTarget(const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition) const override;

		/**
		 * @brief Gets the fixed corner target for Scatter mode.
		 */
		virtual Vector2 GetScatterTarget() const override;

		/**
		 * @brief Returns the type (color) of this ghost.
		 */
		virtual GhostType GetGhostType() const override;

	private:
		std::unique_ptr<IGhostStrategy> ghostStrategy_; ///< The AI strategy (Strategy Pattern).
		GhostType type_;                                ///< The ghost identifier (e.g., Red/Blinky).
	};
}