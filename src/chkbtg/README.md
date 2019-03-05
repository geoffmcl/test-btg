# btgchk project - 20190217 - @reiszner

#### Brief Outline

Run a series of checks on `*.btg.gz` files, Scenery v2.+, ie current terragear, TerraSync server... airport and index based...

If anomolies found, write corrected *-new.bgt.gz, for review, ... like -

 - Repair the mesh...
 - Cut airport holes...
 - Check bordering buckets match...
 
Of course, in due course, over time, these **generated** glitches, will be resolved, fixed, discussed, decided, ... in the `terragear` tool chain...

But seems good to have an independant view... is C, .... does not use SimGear binobj...

Does not handle `fans`, `stripes`, but it is assumed there are none in the v.2.? data...

In some cases, can provide a quick fix, until the **next** scenery generation, is under steam... online...

Thanks @reiszner...

#### Build Source

This is a [cmake](https://cmake.org/install/) build project... see root `CMakeLists.txt` for details...

Unix Build - using say default `Unix Makefiles` generator

```
$ cd build # use out-of-src build
$ cmake .. [-DCMAKE_BUILD_TYPE=Release] [-DCMAKE_INTALL_PREFIX=/where/to/install]
$ make
```

Windows Build - using say `Visual Studio 14 2015 [arch]` generator, or your choice...

```
> cd build # use out-of-src bld
> cmake .. -G "Visual Studio 14 Win64" [-DCMAKE_BUILD_TYPE=Release] [-DCMAKE_INSTALL_PREFIX=x:\Projects\3rdParty.x64]
> cmake --build . --config [Release|Debug|RelWithDebInfo]
```

Or, if preferred, can use the GUI IDE, for cmake, msvc, ...

Happy building...

; eof


