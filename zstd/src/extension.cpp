
#include <dmsdk/sdk.h>
#include <cstring>
#include <zstd.h>

static int Zstd_Compress(lua_State* L)
{
    size_t src_len;
    const char* src       = luaL_checklstring(L, 1, &src_len);
    int compression_level = ZSTD_CLEVEL_DEFAULT;
    if (lua_isnumber(L, 2))
    {
        compression_level = luaL_checkinteger(L, 2);
    }
    size_t dst_len = ZSTD_compressBound(src_len);
    char* dst      = (char*)malloc(dst_len);
    if (!dst)
    {
        return luaL_error(L, "Failed to allocate memory");
    }
    size_t size = ZSTD_compress(dst, dst_len, src, src_len, compression_level);
    if (ZSTD_isError(size))
    {
        free(dst);
        return luaL_error(L, "Failed to compress: %s", ZSTD_getErrorName(size));
    }
    lua_pushlstring(L, dst, size);
    free(dst);
    return 1;
}

static int Zstd_Decompress(lua_State* L)
{
    size_t src_len;
    const char* src = luaL_checklstring(L, 1, &src_len);

    // Check decompressed data size
    unsigned long long content_size = ZSTD_getFrameContentSize(src, src_len);
    if (content_size == ZSTD_CONTENTSIZE_ERROR)
    {
        return luaL_error(L, "Invalid content size");
    }

    // If size is known, use simple decompression
    if (content_size != ZSTD_CONTENTSIZE_UNKNOWN)
    {
        char* dst = (char*)malloc(content_size);
        if (!dst)
        {
            return luaL_error(L, "Failed to allocate memory");
        }

        size_t size = ZSTD_decompress(dst, content_size, src, src_len);
        if (ZSTD_isError(size))
        {
            free(dst);
            return luaL_error(L, "Failed to decompress: %s", ZSTD_getErrorName(size));
        }

        lua_pushlstring(L, dst, size);
        free(dst);
        return 1;
    }

    // If size is unknown, use streaming decompression
    ZSTD_DCtx* dctx = ZSTD_createDCtx();
    if (!dctx)
    {
        return luaL_error(L, "Failed to create decompression context");
    }

    // Initial buffer size
    size_t buffer_size = ZSTD_DStreamOutSize();
    size_t total_size = 0;
    size_t capacity = buffer_size;
    char* result = (char*)malloc(capacity);

    if (!result)
    {
        ZSTD_freeDCtx(dctx);
        return luaL_error(L, "Failed to allocate memory");
    }

    ZSTD_inBuffer input = { src, src_len, 0 };

    while (input.pos < input.size)
    {
        // Check if buffer needs to be expanded
        if (total_size + buffer_size > capacity)
        {
            capacity *= 2;
            char* new_result = (char*)realloc(result, capacity);
            if (!new_result)
            {
                free(result);
                ZSTD_freeDCtx(dctx);
                return luaL_error(L, "Failed to reallocate memory");
            }
            result = new_result;
        }

        ZSTD_outBuffer output = { result + total_size, capacity - total_size, 0 };

        size_t ret = ZSTD_decompressStream(dctx, &output, &input);
        if (ZSTD_isError(ret))
        {
            free(result);
            ZSTD_freeDCtx(dctx);
            return luaL_error(L, "Failed to decompress: %s", ZSTD_getErrorName(ret));
        }

        total_size += output.pos;

        // If ret == 0, decompression is complete
        if (ret == 0)
        {
            break;
        }
    }

    ZSTD_freeDCtx(dctx);

    lua_pushlstring(L, result, total_size);
    free(result);
    return 1;
}

static int Zstd_Version(lua_State* L)
{
    lua_pushstring(L, ZSTD_versionString());
    return 1;
}

// Functions exposed to Lua
static const luaL_reg Ext_methods[] = {
    { "compress", Zstd_Compress },
    { "decompress", Zstd_Decompress },
    { "version", Zstd_Version },

    /* Sentinel: */
    { NULL, NULL }
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, "zstd", Ext_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result InitializeExt(dmExtension::Params* params)
{
    LuaInit(params->m_L);

    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppInitializeExt(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeExt(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeExt(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(zstd, "zstd", AppInitializeExt, AppFinalizeExt, InitializeExt, 0, 0, FinalizeExt)
