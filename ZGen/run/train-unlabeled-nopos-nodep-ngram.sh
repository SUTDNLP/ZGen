#!/bin/bash

ROOT=`pwd`

TRAIN_DEP=${ROOT}/$1".dep"
TRAIN_IN=${ROOT}/$1".dep.unlabel"

TEST_TOK=${ROOT}/$2".tok"
TEST_IN=${ROOT}/$2".dep.unlabel"

DEV_TOK=${ROOT}/$3".tok"
DEV_IN=${ROOT}/$3".dep.unlabel"

NGRAM=${ROOT}/$4

SIG=`date '+%Y-%m-%d-%H%M%S'`-unlabeled-nopos-nodep

WORKSPACE=${ROOT}/workspace${5}/${SIG}

MODEL_DIR=${WORKSPACE}/model
OUTPUT_DIR=${WORKSPACE}/output
TMP_DIR=${WORKSPACE}/tmp
EXE_DIR=${WORKSPACE}/bin

mkdir -p ${MODEL_DIR} ${OUTPUT_DIR} ${EXE_DIR} ${TMP_DIR}

MODEL_PREFIX=${MODEL_DIR}/model

EXE=${EXE_DIR}/ZGen_SR.${SIG}
cp ${ROOT}/ZGen_SR ${EXE}
rm ${MODEL_PREFIX}.*

for i in `seq 1 30`; do
    time ${EXE} learn \
        --type      none   \
        --model     ${MODEL_PREFIX} \
		--ngram     ${NGRAM}            \
        --reference ${TRAIN_DEP}    \
        --input     ${TRAIN_IN}

    cp ${MODEL_PREFIX} ${MODEL_PREFIX}.${i}
    bzip2 ${MODEL_PREFIX}.${i} &

    time ${EXE} test \
        --type      none     \
        --model     ${MODEL_PREFIX}  \
		--ngram		${NGRAM}   \
        --input     ${DEV_IN}        \
        --output    ${OUTPUT_DIR}/devo.dep.${i}

    time ${EXE} test \
        --type      none     \
        --model     ${MODEL_PREFIX}  \
		--ngram		${NGRAM}    \
        --input     ${TEST_IN}       \
        --output    ${OUTPUT_DIR}/testo.dep.${i}

    ./Zpar2Tok.py ${OUTPUT_DIR}/testo.dep.${i} > ${OUTPUT_DIR}/testo.tok.${i}
    ./Zpar2Tok.py ${OUTPUT_DIR}/devo.dep.${i} > ${OUTPUT_DIR}/devo.tok.${i}

    echo "test `./ScoreBLEU.sh -t ${OUTPUT_DIR}/testo.tok.${i} -r ${TEST_TOK} -odir ${TMP_DIR}`"
    echo "dev  `./ScoreBLEU.sh -t ${OUTPUT_DIR}/devo.tok.${i}  -r ${DEV_TOK}  -odir ${TMP_DIR}`"
done
