/* 
 * libnpengine: Nitroplus script interpreter
 * Copyright (C) 2014 Mislav Blažević <krofnica996@gmail.com>
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
#ifndef NSB_CONTEXT_HPP
#define NSB_CONTEXT_HPP

#include "object.hpp"

// Element of call stack
// Contains script and line to which function will return upon ending
struct FuncReturn
{
    ScriptFile* pScript;
    uint32_t SourceLine;
};

class NsbContext : public Object
{
public:
    NsbContext(const string& Name);

    void SetScopeEndLabel(const string& Label);
    void Break();
    void Jump(uint32_t SourceLine);
    void Jump(const string& Symbol);
    bool CallSubroutine(ScriptFile* pDestScript, const string& Symbol); // Attempts to call specified symbol in specified script
    void ReturnSubroutine(); // Function return: pop the call stack
    const std::vector<string>& GetLineArgs() { return pLine->Params; }
    const string& GetName() { return Name; }
    const string& GetScriptName() { return pScript->GetName(); }
    uint32_t GetNextLineEntry() { return SourceIter; }
    ScriptFile* GetScript() { return pScript; }
    void Start() { Active = true; }
    void Stop() { Active = false; }
    void Sleep(int32_t ms);
    void SleepKey(int32_t ms);
    void Resume();
    void Run(NsbInterpreter* pInterpreter);
    void Request(Game* pGame, const string& State);

    void WriteTrace(std::ostream& Stream);

    bool WaitKey;
private:
    bool NextLine(); // Next instruction

    string Name; // Thread handle name
    bool Active; // If true, code in this context is executed
    Line* pLine; // Line of code which is currently being executed
    uint32_t SourceIter;
    ScriptFile* pScript; // Script which is currently being executed (top of call stack)
    sf::Clock SleepClock; // SleepTime clock
    sf::Time SleepTime; // How long should interpreter wait before executing next line
    std::stack<FuncReturn> Returns; // Call stack
    string ScopeEndLabel;
};

#endif
