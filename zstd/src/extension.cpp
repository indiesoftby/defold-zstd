
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
    size_t dst_len  = ZSTD_getFrameContentSize(src, src_len);
    if (dst_len == ZSTD_CONTENTSIZE_ERROR || dst_len == ZSTD_CONTENTSIZE_UNKNOWN)
    {
        return luaL_error(L, "Invalid content size");
    }
    char* dst = (char*)malloc(dst_len);
    if (!dst)
    {
        return luaL_error(L, "Failed to allocate memory");
    }
    size_t size = ZSTD_decompress(dst, dst_len, src, src_len);
    if (ZSTD_isError(size))
    {
        free(dst);
        return luaL_error(L, "Failed to decompress: %s", ZSTD_getErrorName(size));
    }
    lua_pushlstring(L, dst, size);
    free(dst);
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
