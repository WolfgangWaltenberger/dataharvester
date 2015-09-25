#!/usr/bin/python

"""
.. module:: dataharvester_converter.py
   :synopsis: a script used to convert files from one format to another. 
              FIXME does currently implement all functionality of
              the old converter.cc file. Thats why the cc source file
              is left in the repository.

.. moduleauthor:: Wolfgang Waltenberger <wolfgang.waltenberger@gmail.com>

"""

import dataharvester

def convert ( infiles, outfile ):
    for i in infiles: print i,
    print "->",outfile
    w=dataharvester.Writer_file ( outfile )
    for infile in infiles:
        r=dataharvester.Reader_file ( infile )
        try:
            while True: 
                tpl=r.next()
                w.save(tpl)
        except StopIteration,e:
            pass
    dataharvester.Writer_close()

if __name__ == '__main__':
    import argparse, types
    argparser = argparse.ArgumentParser(description=
            'convert n input files into one output file')
    argparser.add_argument('infiles', type=types.StringType, nargs='+',
       help='input file names' )
    argparser.add_argument('outfile', type=types.StringType, nargs=1,
       help='output file name' )
    argparser.add_argument( '-V','--version', action='version', version=dataharvester.version() )
    args=argparser.parse_args()

    convert ( args.infiles, args.outfile[0] )

