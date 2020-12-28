swf2svg
=======

This is the recovered code for the dead [swf2svg project](http://swf2svg.sourceforge.net/) by "Ali G". The code is no longer available on Sourceforge, however it was possible to [extract it](https://gist.github.com/Jonty/2376f46818462345fdc81e029b62ce57) from the [Doxygen documentation](http://swf2svg.sourceforge.net/azar/doc/index.html) that is still available.

This software will extract any vector graphics from a given Macromedia Flash SWF file and convert them into an SVG.

Building
--------
`g++ *.cpp interface/*.cpp request/*.cpp vector/*.cpp -lz -o swf2svg`

Usage
-----
`./swf2svg zombocom.swf output.svg`

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
* Some SVG files mysteriously produce no output
* I don't know why this is called Azarakhsh
