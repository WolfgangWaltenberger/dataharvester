all: oneevent twoevents

oneevent:
	dataharvester-converter -fEvents 1event.root 1event.txt

twoevents:
	dataharvester-converter -fEvents 2events.root 2events.txt

show:
	dataharvester-converter -fEvents 1event.root color_out

vertices:
	./cmssw_vtces.py

tracks:
	./cmssw_trks.py

compile:
	gcc `root-config --libs --cflags` run.cc ; a.out

test:
	g++ `pkg-config dataharvester --libs --cflags` test.cc
	./a.out
