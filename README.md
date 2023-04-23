<h1 align="center">Hana</h1>
Hana is an elegant, clean and minimalistic interpreted programming language inspired from lua, python and javascript &lt;3

⚠️ *Wip ...*

*For now Doxygen docs* - [🌸 Hana Documentation](https://syylvette.github.io/Hana/)

<h2 align="center">Deps</h2>

<ul>
<li>cmake >= 3.12</li>
<li>flex</li>
<li>bison</li>
<li>LLVM (version 10.0.1)</li>
</ul>

<h2 align="center">Building the interpreter</h2>

```
git clone https://github.com/syylvette/Hana.git
cd Hana
mkdir Build && cd Build
cmake .. && make

## Generates a binary 'hana' in Build/Hana directory
./hana -h  ## Lists the usage
```

<h2 align="center">Usage</h2>

<h4 align="left">Create a hana file - <code>hello.hana</code></h4>

```py
writeln("Hello World!")
```

<h4 align="left">Run</h4>

Using the hana interpreter
```sh
hana hello.hana
```

<h4 align="left">Output</h4>

```
Hello World!
```

<h2 align="center">Documentation</h2>

<h3 align="center">General</h3>

<p align="center"><code>hana -h</code> Opens the Hana help menu.</p>

<h3 align="center">Variables</h3>
<p align="center">Variables can be decluwuared using the keyword <code>let</code> or by using their types <code>int</code> <code>double</code> <code>string</code> <code>boolean</code>.</p>

```py
let baka = 99
string tehe = "hahahah"
int chan = 25
let baka = baka + 1
let chan = chan * 2

writeln(tehe)
writeln("%d", baka)
writeln("%d", chan) -- Basically just a scanf alias 
```

<h4 align="left">Output</h4>

```
hahahah
100
50
```

<h3 align="center">Conditionals</h3>

```py
if《condition》
  《statements》
else《condition》
  《statements》

-- No else if supported now
```

<h3 align="center">Comments</h3>
```
-- Single line comment, inspired from lua!
```
```py
--[[
  Multi
  Line
  Comment
--]]
```


<h3 align="center">Loops</h3>

```py
let c = 5
while c > 0
    writeln("UwU")
    c = c - 1
else
    writeln("Boom")
```

<h4 align="left">Output</h4>

```
UwU
UwU
UwU
UwU
UwU
Boom
```

<h3 align="center">Functions/Classes</h3>

Function are created by ```block()``` keyword.
Classes can also be created by same keyword ```block```.

```py
block funcName(《parameters》) : 《returnType》
    《statements》
    return 《nothing/something》
```
Refer ```Testing``` for more examples ~

---


> ReadMe [credit](https://github.com/virejdasani/pythOwO)








