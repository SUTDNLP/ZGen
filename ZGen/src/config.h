/*! \mainpage ZGen
 * ZGen
 * ====
 * 
 * # Introduction
 * 
 * ZGen is a linearization system that constructs natural lnaguage sentences
 * from bags of words, given optional input syntactic constraints. Depending
 * on the amount of input constraints, ZGen can perform free ordering, partial
 * tree linearization and full tree linearization.
 * 
 * # Compiling
 * 
 * ## Prerequiest
 * 
 * ZGen is developed with boost and compiled by cmake, along with some C++ 0x
 * features. To compile ZGen, the following software packages is required:
 * 
 * - gcc 4.5 (or above): for C++ 0x lambdas
 * - boost 1.55 (or above)
 * - cmake 2.8.10
 * 
 * ## Compiling
 * 
 * The compiling commands is
 * 
 * Run the following commands to compile on unix.
 * 
 * ~~~{.bash}
 * ./configure
 * make
 * ~~~
 * 
 * After successfully compiling, you will get 3 executable files under the bin
 * folder. They are:
 * 
 * - ./bin/zgen-sr: The transition-based sentence linearizer.
 * - ./bin/zgen-bf: The best first sentence linearizer.
 * 
 * # Usage
 * 
 * ## Input Data
 * 
 * ZGen accept different mount of syntactic constrains. These constraints includes
 * the Postagging and Dependency parse structure. Constrain are input in a 4 columns
 * data format, which means forms, postag, head, deprel. If certain lot (postag,
 * head, deprel) is filled in the input file, it will be used to constrain the
 * decoding process. The following example give an illustration to the barely
 * constrainted input.
 * 
 * ~~~{.bash}
 * __Ms._Haag__ _ -1 _
 * plays _ -1 _
 * __Elianti__ _ -1 _
 * . _ -1 _
 * ~~~
 * 
 * And this example is an illustration for the half postag and half dependency
 * constrainted input.
 * 
 * ~~~{.bash}
 * __Ms._Haag__ _ 1 N
 * plays _ -1 N
 * __Elianti__ NP -1 _
 * . . 1 N
 * ~~~
 * 
 * ## Reference Data (for training.)
 * 
 * If you want to train your own linearizing model with zgen, you need to prepare
 * your reference data. The reference data should be a well-formed 4 columns
 * dependency tree, as shown below.
 * 
 * ~~~{.bash}
 * _Ms._Haag__ NP 1 N
 * plays VBZ -1 N
 * __Elianti__ NP 1 N
 * . . 1 N
 * ~~~
 * 
 * ## Model
 * 
 * Before performing sentence linearization, you need to get the corresponding model.
 * We have provide several pretrained model on English (PennTreebank). You can
 * download it from [here](https://sourceforge.net/projects/zgen/files/?source=navbar).
 * 
 * ## Running ZGen
 * 
 * After preparing the data and model file (we suppose you put the model file in
 * /path/to/your/model), you can run ZGen in following commands:
 * 
 * ~~~{.bash}
 * ./bin/zgen-sr test -t none \
 * -t /path/to/your/model \
 * -i /path/to/your/input_file \
 * -p /path/to/your/postag_dict
 * ~~~
 * 
 * The option <code>-t</code> is used to specify the constrain type during the
 * decoding. If your input constrain don't contains dependency relations, please
 * use <code>none</code>; If your input contains part of but not full dependency
 * relations, you need to use <code>partial</code>; If your input constrains
 * contains the full dependency relations, please use <code>full</code>.
 * 
 * The option <code>-p</code> is used to specify the postag dict. It's an optional
 * resource to constrain the postag for each token, but it's highly recommanded
 * because it will magnitudely reduce the searching space, thus speed up the
 * decoding.
 * 
 * # Author
 * 
 * - Yue Zhang (yue_zhang@sutd.edu.sg) developed the prototype for ZGen best-first
 * and transition-based linearizer.
 * - Linfeng Song developed the joint morphlogy and generation system on ZGen:BestFrist
 * linearizer and developed the initial version of ZGen:BestFrist linearizer.
 * - Yijia Liu (oneplus.lau@gmail.com) refine the transition-based linearizer and
 * formalized the codes.
 * - Current maintainer for ZGen is Yijia Liu.
 * 
 * # Licences
 * 
 * Please refer the LICENSES file.
 */
#ifndef __ZGEN_CONFIG_H__
#define __ZGEN_CONFIG_H__

#define ZGEN_VERSION  "0.2.0"

#endif  //  end for __ZGEN_CONFIG_H__
