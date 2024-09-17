# Zstandard Compression for Defold

This is a native extension for [the Defold game engine](https://defold.com/) that allows you to use Zstandard to compress and decompress strings in your Lua code. The included version of Zstd is 1.5.6.

Currently, the extension has simple API with two functions: `compress` and `decompress`. It doesn't support streaming, dictionaries and other features of the Zstd library. If you need more features, you can fill an issue on GitHub or even better - submit a PR.

## Usage

First add this extension as a dependency to your `game.project`:

    https://github.com/indiesoftby/defold-zstd/archive/main.zip

Then you can use the extension in your Lua code:

```lua
local zstd = require "zstd"

local compression_level = 3 -- maximum and the slowest compression level is 22
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

## License

This project is licensed under the BSD 3-Clause License. See the LICENSE file for details.
