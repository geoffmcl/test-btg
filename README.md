# Test BTG File

This is a simple project to test and explore BTG (Binary TerraGear) scenery files.

## Backgound:

The scenery of <a target="_blank" href="http://www.flightgear.org/">flightgear.org</a>, <a target="_blank" href="http://wiki.flightgear.org/FlightGear">wiki.flightgear</a>, built by <a target="_blank" href="http://wiki.flightgear.org/TerraGear">Terragear</a>, <a target="_blank" href="https://sourceforge.net/p/flightgear/terragear/ci/master/tree/">source</a>.

Documentation on the BTG file format can be found at <a target="_blank" href="http://wiki.flightgear.org/index.php/BTG_File_Format">BTG_File_Format</a>.

The primary class is <a target="_blank" href="http://api-docs.freeflightsim.org/simgear/classSGBinObject.html">class SGBinObject</a>, coded in <a target="_blank" href="http://api-docs.freeflightsim.org/simgear/sg__binobj_8cxx.html">sg_binobj.cxx</a>, and <a target="_blank" href="http://api-docs.freeflightsim.org/simgear/sg__binobj_8hxx.html">sg_binobj.hxx</a>.


## Building:

### Prereqisites:

 1. git - http://git-scm.com/downloads
 2. cmake - http://www.cmake.org/download/
 3. build tools, like <a target="_blank" href="https://www.visualstudio.com/en-us/downloads/download-visual-studio-vs.aspx">MSVC</a> in Windows, make/gcc in unix, etc
 4. simgear - http://simgear.sourceforge.net/downloads.html - which has its own dependencies
    1. OpenSceneGraph - http://www.openscenegraph.org/ - v 3.2 or later, whihc also has many depenencies
    2. plib - http://plib.sourceforge.net/ - v 1.8.5 or later
    3. boost - http://www.boost.org/ - v 1.37 or newer
    4. OpenAL - http://kcat.strangesoft.net/openal.html
    5. zlib - http://www.zlib.net/
    6. png - http://www.libpng.org/pub/png/pngcode.html
    7. freetype - http://www.freetype.org/download.html
    8. glut - http://freeglut.sourceforge.net/
    9. Plus some optional source, and some I have maybe forgotten!
    
Building SimGear from source is quite a difficult task. I have a repo which can help with that in windows - https://gitlab.com/fgtools/fg-win - where I have tried to include ALL the above sources. And for unix there is <a target="_blank" href="https://sourceforge.net/p/flightgear/fgmeta/ci/next/tree/download_and_compile.sh">download_and_compile.sh</a>

### Building:

This project uses cmake to generate native build files. And as usual cmake is better run in an out-of-source build folder.

After the source has been cloned into a directory -

#### Unix

 1. $ cd build
 2. $ cmake ..
 3. $ make
 
#### Windows

 1. cd build
 2. cmake ..
 3. cmake --build . --config Release
 
Or, of course the cmake GUI can be used, setting the appropriate source diectory, and the bnary directory to the build folder. Click [configure], perhaps several time until all errors are gone, then click [generate] to create the chosen generator files.

Then in windows, if MSVC is the generator, the MSVC IDE can be opened, and the test_btg.sln file loaded. Select the configuration desired, and build.

; eof
