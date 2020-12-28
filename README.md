swf2svg
=======

This is the recovered code for [swf2svg](http://swf2svg.sourceforge.net/) by "Ali G". The code is no longer available on Sourceforge, however it was possible to [extract it](https://gist.github.com/Jonty/2376f46818462345fdc81e029b62ce57) from the [Doxygen documentation](http://swf2svg.sourceforge.net/azar/doc/index.html) that is still available.

This software will extract any vector graphics from a given Macromedia Flash SWF file and convert them into an SVG.

Building
--------
`g++ *.cpp interface/*.cpp request/*.cpp vector/*.cpp -lz -o swf2svg`

Usage
-----
`./swf2svg zombocom.swf` will write `zombocom.svg` in the same path

**Note:** For no obvious reason this crashes randomly, you may need to run it a few times

Documentation
-------------
```
./swf2svg --help
Welcome to Azarakhsh version: 0.4.0 (build 178) on May 10, 2006

Usage:
	./swf2svg <inputfile> [<outputfile>] [option1] [option2] ...

	Options:

		--log: Enable logging details of the conversion in "log.txt"
		--nodots: Disables progression dots to be displayed
		--verbose: Shows details of the conversion on the console
		--onlypaths, --genpaths:
		             Generate only paths of the flash file with no
		             fill colours (for demonstration only)
		--decompress:
		             Decompresses the flash file (if compressed) in
		             "decompress.swf" with no other conversion

Terminating ...
```

Notes
-----
* Originally this [refused to load any files with "swf" in the name](https://github.com/Jonty/swf2svg/commit/f121206ca8eb0c8906f2e64b90dbd58c173556ca)
* This code crashes 50% of the time for no obvious reason
* I don't know why it is called Azarakhsh
