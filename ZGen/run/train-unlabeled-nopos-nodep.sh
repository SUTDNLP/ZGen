#!/bin/bash

ROOT=`pwd`

TRAIN_DEP=${ROOT}/data/train/train-unlabeled.dep
TRAIN_IN=${ROOT}/data/input/train-nopos-nodep.dep

TEST_TOK=${ROOT}/data/test/testr.tok
TEST_IN=${ROOT}/data/input/test-nopos-nodep.dep

DEV_TOK=${ROOT}/data/dev/devr.tok
DEV_IN=${ROOT}/data/input/dev-nopos-nodep.dep

SIG=`date '+%Y-%m-%d-%H%M%S'`-unlabeled-nopos-nodep

DIC=${ROOT}/data/dic/dic.7

WORKSPACE=${ROOT}/workspace/${SIG}

MODEL_DIR=${WORKSPACE}/model
OUTPUT_DIR=${WORKSPACE}/output
TMP_DIR=${WORKSPACE}/tmp
EXE_DIR=${WORKSPACE}/bin

mkdir -p ${MODEL_DIR} ${OUTPUT_DIR} ${EXE_DIR} ${TMP_DIR}

MODEL_PREFIX=${MODEL_DIR}/model

EXE=${EXE_DIR}/ZGen_SR.${SIG}
cp ${ROOT}/bin/ZGen_SR ${EXE}
rm ${MODEL_PREFIX}.*

for i in `seq 1 50`; do
    ${EXE} learn \
        --type      none   \
        --model     ${MODEL_PREFIX} \
        --posdict   ${DIC}          \
        --reference ${TRAIN_DEP}    \
        --input     ${TRAIN_IN}

    cp ${MODEL_PREFIX} ${MODEL_PREFIX}.${i}
    bzip2 ${MODEL_PREFIX}.${i} &

    ${EXE} test \
        --type      none     \
        --model     ${MODEL_PREFIX}  \
        --posdict   ${DIC}           \
        --input     ${DEV_IN}        \
        --output    ${OUTPUT_DIR}/devo.dep.${i}

    ${EXE} test \
        --type      none     \
        --model     ${MODEL_PREFIX}  \
        --posdict   ${DIC}           \
        --input     ${TEST_IN}       \
        --output    ${OUTPUT_DIR}/testo.dep.${i}

    ./scripts/getSentFromDeptree.py ${OUTPUT_DIR}/testo.dep.${i} > ${OUTPUT_DIR}/testo.tok.${i}
    ./scripts/getSentFromDeptree.py ${OUTPUT_DIR}/devo.dep.${i} > ${OUTPUT_DIR}/devo.tok.${i}

    echo "test `./ScoreBLEU.sh -t ${OUTPUT_DIR}/testo.tok.${i} -r ${ROOT}/data/test/testr.tok -odir ${TMP_DIR}`"
    echo "dev  `./ScoreBLEU.sh -t ${OUTPUT_DIR}/devo.tok.${i}  -r ${ROOT}/data/dev/devr.tok   -odir ${TMP_DIR}`"
done
