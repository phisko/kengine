# [DialogComponent](DialogComponent.hpp)

`Component` used to display a multiple-choice dialog.

### Nested types

##### Option

```cpp
struct Option {
	std::string text;
	std::function<void()> onClick = nullptr;

	pmeta_get_class_name(DialogComponentOption);
	pmeta_get_attributes(
		pmeta_reflectible_attribute(&Option::text),
		pmeta_reflectible_attribute(&Option::onClick)
	);
	pmeta_get_methods();
	pmeta_get_parents();
};
```

An option in the dialog. Its `text` member is to be displayed, while `onClick` will be called if the user selects the option.

### Members

##### mainText

```cpp
std::string mainText;
```

Text to be displayed for the dialog (e.g. what an NPC is saying).

##### options

```cpp
std::vector<Option> options;
```

The list of possible answers.
