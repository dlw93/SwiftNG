# SwiftNG
SwiftNG is a software made to estimate two workflows' similarity using *n*-grams as introduced in the [Paper](http://ceur-ws.org/Vol-1670/paper-50.pdf).

## Background
This project was started in the context of my bachelor's thesis on *Graph Similarity for Scientific Workflow Similarity Search* at the chair of *[Knowledge Management in Bioinformatics](https://www.informatik.hu-berlin.de/de/forschung/gebiete/wbi)* (Humboldt University of Berlin). It was further refined and extended during subsequent research and finally presented at *[LWDA 2016](http://hpi.de/en/mueller/lwda-2016.html)*.

## Technical Notes
The project is developed on Windows using Visual Studio and compiled with the Intel C Compiler, although only few optimisations would be necessary to get it to compile on UNIX/Linux with gcc. 

The project relies on only two libraries apart from the C standard library:

* [cJSON](https://github.com/DaveGamble/cJSON)
* [SQLite](https://www.sqlite.org/)

## Instructions
Before being able to compare workflows, an index containing all workflows and their matchings needs to be built. The required files can be gathered from the [FlowAlike](http://flowalike.informatik.hu-berlin.de) website.

To build the index, call 

```SwiftNG --build-index --index=<path_to_index> --workflows=<path_to_workflows> --mappings=<path_to_matchings>```

To load the index and enter the REPL, call

```SwiftNG --compare --index=<path_to_index>```

The program is now waiting for input in the format ```fn n a b``` with ```fn``` being the *n*-gram pattern to use (currently, only *fn=0* is supported, ```n``` specifying the size of the *n*-grams and ```a, b``` being the two workflows' [myExperiment](http://www.myexperiment.org/home) ids.
