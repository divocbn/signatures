# signatures

A small C++23 tool for generating byte-pattern signatures from  GTA V game builds using memory offsets. 

It is mainly used for FiveM (Cfx.re) related reverse engineering tasks where offsets change between updates but underlying code patterns stay similar.

## How it works

- Loads a specific GTA V build executable
- Reads bytes at a given offset
- Converts those bytes into a signature pattern using wildcards
- Prints the resulting pattern for reuse in other builds

## Example

```
signatures.exe -build 3258 -offset 0x1337
```

Output:

```
pattern: 48 ? 04 ? 49 8B D0 ...
```

## Options

```
-b, --build     Build number
-o, --offset    Target offset (hex)
-d, --download  Optional download all builds
```

## To do

- Patterns still break too easy among builds, need to think of some ways to create them more reliable
- Accept dumps and auto generate patterns for each stacktrace call
