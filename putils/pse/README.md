# pse
Phiste's SFML Engine, a simple graphics engine built over SFML

## Engine
Main class, abstraction to a window. Manages `ViewItems`.

### Methods

##### `update(bool clear = true)`
Update the engine (draw ViewItems and move them towards their destination). If `clear` is set to false, the screen will not be cleared of the last image.

## ViewItem
Base class managed by `Engine`. All drawable items inherit from this.
Allows manipulation of the drawable's size and position

### Methods

##### `goTo(const sf::Vector2f &destination, double time = 0)`
Move to `destination` over the next `time` seconds.

## Grid
Allows for simple placement of `ViewItems` in a grid format.

## Sprite
`ViewItem` class to draw a static image.

## AnimatedSprite
`ViewItem` class to draw an animated image.

## CompositeAnimatedSprite
`ViewItem` class to draw several, simultaneously animated images.

## Text
`ViewItem` class to draw text.

## Shape
`ViewItem` class to draw any sf::Shape.
