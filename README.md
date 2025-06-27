# GCC Function-Specific Optimization Plugin

This GCC plugin allows developers to **interactively inspect and modify function-specific optimizations** during the compilation process. It prompts the user at plugin initialization to specify a function name of interest. When GCC encounters that function during parsing, an interactive menu is launched, providing access to internal GCC structures and the ability to tweak optimizations on a per-function basis.

## GCC Version Compatibility

This plugin was implemented and tested with **GCC 13**.  
Compatibility with other versions is not guaranteed and may require adjustments to internal APIs or plugin hooks.

## Requirements

```shell
sudo apt install gcc-13 g++-13 gcc-13-plugin-dev cmake build-essential
```

## Usage

Use the plugin by passing it to gcc with `-fplugin` flag:
```shell
gcc -fplugin=./func-spec-opt.so your_source_file.c
```

