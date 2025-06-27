# GCC Function Inspector Plugin

This GCC plugin allows developers to **interactively inspect and modify function-specific optimizations** during the compilation process. It prompts the user at plugin initialization to specify a function name of interest. When GCC encounters that function during parsing, an interactive menu is launched, providing access to internal GCC structures and the ability to tweak optimizations on a per-function basis.

