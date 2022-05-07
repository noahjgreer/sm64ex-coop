#include "dynos.cpp.h"

void DynOS_Gfx_GeneratePacks(const char* directory) {
    DIR *modsDir = opendir(directory);
    if (!modsDir) { return; }

    struct dirent *dir = NULL;
    while ((dir = readdir(modsDir)) != NULL) {
        // Skip . and ..
        if (SysPath(dir->d_name) == ".") continue;
        if (SysPath(dir->d_name) == "..") continue;

        // If pack folder exists, generate bins
        SysPath _LevelPackFolder = fstring("%s/%s/levels", directory, dir->d_name);
        if (fs_sys_dir_exists(_LevelPackFolder.c_str())) {
            DynOS_Lvl_GeneratePack(_LevelPackFolder);
        }
        SysPath _ActorPackFolder = fstring("%s/%s/actors", directory, dir->d_name);
        if (fs_sys_dir_exists(_ActorPackFolder.c_str())) {
            DynOS_Actor_GeneratePack(_ActorPackFolder);
        }
        SysPath _TexturePackFolder = fstring("%s/%s", directory, dir->d_name);
        SysPath _TexturePackOutputFolder = fstring("%s/%s/textures", directory, dir->d_name);
        if (fs_sys_dir_exists(_TexturePackFolder.c_str())) {
            DynOS_Tex_GeneratePack(_TexturePackFolder, _TexturePackOutputFolder);
        }
    }

    closedir(modsDir);
}

static void ScanPackBins(SysPath aPackFolder) {
    DIR *_PackDir = opendir(aPackFolder.c_str());
    if (!_PackDir) { return; }

    struct dirent *_PackEnt = NULL;
    while ((_PackEnt = readdir(_PackDir)) != NULL) {
        // Skip . and ..
        if (SysPath(_PackEnt->d_name) == ".") continue;
        if (SysPath(_PackEnt->d_name) == "..") continue;

        SysPath _FileName = fstring("%s/%s", aPackFolder.begin(), _PackEnt->d_name);
        s32 length = strlen(_PackEnt->d_name);

        // check for actors
        if (length > 4 && !strncmp(&_PackEnt->d_name[length - 4], ".bin", 4)) {
            String _ActorName = _PackEnt->d_name;
            _ActorName[length - 4] = '\0';
            DynOS_Actor_LoadFromBinary(aPackFolder, strdup(_ActorName.begin()), _FileName, true);
        }

        // check for textures
        if (length > 4 && !strncmp(&_PackEnt->d_name[length - 4], ".tex", 4)) {
            String _TexName = _PackEnt->d_name;
            _TexName[length - 4] = '\0';
            DynOS_Tex_LoadFromBinary(aPackFolder, _FileName, _TexName.begin(), true);
        }
    }
}

static void ScanPacksFolder(SysPath _DynosPacksFolder) {
    DIR *_DynosPacksDir = opendir(_DynosPacksFolder.c_str());
    if (_DynosPacksDir) {
        struct dirent *_DynosPacksEnt = NULL;
        while ((_DynosPacksEnt = readdir(_DynosPacksDir)) != NULL) {

            // Skip . and ..
            if (SysPath(_DynosPacksEnt->d_name) == ".") continue;
            if (SysPath(_DynosPacksEnt->d_name) == "..") continue;

            // If pack folder exists, add it to the pack list
            SysPath _PackFolder = fstring("%s/%s", _DynosPacksFolder.c_str(), _DynosPacksEnt->d_name);
            if (fs_sys_dir_exists(_PackFolder.c_str())) {
                struct PackData* _Pack = DynOS_Pack_Add(_PackFolder);
                DynOS_Actor_GeneratePack(_PackFolder);
                DynOS_Tex_GeneratePack(_PackFolder, _PackFolder);
                ScanPackBins(_PackFolder);
            }
        }
        closedir(_DynosPacksDir);
    }
}

void DynOS_Gfx_Init() {
    // Scan the DynOS packs folder
    SysPath _DynosPacksFolder = fstring("%s/%s", DYNOS_EXE_FOLDER, DYNOS_PACKS_FOLDER);
    ScanPacksFolder(_DynosPacksFolder);

    // Scan the user path folder
    SysPath _DynosPacksUserFolder = fstring("%s/%s", DYNOS_USER_FOLDER, DYNOS_PACKS_FOLDER);
    ScanPacksFolder(_DynosPacksUserFolder);
}
