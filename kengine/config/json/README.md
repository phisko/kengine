# kengine_config_json

System that saves and loads config values to and from a JSON file.

Values can be overridden through the command-line by using [JSON pointer syntax](https://www.rfc-editor.org/rfc/rfc6901) with the `--config:` prefix:
```
--config:'/Log/File/severity_control/global_severity="very_verbose"'
```

The `--dump-config` command-line option can be used to log the complete configuration once it's been loaded.

* [systems](systems)
	* [system](systems/system.md)
