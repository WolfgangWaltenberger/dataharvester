all: write read

write:
	dataharvester-systemwriter -f test.dhf
	cat test.dhf

read:
	dataharvester-converter test.dhf color_out
