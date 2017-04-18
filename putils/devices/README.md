# devices
Simple and re-usable C++ abstractions for physical devices


## NOTE
These abstractions don't actually capture input by themselves. They are meant as a bridge between graphics libraries which capture input and client code that wishes to be notified for these changes.

## Keyboard
An abstraction to a keyboard. Each key is an observable object that will notify its observers when it changes state (pressed or unpressed).
A public `Key` enum is available, which contains a set of default keys (feel free to add whatever other keys you need).

### Methods

##### `addObserver(Key key, std::function<void()> observer)`
Add an observer to a specific key.

##### `bool isKeyPressed(Key key)`
Lets observers know what new state a key is in.

##### `setKeyPressed(Key key, bool pressed)`
Change a key's state and notify its observers.

## Mouse
An abstraction to a mouse. Each button is an observable object that will notify its observers when it changes state (pressed or unpressed).
The mouse itself may also be observed, and will notify its observers whenever a button is clicked. The last click coordinates  can then be recovered using `getCoordinates()`.
A public `Button` enum is available, which contains a set of default buttons (feel free to add whatever other buttons you need).

### Methods

##### `addObserver(Button button, std::function<void()> observer)`
Add an observer to a specific button.

##### `addObserver(std::function<void()> observer)`
Add an observer to the mouse, which will be notified at each click.

##### `bool isKeyPressed()`
Lets observers know what state a button is in`

##### `setKeyPressed(Button button, bool pressed)`
Change a button's state and notify its observers.

##### `click(std::pair<size_t, size_t> coordinates)`
Indicate the mouse has been clicked at `coordinates`, and notify observers.

##### `std::pair<size_t, size_t> getCoordinates()`
Lets observers get the last click coordinates.

##### `setScreenSize(std::pair<size_t, size_t> size)`
Indicate the screen size (so that users can find out the mouse's position relative to its coordinates).

##### `std::pair<size_t, size_> getScreenSize()`
Lets observers know the size of the screen.
