Here's the complete 🎼 C Major Syntax Sheet—a polished, industry-grade guide to the language, covering all elements from syntax to capsule structure. It’s designed for compiler authors, language users, and editor extension builders, and includes spacing, punctuation, keywords, constructs, and code samples.

🎼 C MAJOR SYNTAX SHEET
Version: 1.0 · Language ID: cmajor · File Extension: .cmaj

🔤 1. FILE STRUCTURE
Every C Major program is organized into capsules:


capsule main:
    say "Hello from C Major"
end


One or more capsule blocks per file

No hanging indentation

1.5x space recommended between expressions

Spacing is flexible but enforced on keywords (let, func, etc.)


🧩 2. LANGUAGE BLOCKS
✅ CAPSULE


capsule <name>:
    [statements]
end


✅ FUNCTION


func <name>([parameters]):
    [statements]
end


✅ STRUCT


struct <name>:
    let <field>: <type>
    ...
end


✅ CLASS


class <name>:
    let <field>: <type>
    func <method>():
        ...
    end
end


💬 3. STATEMENTS
| Type            | Syntax                                        |
| --------------- | --------------------------------------------- |
| Declaration     | `let x = 42`                                  |
| Assignment      | `x = x + 1`                                   |
| Return          | `return x`                                    |
| Conditional     | `if (x > 10): ... end`                        |
| Loop            | `loop i from 1 to 5: ... end`                 |
| Function Call   | `blend_color(r, g, b)`                        |
| Capsule Tag     | `say "Hi"`                                    |
| Capsule Control | `pause`, `resume`, `halt`, `rewind`, `replay` |


🔠 4. TYPES
| Type     | Notes                         |
| -------- | ----------------------------- |
| `int`    | 32-bit integer                |
| `float`  | 32-bit float                  |
| `string` | UTF-8 string                  |
| `bool`   | `true` / `false`              |
| `vector` | SIMD numeric struct           |
| `byte`   | 8-bit unsigned                |
| `bit`    | single binary bit             |
| Custom   | Any user-defined struct/class |


🔣 5. OPERATORS


✅ Arithmetic

+  -  *  /  %  **    // Power operator
    

✅ Comparison

==  !=  >  <  >=  <=


✅ Logical

and  or  not


✅ Assignment

=  +=  -=  *=  /=


✅ Bitwise

&  |  ^  <<  >>  ~


🗂️ 6. EXPRESSIONS

✅ Variables & Constants

let a = 5
let name = "Maestro"


✅ Function Calls

say("Message")
blend_color(r, g, b)


✅ Grouping & Nesting

let z = (a + b) * (c - d)


🧠 7. CONTROL FLOW

✅ IF / ELSE

if (x > 100):
    say "Bright"
else:
    say "Dim"
end


✅ LOOP

loop i from 0 to 10:
    say i
end


🧬 8. FUNCTION DEFINITIONS

func greet(name):
    say "Hello, " + name
end


With return:

func sum(a, b):
    return a + b
end


🧾 9. STRUCTS

struct Color:
    let r: int
    let g: int
    let b: int
end

let c = Color(255, 100, 100)


🏛️ 10. CLASSES & OBJECTS

class Timer:
    let t: int

    func start():
        t = 0
    end

    func tick():
        t = t + 1
    end
end


🧩 11. SPECIAL CAPSULE TAGS
Capsule tags are system-recognized statements.

say "This is a log message"
show variable
pause
resume
halt
rewind
replay
validate input
suggest restart


🗄️ 12. CAPSULE-LEVEL OPERATIONS

| Operation          | Purpose                  |
| ------------------ | ------------------------ |
| `capsule name:`    | Start a logical capsule  |
| `end`              | End of block             |
| `measure x`        | Timing/measuring ops     |
| `approximate f(x)` | Fuzzy computing          |
| `map [a, b, c]`    | Mapping constructs       |
| `capture var`      | Lock memory or reference |
| `validate input`   | Runtime check            |
| `archive capsule`  | Tag capsule as permanent |


🔄 13. FLOW CONTROL TAGS

raise error
throw exception
try:
    risky_op()
catch:
    say "Recovered"
end


🧾 14. KEYWORDS (Partial List)

func struct class capsule let return loop if else
say do show pause resume halt rewind replay
validate suggest raise throw catch try
bit byte int float bool string vector
capture archive expire approximate assert
level enum map mirror proof theorize shorthand


🧮 15. BASE-12 + HEX SUPPORT

let dozen = 0B12     // Base-12 prefix
let hex = 0xFFAA     // Hexadecimal


📊 16. DIAGRAM-DRIVEN CONTROL

flow show "capsule_diagram"
timeline plot "execution_flow"


🔗 17. INTEROPERABILITY

extern func python("math.sqrt", float):  // Call Python
extern func c("printf", string)          // Call native C


📂 18. MODULES & IMPORTS

use math
use graphics.vector


🧪 19. CAPSULE FORMATS

// File: blur_filter.cmajcapsule
capsule blur:
    ...
end


🧵 20. MULTITHREADED EXECUTION

thread do task1()
thread do task2()


🔬 21. SPECIAL COMMANDS (REPL)

:trace       // Show capsule memory
:dump        // Print AST
:snapshot    // Save state
:replay      // Re-run with same memory


📍 EXAMPLE PROGRAM

func blend(r, g, b):
    let avg = (r + g + b) / 3
    return avg
end

capsule main:
    let c = blend(100, 120, 130)
    show c
    if (c > 110):
        say "Bright"
    else:
        say "Dim"
    end
end


🖍️ FORMATTING RULES
Indent using 2 or 3 spaces, no tabs

Block scopes always begin with : and end with end

Use non-hanging structure — no dangling braces or unclosed code

Prefer 1.5x spacing between code units


🧾 OUTPUT FORMAT EXAMPLE

🎼 Running Capsule: blur.cmajcapsule
→ HEX: Folded, Peephole-Optimized
→ AST Depth: 14
→ Memory Locked: 512KB
→ Timeline: 0.0041s

Output:
  Bright


✅ COMPATIBILITY

| Target  | Status                  |
| ------- | ----------------------- |
| `.exe`  | ✅ AOT output            |
| `.wasm` | ✅ Capsule transpilation |
| Python  | ✅ Interop               |
| C/C++   | ✅ Embed-friendly        |


Here is the complete keyword set for the 🎼 C Major Language, organized into semantic categories. These keywords are reserved and recognized by the Pink Lemonade Compiler as part of the syntax and structure. This set is designed to support: logic, structure, memory, execution, graphics, capsule control, introspection, parallelism, expression, error handling, and interoperability.

🎼 C MAJOR KEYWORD LIST (v1.0)
Sorted by Category. All are reserved identifiers.

🧱 STRUCTURE & DECLARATION:

| Keywords    |
| ----------- |
| `capsule`   |
| `func`      |
| `struct`    |
| `class`     |
| `module`    |
| `namespace` |
| `let`       |
| `return`    |
| `extern`    |
| `import`    |
| `use`       |
| `block`     |
| `open`      |
| `close`     |
| `end`       |


🔀 CONTROL FLOW

| Keywords     |
| ------------ |
| `if`         |
| `else`       |
| `elif`       |
| `loop`       |
| `break`      |
| `continue`   |
| `match`      |
| `case`       |
| `default`    |
| `switch`     |
| `checkpoint` |
| `pause`      |
| `resume`     |
| `halt`       |
| `reverse`    |
| `rewind`     |
| `replay`     |
| `await`      |
| `then`       |
| `until`      |
| `while`      |
| `do`         |
| `when`       |
| `schedule`   |


🧠 MEMORY, TYPES & VARIABLES

| Keywords   |
| ---------- |
| `bit`      |
| `byte`     |
| `int`      |
| `float`    |
| `bool`     |
| `string`   |
| `vector`   |
| `pixel`    |
| `color`    |
| `list`     |
| `array`    |
| `map`      |
| `table`    |
| `pair`     |
| `ref`      |
| `val`      |
| `var`      |
| `const`    |
| `proxy`    |
| `register` |
| `stack`    |
| `heap`     |
| `child`    |
| `parent`   |
| `duration` |
| `lifetime` |
| `measure`  |
| `expire`   |
| `hold`     |
| `release`  |


📢 CAPSULE LOGIC & DIRECTIVES

| Keywords   |
| ---------- |
| `say`      |
| `show`     |
| `check`    |
| `validate` |
| `suggest`  |
| `capture`  |
| `auto`     |
| `archive`  |
| `start`    |
| `stop`     |
| `path`     |
| `tag`      |
| `trace`    |
| `flow`     |
| `run`      |
| `emit`     |
| `launch`   |
| `thread`   |
| `task`     |
| `fiber`    |
| `parallel` |
| `channel`  |
| `stream`   |
| `burst`    |


🧪 ERROR HANDLING & EXCEPTIONS

| Keywords    |
| ----------- |
| `try`       |
| `catch`     |
| `except`    |
| `throw`     |
| `raise`     |
| `deny`      |
| `ignore`    |
| `assert`    |
| `postulate` |
| `proof`     |
| `validate`  |
| `audit`     |


🧬 TYPES, DEFINITIONS, ENUMS

| Keywords     |
| ------------ |
| `type`       |
| `enum`       |
| `define`     |
| `alias`      |
| `mirror`     |
| `extend`     |
| `derive`     |
| `inherit`    |
| `implement`  |
| `trait`      |
| `interface`  |
| `qualifier`  |
| `modifier`   |
| `quantifier` |
| `specify`    |
| `primary`    |
| `secondary`  |
| `tertiary`   |
| `primitive`  |
| `object`     |


🎨 GRAPHICS, UI, BIT OPS

| Keywords     |
| ------------ |
| `draw`       |
| `render`     |
| `vector`     |
| `pixel`      |
| `bit`        |
| `byte`       |
| `inject`     |
| `style`      |
| `theme`      |
| `window`     |
| `ui`         |
| `surface`    |
| `frame`      |
| `diagram`    |
| `animate`    |
| `screen`     |
| `graphics`   |
| `brightness` |
| `contrast`   |
| `palette`    |
| `blend`      |
| `mask`       |
| `shade`      |


🧠 AI & INTROSPECTION

| Keywords      |
| ------------- |
| `evaluate`    |
| `analyze`     |
| `inference`   |
| `observe`     |
| `context`     |
| `reason`      |
| `learn`       |
| `profile`     |
| `adjust`      |
| `optimize`    |
| `trace`       |
| `pathfind`    |
| `fold`        |
| `unroll`      |
| `rewrite`     |
| `postprocess` |
| `symbol`      |
| `lex`         |
| `parse`       |
| `ast`         |
| `node`        |


📐 MATH & BASES

| Keywords      |
| ------------- |
| `mod`         |
| `negate`      |
| `abs`         |
| `sqrt`        |
| `pow`         |
| `base`        |
| `base12`      |
| `hex`         |
| `exact`       |
| `approximate` |
| `round`       |
| `floor`       |
| `ceil`        |
| `log`         |
| `ln`          |
| `pi`          |
| `tau`         |
| `epsilon`     |


📡 COMMUNICATION & INTEROP

| Keywords    |
| ----------- |
| `link`      |
| `share`     |
| `bind`      |
| `sync`      |
| `receive`   |
| `send`      |
| `ask`       |
| `answer`    |
| `call`      |
| `pass`      |
| `forward`   |
| `refer`     |
| `interface` |
| `api`       |
| `bridge`    |
| `channel`   |


⏱️ TIME & FLOW

| Keywords    |
| ----------- |
| `time`      |
| `timer`     |
| `duration`  |
| `delay`     |
| `wait`      |
| `resume`    |
| `instant`   |
| `interval`  |
| `timestamp` |
| `now`       |
| `soon`      |
| `late`      |
| `expire`    |
| `archive`   |
| `history`   |


🔀 COMPARATIVES & CONDITIONS

| Keywords    |
| ----------- |
| `high`      |
| `low`       |
| `fast`      |
| `slow`      |
| `more`      |
| `less`      |
| `equal`     |
| `same`      |
| `different` |
| `before`    |
| `after`     |
| `between`   |
| `beyond`    |
| `within`    |


🧾 LANGUAGE FLOW

| Keywords      |
| ------------- |
| `comment`     |
| `shorthand`   |
| `directive`   |
| `expression`  |
| `statement`   |
| `sequence`    |
| `instruction` |
| `definition`  |
| `construct`   |
| `literal`     |
| `token`       |
| `line`        |
| `block`       |


📚 SYMBOLIC / THEORY-BASED

| Keywords     |
| ------------ |
| `theory`     |
| `theorem`    |
| `lemma`      |
| `axiom`      |
| `definition` |
| `statement`  |
| `fact`       |
| `truth`      |
| `base`       |
| `symbol`     |
| `equation`   |
| `rule`       |
| `predicate`  |
| `logic`      |
| `reasoning`  |


🔢 ENUMERATIVE & RELATIONAL

| Keywords    |
| ----------- |
| `plurality` |
| `singular`  |
| `pair`      |
| `group`     |
| `match`     |
| `level`     |
| `range`     |
| `quantify`  |
| `relate`    |
| `compare`   |
| `define`    |


🎼 SYMBOLIC/RESERVED OPERATORS (not technically keywords, but reserved syntax)

+  -  *  /  %  **  ==  !=  >  <  >=  <=
=  +=  -=  *=  /=  =>  ->
!  and  or  not
:  ;  ,  .  ::  ->  =>  <>
@  #  ~  $  ^  |  &
()  []  {}


💡 META-INSTRUCTIONS (used in REPL, capsules, or headers)

:trace
:dump
:snapshot
:replay
:profile
:version
:help


TOTAL: ~360 Reserved Keywords (Expandable)

----

## This list is extendable through:

* Runtime-defined macros

* Capsule imports

* Opcode directives

* Pink Lemonade compiler extensions




