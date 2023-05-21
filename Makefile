# project: Affine cipher
# author: xmikul69
# course: KRY @ FIT VUT
# 2023

all:
	gcc kry.c -o kry -O3 -fopenmp -Werror -Wall

zip:
	rm 221674.zip
	zip 221674.zip Makefile kry.c doc.md testAll.sh
