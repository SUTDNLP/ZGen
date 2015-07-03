ZGen
====

## Introduction

ZGen is a linearization system that constructs natural lnaguage sentences
from bags of words, given optional input syntactic constraints. Depending
on the amount of input constraints, ZGen can perform free ordering, partial
tree linearization and full tree linearization.

## Compiling

### Prerequiest

ZGen is developed with boost and compiled by cmake, along with some C++ 0x
features. To compile ZGen, the following software packages is required:

- gcc 4.5 (or above): for C++ 0x lambdas
- boost 1.55 (or above)
- cmake 2.8.10

### Make

The compiling commands is

Run the following commands to compile on unix.

~~~{.bash}
./configure
make
~~~

After successfully compiling, you will get 2 executable files under the bin
folder. They are:

- ./bin/ZGen_SR: The transition-based linearizer.
- ./bin/unittest: The unittest suites.

## Usage

### Input Data

ZGen accepts different amount of syntactic constrains. These constraints includes the `Postagging`
and `(partial) Dependency parse structure`. Constrain are input in a 4 columns data format, which
means forms, postag, head, deprel. If certain slot (postag, head, deprel) is filled in the input
data, it will be used to constrain the decoding process. The following example give an illustration
to the input without any syntactic constrain:

~~~{.bash}
__Ms._Haag__ _ -1 _
plays _ -1 _
__Elianti__ _ -1 _
. _ -1 _
~~~

And this example is an illustration for the input with partial postag and dependency constraint.

~~~{.bash}
__Ms._Haag__ _ 1 N
plays _ -1 N
__Elianti__ NP -1 _
. . 1 N
~~~
which means, the token `__Ms._Haag__` should be the modifier of `__Elianti__`, and `__Elianti__`
should be tagged as `NP` in the final output.

> **NOTE**: if the input contains syntactic constrains, you need to specify the constrain type by
> the `--type` command line options.

### Reference Data (for training.)

If you want to train your own linearizing model with zgen, you need to prepare
your reference data. The reference data should be a well-formed 4 columns
dependency tree, as shown below.

~~~{.bash}
__Ms._Haag__ NP 1 N
plays VBZ -1 N
__Elianti__ NP 1 N
. . 1 N
~~~

### Noun Phrases

ZGen accepts the prerecognized noun phrases. A special prefix `__` and suffix
`__` should be added to the words in the phrase.

### Postag Dictionary

ZGen accepts another form of constrain which is postag dictionary. The dictionary
is used to indicate the possible tags for a certain word. Postag dictionary is in
the form of `[word] [postag]`

~~~{.bash}
Does NNP
Does VBZ
hanging VBG       
~~~

Postag dictionary is an optional resource to constrain the postag for each token, 
but it's highly recommanded because it will magnitudely reduce the searching space, 
thus speed up the decoding.

## Model

Before performing sentence linearization, you need to get the corresponding model.
We have provide several pretrained model on English (PennTreebank). You can
download it from [here](https://sourceforge.net/projects/zgen/files/?source=navbar).

## Running ZGen

After preparing the data and model file (we suppose you put the model file in 
/path/to/your/model), you can run ZGen in following commands:

~~~{.bash}
./bin/ZGen_SR test \
        --type  none \
        --model /path/to/your/model \
        --input /path/to/your/input_file \
        --posdict /path/to/your/postag_dict
~~~

The option <code>--type</code> is used to specify the constrain type during the
decoding. If your input constrain don't contains dependency relations, please
use <code>none</code>; If your input contains part of but not full dependency
relations, you need to use <code>partial</code>; If your input constrains
contains the full dependency relations, please use <code>full</code>.

For details decription of the command line options, please run:

~~~{.bash}
./bin/ZGen_SR test --help
~~~

## Author

- Yue Zhang (yue_zhang@sutd.edu.sg) developed the prototype for ZGen best-first
and transition-based linearizer.
- Linfeng Song developed the joint morphlogy and generation system on ZGen:BestFrist
linearizer and developed the initial version of ZGen:BestFrist linearizer.
- Yijia Liu (oneplus.lau@gmail.com) refine the transition-based linearizer and
 formalized the codes.
- Current maintainer for ZGen is Yijia Liu.

## Publication
- Yijia Liu, Yue Zhang, Wanxiang Che, and Ting Liu. 2015. Transition-Based Syntactic Linearization. (To appear) _In Proceedings of NAACL2015_. Denver, Colorado, USA, May.
- Linfeng Song, Yue Zhang, Kai Song and Qun Liu. 2014. Joint Morphological Generation and Syntactic Linearization. _In Proceedings of AAAI 2014_. Quebec City, Canada, July.

## Development (v0.2.0)

- The BestFirst sentence generator described in Song et al, (2014) is still under construction.
- Multi-threaded training is on board and will be delivered in future version.

## Licences

Please refer the LICENSES file.
