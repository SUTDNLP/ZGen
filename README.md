ZGen
====

## Introduction

ZGen is a linearization system that constructs natural language sentences
from bags of words, given optional input syntactic constraints. Depending
on the amount of input constraints, ZGen can perform free ordering, partial
tree linearization and full tree linearization.

## Compiling

### Prerequisites

ZGen is developed with boost and compiled by cmake, along with some C++ 0x
features. To compile ZGen, the following software packages is required:

- gcc 4.5 (or above): for C++ 0x lambdas
- boost 1.57 (or above)
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

ZGen accepts different amount of syntactic constraints. These constraints includes the `Postagging`
and `(partial) Dependency parse structure`. Constraints are input in a 4 columns data format, which
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

> **NOTE**: if the input contains syntactic constraints, you need to specify the constraint type by
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

ZGen accepts another form of constraint which is postag dictionary. The dictionary
is used to indicate the possible tags for a certain word. Postag dictionary is in
the form of `[word] [postag]`

~~~{.bash}
Does NNP
Does VBZ
hanging VBG       
~~~

Postag dictionary is an optional resource to constrain the postag for each token, 
but it's highly recommanded because it will reduce the search space by magnitude, 
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

The option <code>--type</code> is used to specify the constraint type during the
decoding. If your input constraint don't contains dependency relations, please
use <code>none</code>; If your input contains part of but not full dependency
relations, you need to use <code>partial</code>; If your input constrains
contains the full dependency relations, please use <code>full</code>.

For details decription of the command line options, please run:

~~~{.bash}
./bin/ZGen_SR test --help
~~~

Sample scripts to train and test ZGen are available at /run/train-unlabeled-nopos-nodep.sh

## Combine N-gram model

If you obtain a large N-gram language model, which can be trained by KenLM tool,
you can run ZGen in following commands:

~~~{.bash}
./bin/ZGen_SR test \
	--type none \
	--model /path/to/your/model \
	--ngram /path/to/your/ngram/model \
	--input /path/to/your/input_file \
	--posdict /path/to/your/postag_dict
~~~

## Author

- Yue Zhang (yue_zhang@sutd.edu.sg) developed the prototype for ZGen best-first
and transition-based linearizer.
- Linfeng Song developed the joint morphlogy and generation system on ZGen:BestFrist
linearizer and developed the initial version of ZGen:BestFrist linearizer.
- Yijia Liu (oneplus.lau@gmail.com) refine the transition-based linearizer and
 formalized the codes.
- Jiangming Liu (jmliunlp@gmail.com) combine the N-gram model into the system.
- Ratish Puduppully(ratish.surendran@research.iiit.ac.in) added lookahead features.
- Current maintainers for ZGen are Jiangming Liu and Ratish Puduppully.

## Publication
- Ratish Puduppully, Yue Zhang and Manish Shrivastava. 2016. Transition-Based Syntactic Linearization with Lookahead Features. _In Proceedings of NAACL 2016 (To Appear)_. San Diego, USA, June.
- Jiangmign Liu. 2015. An Empirical Comparision between N-gram and Syntax Language Model for Word Ordering. _In Proceedings of EMNLP2015_. Lisbon, Portugal, September.
- Yijia Liu, Yue Zhang, Wanxiang Che, and Ting Liu. 2015. Transition-Based Syntactic Linearization. _In Proceedings of NAACL2015_. Denver, Colorado, USA, May.
- Linfeng Song, Yue Zhang, Kai Song and Qun Liu. 2014. Joint Morphological Generation and Syntactic Linearization. _In Proceedings of AAAI 2014_. Quebec City, Canada, July.

## Licences

Please refer the LICENSES file.
