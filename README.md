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
    1. OpenSceneGraph - http://www.openscenegraph.org/ - v 3.2 or later, which also has many dependencies
    2. plib - http://plib.sourceforge.net/ - v 1.8.5 or later
    3. boost - http://www.boost.org/ - v 1.37 or newer
    4. OpenAL - http://kcat.strangesoft.net/openal.html
    5. zlib - http://www.zlib.net/
    6. png - http://www.libpng.org/pub/png/pngcode.html
    7. freetype - http://www.freetype.org/download.html
    8. glut (freeglut) - http://freeglut.sourceforge.net/
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
 
Naturally, the 2nd step will FAIL is simgear, and othre prereqisite libraries are not found.
 
Or, of course the cmake GUI can be used, setting the appropriate source diectory, and the bnary directory to the build folder. Click [configure], perhaps several times until all errors are gone, then click [generate] to create the chosen generator files.

Then in windows, if MSVC is the generator, the MSVC IDE can be opened, and the test_btg.sln file loaded. Select the configuration desired, and build.

### Running:

Running test-btg console app with --help, -? will show the brief help. In essence need to give the path to an <index>.btg.gz file. If an <index>.stg file is given, then each object listed in that STG file will be loaded and enumerated.

Also an input directory can be given, with --recurs (-r) to recurse into subdirectory finding all *.btg.gz and *.stg files, and each will be enumerated.

Of course the source of the flightgear scenery files, can be downloaded from <a target="_blank" href="http://www.flightgear.org/download/scenery/">scenery</a>, where they can be downloaded using a <a tareget="_blank" href="http://www.flightgear.org/legacy-Downloads/scenery-v2.12.html">grpahical interface</a>, from mirror sites, using BitTorrent, or purchased on a DVD.

Additionally a small sample of the scenery is included in the flightgear base data, <a target="_blank" href="https://sourceforge.net/p/flightgear/fgdata/ci/next/tree/">FGData</a>, or by using terrasync, when running the fgfs simulator. The full source of the scenery files is <a target="_blank" href="https://code.google.com/p/terrascenery/source/checkout">here</a>.

Essentially test-btg loads the <index>.btg.gz file into memory, and enumerates its contents, giving more information if the 'verbosity', -v2, -v5, -v9, is increased.

It has an additional option to write some of the contents to an xgraph like file, -x out-file.xg, which can then be viewed in <a target="_blank" href="https://sites.google.com/site/polyview2d/">PolyView2D</a>. A slightly update source of PolyView2D is part of this repo <a target="_blank" href="https://gitlab.com/fgtools/osm2xg">osm2xg</a> ... in effect this produces a 2D rendering of the BTG file... I would love some day to render them in 3D.

Another tool which loads btg files is the Map component of <a target="_blank" href="http://wiki.flightgear.org/Atlas">Atlas/Map</a>, or <a target="_blank" href="http://atlas.sourceforge.net/">About Atlas</a> and writes them out to jpeg or png image files tobe loaded by Atlas. This repo <a target="_blank" href="https://gitlab.com/fgtools/atlas-g">Atlas-g</a> is my updated source of this tool.

Have FUN!

; eof
