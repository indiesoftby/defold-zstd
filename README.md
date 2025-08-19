[![Zstd Cover](cover.jpg)](https://github.com/indiesoftby/defold-zstd)

# Zstandard Compression for Defold

This is a native extension for [the Defold game engine](https://defold.com/) that allows you to use Zstandard to compress and decompress strings in your Lua code. The included version of Zstd is 1.5.7.

> [Zstandard](https://github.com/facebook/zstd), or `zstd` as short version, is a fast lossless compression algorithm, targeting real-time compression scenarios at zlib-level and better compression ratios.

Currently, the extension has simple API with two functions: `compress` and `decompress`. It doesn't support streaming, dictionaries and other features of the Zstd library. If you need more features, you can fill an issue on GitHub or even better - submit a PR.

## Usage

First add this extension as a dependency to your `game.project`:

    https://github.com/indiesoftby/defold-zstd/archive/main.zip

It makes available global Lua functions `zstd.*`. Then you can use the extension in your Lua code:

```lua
local compression_level = 3 -- default is 3, maximum and the slowest compression level is 22
local compressed = zstd.compress("Hello, world!", compression_level)
local decompressed = zstd.decompress(compressed)

print(decompressed)
```

## API

### `zstd.version()`
Returns the version of the Zstandard library.

**Returns:**
- `string`: The version.

### `zstd.compress(data, level)`
Compresses a string.

**Parameters:**
- `data` (string): The data to compress.
- `level` (number, optional): Optional compression level (default is 3, maximum is 22)

**Returns:**
- `string`: The compressed data

### `zstd.decompress(data)`
Decompresses a string.

**Parameters:**
- `data` (string): The data to decompress.

**Returns:**
- `string`: The decompressed data.

## Maintainer Notes

The Zstd library is assembled into a single file `zstd.c` using the script `./build/single_file_libs/create_single_file_library.sh` that comes with the library's source code. Also, to work around build issues on Android due to the qsort_r function, the dictBuilder/* lines are commented out in the `zstd-in.c` file.

## License

This project is licensed under the BSD 3-Clause License. See the LICENSE and LICENSE.zstd files for details.
