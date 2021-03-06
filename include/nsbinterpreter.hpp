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
#ifndef NSB_INTERPRETER_HPP
#define NSB_INTERPRETER_HPP

#include "scriptfile.hpp"
#include "object.hpp"
#include "objectholder.hpp"

#include <stack>
#include <cstdint>
#include <list>
#include <map>
#include <queue>
#include <vector>
#include <functional>
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 576

namespace sf
{
    class RenderTexture;
    class Texture;
}

namespace std
{
    class thread;
}

class Game;
class ResourceMgr;
class Line;
class Drawable;
class DrawableBase;
class ArrayVariable;
class Text;
class Playable;
class Variable;
class NsbContext;

typedef std::list<std::pair<string, ArrayVariable*>> ArrayMembers;
typedef std::function<int32_t(int32_t)> PosFunc;
typedef std::map<string, Variable*> VariableStore;

class SpecialPosVisitor : public boost::static_visitor<PosFunc>
{
    static const size_t SPECIAL_POS_NUM = 7;
public:
    PosFunc operator()(int32_t Int) const
    {
        return [Int] (int32_t) { return Int; };
    }

    PosFunc operator()(string Str) const
    {
        std::transform(Str.begin(), Str.end(), Str.begin(), ::tolower);
        size_t i = -1;
        while (++i < SPECIAL_POS_NUM)
            if (Str == SpecialPos[i])
                return SpecialPosTable[i];
        assert(false);
    }
private:
    const PosFunc SpecialPosTable[SPECIAL_POS_NUM] =
    {
      [] (int32_t x) { return WINDOW_WIDTH / 2 - x / 2; },
      [] (int32_t y) { return WINDOW_HEIGHT - y; },
      [] (int32_t y) { return WINDOW_HEIGHT / 2 - y / 2; },
      [] (int32_t x) { return 0; },
      [] (int32_t y) { return 0; },
      [] (int32_t y) { return 0; },
      [] (int32_t x) { return 0; }
    };
    const string SpecialPos[SPECIAL_POS_NUM] =
    {
        "center", "inbottom", "middle",
        "onleft", "outtop", "intop",
        "outright"
    };
};

struct Button
{
    Button() : Clicked(false)
    {
    }

    void CheckClicked(int x, int y);
    std::map<string, Drawable*> Textures;
    bool Clicked;
};

// Represents Nitroscript variable
struct Variable
{
    Variable() : IsPtr(false) {}
    Variable(int32_t Int, bool IsPtr = false) : Value(Int), IsPtr(IsPtr) {}
    Variable(const string& String, bool IsPtr = false) : Value(String), IsPtr(IsPtr) {}
    virtual ~Variable() {}
    bool IsPtr;
    boost::variant<int32_t, float, string> Value;
};

// Note that this structure is actually a tree. See: ArrayMembers typedef
struct ArrayVariable : Variable
{
    ArrayMembers Members;
};

struct Callback
{
    sf::Keyboard::Key Key;
    string Script;
};

template <class T>
class _stack : public std::stack<T>
{
    typedef typename std::stack<T>::size_type size_type;
public:
    T& operator[] (size_type n)
    {
        assert(n < this->c.size());
        return this->c[n];
    }

    size_type size()
    {
        return this->c.size();
    }
};

class NsbInterpreter
{
    friend class NsbContext;
    typedef void (NsbInterpreter::*BuiltinFunc)();
public:
    NsbInterpreter();
    virtual ~NsbInterpreter();
    virtual void Initialize(Game* pGame);

    void Stop();
    void Pause();
    void Start();
    void StartDebugger();

    void CallScript(const string& FileName, const string& Symbol);
    void ExecuteScriptLocal(const string& InitScript);
    void HandleEvent(sf::Event Event);
    void GLDelete(DrawableBase* pDrawable);
protected:
    void Run();
    void Sleep(int32_t ms);
    void LoadScript(const string& FileName);
    void ExecuteScript(const string& InitScript);

    // Magic handlers
    void Break();
    void LoadImage();
    void Jump();
    void CreateProcess();
    void LoadTextureClip();
    void Increment();
    void LogicalGreater();
    void LogicalLess();
    void Count();
    virtual void Literal();
    void Get();
    void DrawToTexture();
    void CreateColor();
    void Fade();
    void Begin();
    void End();
    void CreateTexture();
    void Delete();
    void CallFunction();
    void Add();
    void String();
    void CreateMovie();
    void DrawTransition();
    void ClearParams();
    void CreateRenderTexture();
    void RemainTime();
    void DurationTime();
    void ApplyBlur();
    void CreateWindow();
    void SetTextboxAttributes();
    void CreateSound();
    void SetLoopPoint();
    void SetFontAttributes();
    void SetVolume();
    void WaitText();
    void Move();
    void Wait();
    void WaitKey();
    void SetLoop();
    void ParseText();
    void Request();
    void SetShortcut();
    void ArrayRead();
    void Assign();
    void Negative();
    void PlaceholderParam();
    void Zoom();
    void If();
    void LogicalNot();
    void LogicalEqual();
    void LogicalNotEqual();
    void LogicalAnd();
    void LogicalOr();
    void LogicalGreaterEqual();
    void LogicalLessEqual();
    void CallChapter();
    void SetVertex();
    void System();
    void CreateScrollbar();
    void CallScene();
    void Time();
    void Shake();
    void GetModuleFileName();
    void ScopeBegin();
    void ScopeEnd();
    void WriteFile();
    void Divide();
    void ImageHorizon();
    void ImageVertical();
    void Multiply();
    void Substract();
    void VariableValue();
    void ReadFile();
    void AssocArray();
    void Array();
    void SetAlias();
    void CreateChoice();
    void SetNextFocus();
    void SetFrequency();
    void SetPan();
    void SoundAmplitude();
    void Select();
    void While();
    void Exit();
    void CaseBegin();
    void StrStr();
    void AddAssign();
    void SubAssign();
    void UNK90();
    void CursorPosition();
    void MoveCursor();
    void Position();

    // Stubs
    void CreateName();
    void UNK101();
    void UNK115();
    void LockVideo();
    void Save();
    void DeleteSaveFile();
    void Conquest();
    void ClearScore();
    void ClearBacklog();

    // Builtin functions
    void NSBDelete();
    void NSBArrayRead(int32_t Depth);
    void NSBRequest(const string& State);
    void NSBSetLoop(Playable* pMusic, bool Loop);
    void NSBWaitText(Text* pText, const string& unk);
    void NSBSetVolume(Playable* pMusic, int32_t NumSeconds, int32_t Volume, const string& Tempo);
    void NSBSetLoopPoint(Playable* pMusic, int32_t Begin, int32_t End);
    //void NSBSetFontAttributes(const string& Font, int32_t Size, const string& Color1, const string& Color2, int32_t unk0, const string& unk1);
    void NSBCreateSound(const string& Type, const string& File);
    //void NSBSetTextboxAttributes(int32_t unk0, const string& Font, int32_t unk1, const string& Color1, const string& Color2, int32_t unk2, const string& unk3);
    void NSBCreateWindow(int32_t unk0, int32_t x, int32_t y, int32_t Width, int32_t Height, bool unk1);
    void NSBRemainTime();
    void NSBDurationTime();
    void NSBFade(DrawableBase* pDrawable, int32_t Time, int32_t Opacity, const string& Tempo, bool Wait);
    void NSBCreateProcess(int32_t unk1, int32_t unk2, int32_t unk3, const string& Function);
    void NSBSystem(string Command, string Parameters, string Directory);
    void NSBWriteFile(string& FileName, string& Data);
    void NSBReadFile(const string& Filename);

    // GL functions are builtins like NSB, but need to be called from OpenGL thread
    // See: Game::GLCallback
    void GLZoom(Drawable* pDrawable, int32_t Time, int32_t x, int32_t y, const string& Tempo, bool Wait);
    void GLMove(DrawableBase* pDrawable, int32_t Time, int32_t x, int32_t y, const string& Tempo, bool Wait);
    void GLCreateTexture(int32_t Priority, PosFunc XFunc, PosFunc YFunc, const string& File);
    void GLCreateColor(int32_t Priority, int32_t x, int32_t y, int32_t Width, int32_t Height, const string& Color);
    // NOTE: Chaos;Head doesn't have last parameter (音声同期)
    void GLCreateMovie(int32_t Priority, int32_t x, int32_t y, bool Loop, bool Alpha, const string& File, bool Audio);
    void GLDrawTransition(Drawable* pDrawable, int32_t Time, int32_t Start, int32_t End, int32_t Range, const string& Tempo, string File, bool Wait);
    void GLCreateRenderTexture(int32_t Width, int32_t Height, const string& Color);
    void GLDrawToTexture(sf::RenderTexture* pTexture, int32_t x, int32_t y, const string& File);
    void GLApplyBlur(Drawable* pDrawable, const string& Heaviness);
    void GLParseText(const string& Box, const string& XML);
    void GLLoadTextureClip(int32_t Priority, PosFunc XFunc, PosFunc YFunc, int32_t tx, int32_t ty, int32_t width, int32_t height, const string& File);
    void GLLoadImage(const string& File);

    // Set, Increment, Decrement...
    virtual void OnVariableChanged(const string& Identifier) { }

    // Helpers
    sf::Vector2u TextureSize();
    void Pop();
    template <class T> void Push(T Val);
    template <class T> T* Get(bool Expect);
    template <class T> T Pop();
    template <class T, class U> void BinaryOperator(std::function<U(T, T)> Func);
    template <class T> T GetVariable(const string& Identifier); // Transforms identifier to value
    template <class T> void WildcardCall(std::string Handle, std::function<void(T*)> Func); // Calls Func for all handles matching wildcard
    void SetVariable(const string& Identifier, Variable* pVar); // Sets value of global variable
    void SetLocalVariable(const string& Identifier, Variable* pVar);
    void SetVariable(const string& Identifier, Variable* pVar, VariableStore& Container);
    void CallScriptSymbol(const string& Prefix);
    DrawableBase* GetDrawable(bool Expect = true); // Return current drawable as specified by HandleName
    Playable* GetPlayable(bool Expect = true); // Expect = true: Error if not found, Expect = false: Error if found
    Object* GetObject(bool Expect = true);

    // Event handlers
    void KeyPressed(sf::Keyboard::Key Key);
    virtual void MouseMoved(sf::Vector2i Pos);
    virtual void MouseClicked(sf::Event::MouseButtonEvent Event);

    bool NsbAssert(bool expr, string error);

    Game* pGame;
    NsbContext* pContext;
    NsbContext* pMainContext;
    volatile bool RunInterpreter;
    volatile bool StopInterpreter;

    bool EventLoop;
    string HandleName; // Identifier of current Drawable/Playable used by NSB and GL functions
    std::vector<ScriptFile*> LoadedScripts; // Scripts considered in symbol lookup
    VariableStore Variables; // Global variables
    VariableStore LocalVariables;
    std::map<string, ArrayVariable*> Arrays; // Same as above, except these are trees (TODO: merge?)
    _stack<Variable*> Stack; // Variable stack (builtin function parameters)
    std::vector<BuiltinFunc> Builtins; // Jump table for builtin functions
    std::list<NsbContext*> Threads;
    std::vector<Callback> Callbacks;

private:
    void WaitForResume();

    // NsbDebugger
    void DbgBreak();
    void Inspect(int32_t n);
    void PrintArray(ArrayVariable* pArray);
    void PrintVariable(Variable* pVar);
    void DebuggerTick(uint16_t Magic);
    string Disassemble(Line* pLine);
    void DebuggerMain();
    void SetBreakpoint(const string& Script, int32_t LineNumber);
    std::list<std::pair<string, int32_t> > Breakpoints;
    std::vector<bool> MagicBreakpoints;
    std::thread* pDebuggerThread;
    volatile bool DbgStepping;
    volatile bool BreakOnAssert;
};

template <> PosFunc NsbInterpreter::Pop();
template <> bool NsbInterpreter::Pop();
template <> void NsbInterpreter::Push(bool Val);

sf::Texture* LoadTextureFromFile(const string& File, const sf::IntRect& Area);
sf::Texture* LoadTextureFromColor(string Color, int32_t Width, int32_t Height);

/* Template member function definitions */

template <class T> T* NsbInterpreter::Get(bool Expect)
{
    T* pObject = dynamic_cast<T*>(ObjectHolder::Read(HandleName));
    if (Expect) NsbAssert(pObject, "Failed to find object");
    return pObject;
}

template <class T> void NsbInterpreter::WildcardCall(std::string Handle, std::function<void(T*)> Func)
{
    ObjectHolder::ForeachWildcard<T>(Handle, Func, HandleName);
}

template <class T> T NsbInterpreter::GetVariable(const string& Identifier)
{
    auto iter = Variables.find(Identifier);
    try
    {
        // Not a variable but a literal
        if (iter == Variables.end())
            return boost::lexical_cast<T>(Identifier);
        // Regular variable, TODO: Only dereference if $?
        else
            return boost::get<T>(iter->second->Value);
    }
    catch (...)
    {
        std::cout << "Failed to cast " << Identifier << " to correct type." << std::endl;
        return T();
    }
}

template <class T> void NsbInterpreter::Push(T Val)
{
    Stack.push(new Variable(Val));
}

template <class T> T NsbInterpreter::Pop()
{
    if (NsbAssert(!Stack.empty(), "Poping from empty stack"))
        return T(); // TODO: HACK!
    Variable* pVar = Stack.top();
    T Ret;
    if (T* pT = boost::get<T>(&pVar->Value))
        Ret = *pT;
    else
    {
        NsbAssert(false, "Pop<>(): Unexpected variable type");
        Ret = T();
    }
    if (!pVar->IsPtr)
        delete pVar;
    Stack.pop();
    return Ret;
}

template <class T, class U> void NsbInterpreter::BinaryOperator(std::function<U(T, T)> Func)
{
    T First = Pop<T>();
    T Second = Pop<T>();
    Push(Func(Second, First));
}

#endif
