/* 
 * libnpengine: Nitroplus script interpreter
 * Copyright (C) 2013-2014 Mislav Blažević <krofnica996@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * */
#ifndef RESOURCE_MGR_HPP
#define RESOURCE_MGR_HPP

#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <cstring>
#include <glib.h>
#include "inpafile.hpp"

class ScriptFile;

struct MapDeleter
{
    template <class T> void operator() (T Data) { delete Data.second; }
};

template <class T>
struct CacheHolder
{
    static void Clear()
    {
        std::for_each(Cache.begin(), Cache.end(), MapDeleter());
        Cache.clear();
    }

    static T* Read(const std::string& Path)
    {
        auto iter = Cache.find(Path);
        if (iter != Cache.end())
            return iter->second;
        return nullptr;
    }

    static void Write(const std::string& Path, T* Data)
    {
        Cache[Path] = Data;
    }

    static std::map<std::string, T*> Cache;
};

template <class T> std::map<std::string, T*> CacheHolder<T>::Cache;

class Resource
{
public:
    Resource(INpaFile* pArchive, INpaFile::NpaIterator File) : pArchive(pArchive), File(File) { }

    bool IsValid() { return pArchive != nullptr; }
    uint32_t GetSize() { return pArchive->GetFileSize(File); }
    char* ReadData(uint32_t Offset, uint32_t Size) { return pArchive->ReadFile(File, Offset, Size, g_malloc); }

private:
    INpaFile* pArchive;
    INpaFile::NpaIterator File;
};

class ResourceMgr
{
public:
    ResourceMgr();
    virtual ~ResourceMgr();

    virtual Resource GetResource(std::string Path);
    virtual char* Read(std::string Path, uint32_t& Size);
    ScriptFile* GetScriptFile(const std::string& Path);

protected:
    virtual ScriptFile* ReadScriptFile(const std::string& Path) = 0;
    std::vector<INpaFile*> Archives;
};

extern ResourceMgr* sResourceMgr;

#endif
