# Hana
Hana is an elegant, clean and minimalistic interpreted programming language inspired from lua, python and javascript &lt;3

⚠️*Wip ...*

*For now Doxygen docs* - [🌸 Hana Documentation](https://syylvette.github.io/Hana/)

## Deps
* cmake >=3.12
* flex
* bison
* LLVM 14*

## Building the interpreter

```
git clone https://github.com/syylvette/Hana.git
cd Hana
mkdir Build && cd Build
cmake ../Hana && make

## Generates a binary 'hana' in Build/Hana directory
./hana -h  ## Lists the usage
```

## Basic Syntax

```
-- Single line comment (Lua dope lol)
--[[ 
    Multi-line Comment 
--]]

let x = 10
let s = "Hellow"

let y
y = true

if condition
    Do-something
else
    Do-something-else

while i > 0
  i = i - 1
  Do-something
else
  display("Terminated")

fn myFunction(type param1, type param2) : return-type
  Statements
  return expression

fn myFunction()
  Statements

fn myFunction(let param1, let param2)
  Statements
  return expression

int x = 1
int y = 2 
int sum = x+y
boolean flag = false
string s = 'This is a string.'

-- Or, Javascript way
let n = 1     
let sayHi = "Hello" 

-- Follows python indentation
Stmt
  Stmt
    Stmt
  Stmt
Stmt

input() -- Takes a string only as of now

display("%s", input())

display("%s", "Idk..") -- C like scanf() 
displayln("%d", some-int) -- with Newline

```
